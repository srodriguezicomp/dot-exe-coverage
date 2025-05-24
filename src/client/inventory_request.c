#include <inventory_request.h>
#include <client_globals.h>
#include <client_menu.h>

void show_inventary(){
    printf("Inventario actual:\n");
    printf("foodBox: %d | medicineBox: %d | waterBox: %d | weaponryBox: %d\n",
        food_box, medicine_box, water_box, weaponry_box);
    show_main_menu();
}
