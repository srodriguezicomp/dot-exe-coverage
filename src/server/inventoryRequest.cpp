#include "inventoryRequest.hpp"
#include "serverGlobals.hpp"
#include "serverUtils.hpp"
#include "supplyOrderRequest.hpp"
#include <cjson/cJSON.h>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include "../dataAccess/mongoDbConnector.hpp"

Inventory getInventoryFromJson(
    const std::string& m_json_str
) {
    Inventory m_inventory = {0, 0, 0, 0}; // Valores iniciales por defecto

    // Parsear el JSON
    cJSON* m_root = cJSON_Parse(m_json_str.c_str());
    if (!m_root) {
        std::cerr << "Error parsing JSON" << std::endl;
        return m_inventory;
    }

    // Navegar a body -> inventory
    cJSON* m_body = cJSON_GetObjectItem(m_root, "body");
    if (!m_body) {
        std::cerr << "No se encontró el campo 'body'" << std::endl;
        cJSON_Delete(m_root);
        return m_inventory;
    }

    cJSON* m_inventoryJson = cJSON_GetObjectItem(m_body, "inventory");
    if (!m_inventoryJson) {
        std::cerr << "No se encontró el campo 'inventory'" << std::endl;
        cJSON_Delete(m_root);
        return m_inventory;
    }

    // Extraer los valores
    cJSON* m_waterBox = cJSON_GetObjectItem(m_inventoryJson, "waterBox");
    cJSON* m_weaponryBox = cJSON_GetObjectItem(m_inventoryJson, "weaponryBox");
    cJSON* m_foodBox = cJSON_GetObjectItem(m_inventoryJson, "foodBox");
    cJSON* m_medicineBox = cJSON_GetObjectItem(m_inventoryJson, "medicineBox");

    if (cJSON_IsNumber(m_waterBox)) m_inventory.m_waterBox = m_waterBox->valueint;
    if (cJSON_IsNumber(m_weaponryBox)) m_inventory.m_weaponryBox = m_weaponryBox->valueint;
    if (cJSON_IsNumber(m_foodBox)) m_inventory.m_foodBox = m_foodBox->valueint;
    if (cJSON_IsNumber(m_medicineBox)) m_inventory.m_medicineBox = m_medicineBox->valueint;

    cJSON_Delete(m_root);
    return m_inventory;
}

const std::vector<std::string> validateInventory(
    const Inventory& m_inventory, 
    std::function<bool(int)> m_validator
) {
    std::vector<FieldInfo> m_invalidFields;

    if (m_validator(m_inventory.m_waterBox)) {
        m_invalidFields.push_back({"waterBox", m_inventory.m_waterBox});
    }
    if (m_validator(m_inventory.m_weaponryBox)) {
        m_invalidFields.push_back({"weaponryBox", m_inventory.m_weaponryBox});
    }
    if (m_validator(m_inventory.m_foodBox)) {
        m_invalidFields.push_back({"foodBox", m_inventory.m_foodBox});
    }
    if (m_validator(m_inventory.m_medicineBox)) {
        m_invalidFields.push_back({"medicineBox", m_inventory.m_medicineBox});
    }

    std::sort(m_invalidFields.begin(), m_invalidFields.end(), [](const FieldInfo& m_a, const FieldInfo& m_b) {
        return m_a.m_value < m_b.m_value;
    });

    static std::vector<std::string> m_fields;
    m_fields.clear();
    for (const auto& m_field : m_invalidFields) {
        m_fields.push_back(m_field.m_name);
    }

    return m_fields;
}

void updateInventoryDocument(
    Inventory m_inv,
    int m_senderId
){
    bsoncxx::builder::stream::document m_inventoryBuilder;
    m_inventoryBuilder 
        << "waterBox" << m_inv.m_waterBox
        << "weaponryBox" << m_inv.m_weaponryBox
        << "foodBox" << m_inv.m_foodBox
        << "medicineBox" << m_inv.m_medicineBox;

    bsoncxx::builder::stream::document m_docBuilder;
    m_docBuilder 
        << "clientId" << m_connections[m_senderId].m_id
        << "inventory" << m_inventoryBuilder.view()
        << "clientType" << m_connections[m_senderId].m_clientType;

    mongocxx::collection m_collection = getCollectionByName("inventory");
    upsertDocument(
        m_collection,
        "clientId",
        bsoncxx::types::bson_value::value(m_connections[m_senderId].m_id),
        m_docBuilder.view()
    );
}

void handleInventoryUpdateMessage(
    const std::string& m_msg,
    int m_senderId
){
    Inventory m_inv = getInventoryFromJson(m_msg);
    
    std::vector<std::string> m_fieldsInvalid = validateInventory(m_inv, [](int m_val) {
        return m_val < MIN_INVENTORY;
    });
    if(m_fieldsInvalid.empty()){
        updateInventoryDocument(m_inv, m_senderId);
    }else{
        if(!handleSendRequest(m_fieldsInvalid, m_inv, m_senderId, MIN_INVENTORY)){
            updateInventoryDocument(m_inv, m_senderId);
        }
    }
}
