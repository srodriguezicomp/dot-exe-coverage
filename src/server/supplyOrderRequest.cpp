#include "supplyOrderRequest.hpp"
#include "serverGlobals.hpp"
#include "server.hpp"
#include "inventoryRequest.hpp"
#include "serverUtils.hpp"
#include "../dataAccess/mongoDbConnector.hpp"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>

std::string shipmentStatusToString(
    ShipmentStatus m_status
) {
    switch (m_status) {
        case ShipmentStatus::SENT_TO_ORIGIN: return "SENT_TO_ORIGIN";
        case ShipmentStatus::SHIPPED_TO_DESTINATION: return "SHIPPED_TO_DESTINATION";
        case ShipmentStatus::DELIVERED: return "DELIVERED";
        default: return "UNKNOWN";
    }
}

void sendSupplyRequest(
    int m_senderId, 
    int m_clientId, 
    const Inventory& m_inventory, 
    std::string& m_shipmentId, // Nota: paso por referencia para poder actualizarlo si se genera
    int m_fd,
    std::string m_messageType,
    std::string m_status
) {

    mongocxx::collection m_collection = getCollectionByName("shipments"); // función para obtener la colección

    // Si m_shipmentId está vacío, generar uno nuevo
    if (m_shipmentId.empty()) {
        m_shipmentId = std::to_string(generarNuevoId());
    }

    // Construir el documento a insertar o actualizar
    bsoncxx::builder::stream::document m_docBuilder;
    m_docBuilder 
        << "clientIdSource" << std::to_string(m_senderId)
        << "clientIdDestiny" << std::to_string(m_clientId)
        << "inventory" << bsoncxx::builder::stream::open_document
            << "waterBox" << m_inventory.m_waterBox
            << "weaponryBox" << m_inventory.m_weaponryBox
            << "foodBox" << m_inventory.m_foodBox
            << "medicineBox" << m_inventory.m_medicineBox
        << bsoncxx::builder::stream::close_document
        << "shipmentId" << std::stoi(m_shipmentId) // Lo guardamos como int en MongoDB
        << "status" << m_status;

    // Hacer upsert usando shipmentId como clave
    upsertDocument(
        m_collection,
        "shipmentId",
        bsoncxx::types::bson_value::value(std::stoi(m_shipmentId)),
        m_docBuilder.view()
    );

    // Armar el mensaje a enviar
    std::ostringstream m_messageStream;
    m_messageStream << "{"
                   << "\"messageType\": "<< m_messageType << ","
                   << "\"body\": {"
                   << "\"inventory\": {"
                   << "\"waterBox\": " << m_inventory.m_waterBox << ","
                   << "\"weaponryBox\": " << m_inventory.m_weaponryBox << ","
                   << "\"foodBox\": " << m_inventory.m_foodBox << ","
                   << "\"medicineBox\": " << m_inventory.m_medicineBox
                   << "},"
                   << "\"shipmentId\": " << m_shipmentId
                   << "}"
                   << "}";

    std::string m_message = m_messageStream.str();

    // Enviar el mensaje
    sendMessage(m_fd, m_message);
}

int handleSendRequest(
    std::vector<std::string> m_fieldsToRequest,
    Inventory m_inv,
    int m_senderId,
    int m_requestedQuantity
){
    mongocxx::collection m_collection = getCollectionByName("inventory");
    auto m_documents = getMayorPrioritized(
        m_collection,
        m_fieldsToRequest,
        (m_requestedQuantity + MIN_INVENTORY),
        "warehouse",
        MAX_CLIENTS_PRIORIZED
    );
    if(!m_documents.empty()){
        for (const auto& m_doc : m_documents) {
            // Obtener el campo "clientId" del documento
            auto m_clientIdElement = m_doc.view()["clientId"];
        
            if (!m_clientIdElement || m_clientIdElement.type() != bsoncxx::type::k_utf8) {
                std::cerr << "Documento sin clientId válido." << std::endl;
                continue;
            }
        
            std::string m_clientIdStr = m_clientIdElement.get_string().value.to_string();
        
            try {
                // Convertir el clientId de string a int
                int m_clientIdInt = std::stoi(m_clientIdStr);
        
                // Buscar si el cliente existe en el map de conexiones
                auto m_it = m_connections.find(m_clientIdInt);
                if (m_it != m_connections.end()) {
                    Client& m_client = m_it->second;
                    std::cout << "Cliente conectado encontrado: m_id=" << m_client.m_id 
                                << ", m_fd=" << m_client.m_fd 
                                << ", tipo=" << m_client.m_clientType << std::endl;
                    
                    std::string m_shipmentId = "";
                    sendSupplyRequest(
                        m_senderId,
                        m_client.m_id,
                        Inventory{ 
                            m_inv.m_waterBox + MIN_INVENTORY,
                            m_inv.m_weaponryBox + MIN_INVENTORY,
                            m_inv.m_foodBox + MIN_INVENTORY,
                            m_inv.m_medicineBox + MIN_INVENTORY
                        },
                        m_shipmentId,
                        m_it->first,
                        "SUPPLY_ORDER_REQUEST",
                        shipmentStatusToString(ShipmentStatus::SENT_TO_ORIGIN)
                    );
                    return 1;
                }
        
            } catch (const std::exception& m_e) {
                std::cerr << "Error al convertir clientId a entero: " << m_e.what() << std::endl;
                return 0;
            }
        }
    }
    return 0;
}

void handleSupplyOrderMessage(
    const std::string& m_msg,
    int m_senderId
){
    Inventory m_inv = getInventoryFromJson(m_msg);
    
    std::vector<std::string> m_requestedFields = validateInventory(m_inv, [](int m_val) {
        return m_val != 0;
    });
    int m_maxRequestedQuantity = std::max({
        m_inv.m_waterBox,
        m_inv.m_weaponryBox,
        m_inv.m_foodBox,
        m_inv.m_medicineBox
    });

    handleSendRequest(m_requestedFields, m_inv, m_senderId, m_maxRequestedQuantity);
}

void handleSupplyOrderConfirmation(
    const std::string& m_msg
){
    int m_shipmentId = std::stoi(getValueFromJson(m_msg, {"body", "shipmentId"}));

    auto m_collection = getCollectionByName("shipments");
    auto m_resultDoc = findShipment(m_collection,m_shipmentId);

    auto m_shipmentStatus = m_resultDoc.view()["status"].get_utf8().value.to_string();

    if(m_shipmentStatus == "SENT_TO_ORIGIN"){

        auto m_clientIdDestiny = std::stoi(m_resultDoc.view()["clientIdDestiny"].get_utf8().value.to_string());
        auto m_foodBox = m_resultDoc.view()["inventory"]["foodBox"].get_int32();
        auto m_medicineBox = m_resultDoc.view()["inventory"]["medicineBox"].get_int32();
        auto m_waterBox = m_resultDoc.view()["inventory"]["waterBox"].get_int32();
        auto m_weaponryBox = m_resultDoc.view()["inventory"]["weaponryBox"].get_int32();

        std::stringstream m_msg;
        m_msg  << "{"
            << "\"messageType\":\"SUPPLY_SEND\","
            << "\"body\":{"
                <<"\"inventory\":{"
                    << "\"foodBox\":" << m_foodBox << ","
                    << "\"medicineBox\":" << m_medicineBox << ","
                    << "\"waterBox\":" << m_waterBox << ","
                    << "\"weaponryBox\":" << m_weaponryBox << "},"
                << "\"shipmentId\":" << m_shipmentId << "}}";
        
        sendMessage(m_clientIdDestiny, m_msg.str());

        updateField(m_collection, "shipmentId", m_shipmentId, "status", "SHIPPED_TO_DESTINATION");

        // VER Actualizar el inventario del cliente de origen

    }else if(m_shipmentStatus == "SHIPPED_TO_DESTINATION"){

        // Actualizar el inventario del cliente de destino

        updateField(m_collection, "shipmentId", m_shipmentId, "status", "DELIVERED");

    }
}

