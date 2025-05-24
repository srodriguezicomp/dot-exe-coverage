#include "serverGlobals.hpp"

// Define constants
const int PORT = 12346;
const int MAX_CLIENTS = 1000;
const int MIN_INVENTORY = 4;
const std::string KEY = "1A2B3C4D5E6F708192A3B4C5D6E7F809";
const std::string IV = "0F1E2D3C4B5A69788796A5B4C3D2E1F0";
const std::string UNLOCK_KEY = "unlock_me";
const int MAX_CLIENTS_PRIORIZED = 10;

// Define global variables
std::unordered_map<int, Client> m_connections; // connectionId -> Client
std::unordered_map<int, int> m_remainingAttempts; // m_senderId -> remainingAttemps
std::unordered_map<int, int> m_requestMap; // requestId -> clientId
std::vector<int> m_blackListConnections; // Lista negra de clientes - clientes infectados
std::vector<int>  m_bannedConnections; // connectionId -> Client
int m_nextClientId = 1;

// MongoDB instance and client
mongocxx::instance m_instance{};
mongocxx::client m_client{mongocxx::uri{"mongodb://mongodb:27017"}};