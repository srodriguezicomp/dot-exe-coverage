#ifndef SERVERUTILS_HPP
#define SERVERUTILS_HPP
#include <string>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

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
mongocxx::collection getCollectionByName(
    const std::string& m_collectionName
);

/**
 * @brief
 * 
 * Extract values ​​from a json...
 * 
 * Example:
 * @code
 * functionName(); // Output:
 * @endcode
 * 
*/
std::string getValueFromJson(
    const std::string& m_json_str,
    const std::vector<std::string>& m_keys
);

#endif 