#ifndef ALERTREQUEST_HPP
#define ALERTREQUEST_HPP
#include <string>

/**
 * @brief
 * 
 * Sends an alert message to all connected clients...
 * 
 * Example:
 * @code
 * functionName(); // Output:
 * @endcode
 * 
*/
void sendBroadcastAlert(
    const std::string& m_alertMsg, 
    const std::string& m_alertType
);

/**
 * @brief
 * 
 * Function description...
 * 
 * Example:
 * @code
 * functionName(); // Output:
 * @endcode
 * 
*/
void handleAlertRequest(
    int m_senderId, 
    const std::string& m_msg,
    const bool m_isInBlacklist
);

#endif 