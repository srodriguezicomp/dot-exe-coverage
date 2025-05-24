#include <iostream>
#include <unordered_map>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <uv.h>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include "../dataAccess/mongoDbConnector.hpp"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <cjson/cJSON.h>
#include "server.hpp"
#include "serverGlobals.hpp"
#include "serverUtils.hpp"
#include "alertRequest.hpp"
#include "authRequest.hpp"
#include "inventoryRequest.hpp"
#include "supplyOrderRequest.hpp"
#include "warehouseRegisterRequest.hpp"

void runInitializeCollections(){
    auto m_collection = getCollectionByName("users");
    initializeCollections(m_collection, KEY, IV);
}

void sendMessage(
    int m_senderId, 
    const std::string& m_msg
){
	std::cout << "sendMessage to m_senderId: " << m_senderId << std::endl; //TESTS
    std::cout << "CLient m_id del m_senderId: " << m_senderId <<": " << m_connections[m_senderId].m_id<< std::endl; //TESTS
    send(m_connections[m_senderId].m_fd, m_msg.c_str(), m_msg.size(), 0);
}

void handleMessage(
    int m_senderId, 
    const std::string& m_msg
){

    bool m_isInBlacklist = std::find(m_blackListConnections.begin(), m_blackListConnections.end(), m_senderId) != m_blackListConnections.end();
    if(m_isInBlacklist && UNLOCK_KEY != getValueFromJson(m_msg, {"body", "message"})){
        return;
    }

	std::cout << "handleMessage from m_senderId: " << m_senderId << std::endl; //TESTS
    std::cout << "m_msg: " << m_msg << std::endl; //TESTS

    auto m_it = m_remainingAttempts.find(m_senderId);
    std::string m_messageType = getValueFromJson(m_msg,{"messageType"});

    if("AUTENTICATION_REQUEST" == m_messageType){
        handleAuthRequest(m_senderId, m_msg);
    }else if("ALERT_MESSAGE" == m_messageType){
        handleAlertRequest(m_senderId, m_msg, m_isInBlacklist);
    }else if("SUPPLY_ORDER" == m_messageType){
        handleSupplyOrderMessage(m_msg, m_senderId);
    }else if("INVENTORY_UPDATE" == m_messageType){
        handleInventoryUpdateMessage(m_msg, m_senderId);
    }else if("WAREHOUSE_REGISTRATION" == m_messageType) {
        handleWarehouseRegisterRequest(m_msg);
    } else if ("SUPPLY_ORDER_CONFIRMATION" == m_messageType){
        handleSupplyOrderConfirmation(m_msg);
    }
}

void onRead(
    uv_poll_t* m_handle, 
    int m_status, 
    int m_events
){
    std::cout << "Nuevo mensaje del cliente" << std::endl;  //TESTS

    // Obtiene el file descriptor del cliente
    int m_fd = m_handle->io_watcher.fd;   
    char m_buffer[1024];

    // Lee el mensaje del cliente
    // Devuelve la cantidad de bytes leídos o -1 en caso de error
    // Es 0 si el cliente cerro la conexion
    int m_len = recv(m_fd, m_buffer, sizeof(m_buffer) - 1, 0);
    if (m_len <= 0) {
        uv_poll_stop(m_handle);
        close(m_fd);
        delete m_handle;
        return;
    }

    // Agrega el caracter nulo al final del m_buffer para convertirlo en una cadena
    m_buffer[m_len] = '\0';

    // Busca el ID del cliente a partir del file descriptor (m_fd)
    int m_senderId = -1;
    for (const auto& m_pair : m_connections) {
        if (m_pair.second.m_fd == m_fd) {
            m_senderId = m_pair.first;
            std::cout << "Cliente m_id: " << m_pair.second.m_id << std::endl; //TESTS
            std::cout << "Cliente m_fd: " << m_pair.second.m_fd << std::endl; //TESTS
            break;
        }
    }

    bool m_isBanned = std::find(m_bannedConnections.begin(), m_bannedConnections.end(), m_senderId) != m_bannedConnections.end();
    if(m_isBanned){
        uv_poll_stop(m_handle);
        close(m_fd);
        delete m_handle;
        return;
    }else{
        handleMessage(m_senderId, m_buffer);
    }
}

void setNonBlocking(
    int m_fd
) {
    int m_flags = fcntl(m_fd, F_GETFL, 0);
    fcntl(m_fd, F_SETFL, m_flags | O_NONBLOCK);
}

void runServer(){
    runInitializeCollections();

    // Crea un socket TCP
    int m_listenFd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in m_addr{};
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = INADDR_ANY;
    m_addr.sin_port = htons(PORT);

    bind(m_listenFd, (struct sockaddr*)&m_addr, sizeof(m_addr));
    listen(m_listenFd, 10);

    // Inicializa y obtiene un bucle de eventos
    uv_loop_t* m_loop = uv_default_loop();

    // Manejador de eventos para monitorear eventos de entrada/salida (I/O) en un descriptor de archivo
    uv_poll_t* m_listenPoll = new uv_poll_t;

    // Inicializa un manejador de eventos
    uv_poll_init(m_loop, m_listenPoll, m_listenFd);

    // Inicia el monitoreo de eventos en el descriptor de archivo asociado al manejador 
    // Monitorea el socket del servidor en busca de eventos de lectura (nuevas conexiones entrantes)
    uv_poll_start(m_listenPoll, UV_READABLE, [](uv_poll_t* m_handle, int m_status, int m_events) {
        struct sockaddr_in m_clientAddr;
        socklen_t m_clientLen = sizeof(m_clientAddr);

        // Acepta una nueva conexión entrante en el socket del servidor
        int m_clientFd = accept(m_handle->io_watcher.fd, (struct sockaddr*)&m_clientAddr, &m_clientLen);
        if (m_clientFd < 0) return;

        setNonBlocking(m_clientFd);

        // Crea un objeto Client especificando un id y el file descriptor
        Client m_client;
        m_client.m_id = m_nextClientId++;
        m_client.m_fd = m_clientFd;
        m_client.m_clientType = "connection";

        // Almacena el cliente en un mapa de clientes
        m_connections[m_client.m_id] = m_client;

        // Manejador de eventos para el cliente
        uv_poll_t* m_clientPoll = new uv_poll_t;
        // Asocia el manejador con el bucle de eventos y el descriptor del cliente
        uv_poll_init(uv_default_loop(), m_clientPoll, m_clientFd);
        // Monitorea eventos de lectura en el cliente.
        // Ejecuta el callback onRead cuando hay datos disponibles para leer en el socket del cliente.
        uv_poll_start(m_clientPoll, UV_READABLE, onRead);

        std::cout << "Cliente conectado con ID: " << m_client.m_id << std::endl;
    });

    std::cout << "Servidor escuchando en el puerto " << PORT << std::endl;

    // Inicia el bucle de eventos
    // Procesa los eventos registrados (como conexiones entrantes, 
    // datos disponibles para leer, etc.) y ejecuta los callbacks asociados.
    uv_run(m_loop, UV_RUN_DEFAULT);
    return;
}
