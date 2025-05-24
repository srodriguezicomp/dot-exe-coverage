#include "alertRequest.hpp"
#include "serverGlobals.hpp"
#include "server.hpp"
#include "serverUtils.hpp"

void sendBroadcastAlert(
    const std::string& m_alertMsg, 
    const std::string& m_alertType
){
    std::stringstream m_alertToSend;
    m_alertToSend << "{\"messageType\":\"ALERT\",\"body\":{\"type\":\"" << m_alertType << "\",\"message\":\"" << m_alertMsg << "\"}}";
    for (const auto& m_pair : m_connections) {
        sendMessage(m_pair.first, m_alertToSend.str());  // Envía el mensaje al cliente
    }
}

void handleAlertRequest(
    int m_senderId, 
    const std::string& m_msg,
    const bool m_isInBlacklist
){
    std::string m_alertMessage = getValueFromJson(m_msg, {"body", "message"});
    std::string m_alertType = getValueFromJson(m_msg, {"body", "type"});
    if(m_senderId == -1){
        // Rutina en caso de que sea un sensor
        sendBroadcastAlert(m_alertMessage, m_alertType); // Envia un mensaje de alerta a todos los clientes conectados
    }else{
        if(m_isInBlacklist){
            m_blackListConnections.erase(std::remove(m_blackListConnections.begin(), m_blackListConnections.end(), m_senderId), m_blackListConnections.end()); // Elimina el id del cliente de la blacklist
        }else{
            // Rutina en caso que el warehouse NO este en la blacklist
            m_blackListConnections.push_back(m_senderId); // Agrega el id del cliente a la blacklist
            sendBroadcastAlert(m_alertMessage, m_alertType); // Envia un mensaje de alerta a todos los clientes conectados
        }
    }
}