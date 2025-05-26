#ifndef SERVERGLOBALS_HPP
#define SERVERGLOBALS_HPP
#include <unordered_map>
#include <vector>
#include <string>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include "server.hpp"

// Structs
struct Client {
    int m_id;
    int m_fd;
    std::string m_clientType;
};

// Declare constants
extern const int PORT;
extern const int MAX_CLIENTS;
extern const int MIN_INVENTORY;
extern const std::string KEY;
extern const std::string IV;
extern const std::string UNLOCK_KEY;
extern const int MAX_CLIENTS_PRIORIZED;

// Declare global variables
extern std::unordered_map<int, Client> m_connections;
extern std::unordered_map<int, int> m_remainingAttempts;
extern std::unordered_map<int, int> m_requestMap;
extern std::vector<int> m_blackListConnections;
extern std::vector<int> m_bannedConnections;
extern int m_nextClientId;

// Declare MongoDB instance and client
extern mongocxx::instance m_instance;
extern mongocxx::client m_client;

#endif