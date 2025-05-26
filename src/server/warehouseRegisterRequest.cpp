#include "warehouseRegisterRequest.hpp"
#include "serverGlobals.hpp"
#include "authRequest.hpp"
#include "serverUtils.hpp"

void handleWarehouseRegisterRequest(
    const std::string& m_msg
){
    std::string m_user = getValueFromJson(m_msg, {"body", "user"});
    std::string m_password = getValueFromJson(m_msg, {"body", "password"});
    createUser(m_user, m_password, "warehouse");
    std::cout << "warehouse_register | user " << m_user << " | password " << m_password << std::endl;
}