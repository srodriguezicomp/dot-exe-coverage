#ifndef SUPPLYORDERREQUEST_HPP
#define SUPPLYORDERREQUEST_HPP
#include <string>
#include "inventoryRequest.hpp"
#include <vector>

// Enums
enum class ShipmentStatus {
    SENT_TO_ORIGIN,
    SHIPPED_TO_DESTINATION,
    DELIVERED
};

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
std::string shipmentStatusToString(
    ShipmentStatus m_status
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
void sendSupplyRequest(
    int m_senderId, 
    int m_clientId, 
    const Inventory& m_inventory, 
    std::string& m_shipmentId, // Nota: paso por referencia para poder actualizarlo si se genera
    int m_fd,
    std::string m_messageType,
    std::string m_status
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
int handleSendRequest(
    std::vector<std::string> m_fieldsToRequest,
    Inventory m_inv,
    int m_senderId,
    int m_requestedQuantity
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
void handleSupplyOrderMessage(
    const std::string& m_msg,
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
void handleSupplyOrderConfirmation(
    const std::string& m_msg
);

#endif 

