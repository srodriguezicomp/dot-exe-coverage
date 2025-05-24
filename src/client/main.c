#include <stdio.h>
#include <client.h>
#include <stdlib.h>
#include <unistd.h>
#include <bits/getopt_core.h>

int main(int argc, char* argv[])
{
    printf("Hola desde el main de client\n");

    // clientGreetings();

    // if ( argc < 3 ) {
	// 	fprintf( stderr, "Uso %s host puerto\n", argv[0]);
	// 	exit( 1 );
	// }

    // run_client(argv[1], atoi(argv[2]));

    //------------------------------------

    // run_client(argc, argv);

    //------------------------------------

    int opt;
    int qty = -1;
    int supinc = -1;
    char* user = NULL;
    char* password = NULL;

    // Agregamos opciones u:p: además de q:s:
    while ((opt = getopt(argc, argv, "q:s:u:p:")) != -1) {
        switch (opt) {
            case 'q':
                qty = atoi(optarg);
                break;
            case 's':
                supinc = atoi(optarg);
                break;
            case 'u':
                user = optarg;
                break;
            case 'p':
                password = optarg;
                break;
            default:
                fprintf(stderr, "Uso: %s -q <cantidad> -s <supply_increment> -u <usuario> -p <password>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (qty == -1 || supinc == -1 || user == NULL || password == NULL) {
        fprintf(stderr, "Faltan argumentos. Uso: %s -q <cantidad> -s <supply_increment> -u <usuario> -p <password>\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    run_client(qty, supinc, user, password);

    return 0;
}