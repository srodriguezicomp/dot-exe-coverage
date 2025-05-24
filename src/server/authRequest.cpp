#include "authRequest.hpp"
#include "serverGlobals.hpp"
#include "../dataAccess/mongoDbConnector.hpp"
#include "server.hpp"
#include "serverUtils.hpp"

void banClient(
    int m_senderId
){
    m_bannedConnections.push_back(m_senderId);
}

int createUser(
    std::string m_clientUser, 
    std::string m_clientPassword, 
    std::string m_clientType
){
   
    // Obtén la colección desde la base de datos
    auto m_collection = getCollectionByName("users");

    // Define los datos del usuario
    std::string m_user = m_clientUser;
    std::string m_password = m_clientPassword;
    std::string m_type = m_clientType;

    // Llama a la función insertUser
    int m_newId = insertUser(m_collection, m_user, m_password, m_type, KEY, IV);

    std::cout << "Usuario insertado correctamente." << std::endl;
    return m_newId;
}

void handleAuthRequest(
    int m_senderId, 
    const std::string& m_msg
){

    std::cout << "AUTENTICATION_REQUEST desde cliente "<< m_senderId <<".\n";
    std::string m_user = getValueFromJson(m_msg, {"body", "user"});
    std::string m_password = getValueFromJson(m_msg, {"body", "password"});

    auto m_collection = getCollectionByName("users");
    std::stringstream m_messageToSend;
    auto m_resultDoc = findUser(m_collection,m_user, KEY, IV);

    if (m_resultDoc.view().empty()) {
        std::cout << "Usuario nuevo conectado." << std::endl;
        int m_newId = createUser(
            m_user, 
            m_password, 
            "hub"
        );
        m_messageToSend << "{\"messageType\":\"AUTENTICATION_CONFIRM\",\"body\":{\"status\":\""<< "OK" << "\",\"clientType\":\"" << "hub" << "\"}}";
        // Envia al clientId de destino la cantidad de waterBox de value
        sendMessage(m_senderId, m_messageToSend.str());
        m_connections[m_senderId].m_id = m_newId;
        m_connections[m_senderId].m_clientType = "hub";
    }else{
        std::cout << "Usuario ya registrado intentando conectarse." << std::endl;
        auto m_typeValue = m_resultDoc.view()["type"].get_string().value.to_string();
        auto m_passwordBd = m_resultDoc.view()["password"].get_string().value.to_string();
        
        if(m_passwordBd == m_password){
            std::cout << "Password correcta." << std::endl;
            m_messageToSend << "{\"messageType\":\"AUTENTICATION_CONFIRM\",\"body\":{\"status\":\""<< "OK" << "\",\"clientType\":\"" << m_typeValue << "\"}}";
            // Envia al clientId de destino la cantidad de waterBox de value
            sendMessage(m_senderId, m_messageToSend.str());
            m_connections[m_senderId].m_id = m_resultDoc.view()["id"].get_int32();
            m_connections[m_senderId].m_clientType = m_typeValue;
            auto m_it = m_remainingAttempts.find(m_senderId);
            if (m_it != m_remainingAttempts.end()) {
                m_remainingAttempts.erase(m_it);
            }
        }else{
            std::cout << "Password incorrecta." << std::endl;
            auto m_it = m_remainingAttempts.find(m_senderId);

            if (m_it != m_remainingAttempts.end()) {
                // El m_senderId ya existe en el map
                if (m_it->second > 1) {
                    // Si tiene más de 1 intento restante, restarle uno
                    m_it->second -= 1;
                    m_messageToSend << "{\"messageType\":\"PASSWORD_FAILED\",\"body\":{\"status\":\""<< "NOK" << "\",\"remainingAttempts\":\"" << m_remainingAttempts[m_senderId] << "\"}}";
                    sendMessage(m_senderId, m_messageToSend.str());
                } else {
                    std::cout << "Limites de intentos alcanzados." << std::endl;
                    m_messageToSend << "{\"messageType\":\"PASSWORD_FAILED\",\"body\":{\"status\":\""<< "NOK" << "\",\"remainingAttempts\":\"" << m_remainingAttempts[m_senderId] << "\"}}";
                    sendMessage(m_senderId, m_messageToSend.str());
                    // Si tiene 1 o 0 intentos restantes, eliminar del map
                    m_remainingAttempts.erase(m_it);
                    banClient(m_senderId);
                }
            } else {
                // Si no existe, agregarlo con valor 2
                m_remainingAttempts[m_senderId] = 2;
                m_messageToSend << "{\"messageType\":\"PASSWORD_FAILED\",\"body\":{\"status\":\""<< "NOK" << "\",\"remainingAttempts\":\"" << 2 << "\"}}";
                sendMessage(m_senderId, m_messageToSend.str());
            }
        }
    }
}