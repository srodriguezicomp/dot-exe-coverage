#ifndef INVENTORYREQUEST_HPP
#define INVENTORYREQUEST_HPP
#include <string>
#include <vector>
#include <functional>

// Structs
struct Inventory {
    int m_waterBox;
    int m_weaponryBox;
    int m_foodBox;
    int m_medicineBox;
};

struct FieldInfo {
    std::string m_name;
    int m_value;
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
Inventory getInventoryFromJson(
    const std::string& m_json_str
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
const std::vector<std::string> validateInventory(
    const Inventory& m_inventory, 
    std::function<bool(int)> m_validator
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
void updateInventoryDocument(
    Inventory m_inv,
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
void handleInventoryUpdateMessage(
    const std::string& m_msg,
    int m_senderId
);

#endif 