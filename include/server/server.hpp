#ifndef SERVER_HPP
#define SERVER_HPP
#include <string>
#include <uv.h>

/**
 * @brief
 * 
 * Function responsible for initializing the database collections...
 * 
 * Example:
 * @code
 * functionName(); // Output:
 * @endcode
 * 
*/
void runInitializeCollections();

/**
 * @brief
 * 
 * Sends a message to a client specifying the file descriptor (m_fd) and the message (m_msg)...
 * 
 * Example:
 * @code
 * functionName(); // Output:
 * @endcode
 * 
*/
void sendMessage(
    int m_senderId, 
    const std::string& m_msg
);

/**
 * @brief
 * 
 * Function responsible for defining the type of message received and taking the corresponding action...
 * Handles me-auth messages received from clients
 * It is passed the m_id of the client that sent the message and the message itself
 * Depending on the type of message, different actions are performed
 * 
 * Example:
 * @code
 * functionName(); // Output:
 * @endcode
 * 
*/
void handleMessage(
    int m_senderId, 
    const std::string& m_msg
);

/**
 * @brief
 * 
 * Function responsible for handling messages received from clients...
 * 
 * Example:
 * @code
 * functionName(); // Output:
 * @endcode
 * 
*/
void onRead(
    uv_poll_t* m_handle, 
    int m_status, 
    int m_events
);

/**
 * @brief
 * 
 * Configures sockets in non-blocking mode...
 * 
 * Example:
 * @code
 * functionName(); // Output:
 * @endcode
 * 
*/
void setNonBlocking(
    int m_fd
);

/**
 * @brief Corre el servidor.
 * 
 * Esta funcion corre el servidor.
 * 
 * Example:
 * @code
 * runServer(); // Output:
 * @endcode
 * 
*/
void runServer();

#endif 