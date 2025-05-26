#include <iostream>
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
#include <cjson/cJSON.h>
#include "server.hpp"


int main(int argc, char* argv[])
{
    std::cout << "Hola desde el main de server" << std::endl;

    // serverGreetings();

    // if ( argc < 2 ) {
    //     fprintf( stderr, "Uso: %s <puerto>\n", argv[0] );
    // exit( 1 );
    // }

    // runServer( atoi( argv[1] ) );

    runServer();
    return 0;
}
