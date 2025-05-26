#include <client_supply.h>
#include <client_globals.h>
#include <client_menu.h>

void config_supply_header(){
    printf("\n--- CONFIGURE CURRENT SUPPLY ---\n");
    printf("1. foodBox\n2. medicineBox\n3. waterBox\n4. weaponryBox\n");
    printf("Select an option:\n");
}

void config_supply(
    int option
){
    if (option >= 1 && option <= 4) {
        supply_defined = option;
        printf("Current supply defined as: %s\n",
            option == 1 ? "foodBox" :
            option == 2 ? "medicineBox" :
            option == 3 ? "waterBox" : "weaponryBox");
    } else {
        printf("Invalid option\n");
    }
    menu_level = 0;
    show_main_menu();
}