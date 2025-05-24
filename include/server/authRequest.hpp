#ifndef AUTHREQUEST_HPP
#define AUTHREQUEST_HPP
#include <string>

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
void banClient(
    int m_senderId
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
int createUser(
    std::string m_clientUser, 
    std::string m_clientPassword, 
    std::string m_clientType
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
void handleAuthRequest(
    int m_senderId, 
    const std::string& m_msg
);

#endif 