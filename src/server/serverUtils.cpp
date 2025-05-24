#include "serverUtils.hpp"
#include "serverGlobals.hpp"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <cjson/cJSON.h>

mongocxx::collection getCollectionByName(
    const std::string& m_collectionName
) {
    auto m_db = m_client["dhl_db"];
    return m_db[m_collectionName];
}

std::string getValueFromJson(
    const std::string& m_json_str,
    const std::vector<std::string>& m_keys
){
    std::string m_value;

    // Parsear el JSON
    cJSON* m_root = cJSON_Parse(m_json_str.c_str());
    if (!m_root) {
        std::cerr << "Error parsing JSON" << std::endl;
        return "";
    }

    cJSON* m_current = m_root;

    // Recorrer las claves en el arreglo
    for (size_t i = 0; i < m_keys.size(); ++i) {
        m_current = cJSON_GetObjectItem(m_current, m_keys[i].c_str());
        if (!m_current) {
            std::cerr << "No se encontró el campo '" << m_keys[i] << "'" << std::endl;
            cJSON_Delete(m_root);
            return "";
        }
    }

    // Verificar si el último elemento es una cadena
    if (!cJSON_IsString(m_current)) {
        std::cerr << "El valor del campo no es una cadena" << std::endl;
        std::cout << m_current << std::endl;
        if (cJSON_IsNumber(m_current)) {
            std::cerr << "El valor del campo es un número: " << std::to_string(m_current->valueint) << std::endl;
        } else {
            std::cerr << "El valor del campo no es una cadena ni un número" << std::endl;
            cJSON_Delete(m_root);
            return "";
        }
        // cJSON_Delete(root);
        // return "";
    }

    // // Obtener el valor como cadena
    // std::string m_value = current->valuestring;

    // // Liberar la memoria del objeto JSON
    // cJSON_Delete(root);

    std::cerr << "Antes del if getValueFromJson" << std::endl;

    if (cJSON_IsString(m_current)) {
        m_value = m_current->valuestring;
    } else if (cJSON_IsNumber(m_current)) {
        m_value = std::to_string(m_current->valueint);
    } else {
        std::cerr << "El valor del campo no es una cadena ni un número" << std::endl;
        cJSON_Delete(m_root);
        return "";
    }

    return m_value;
}

