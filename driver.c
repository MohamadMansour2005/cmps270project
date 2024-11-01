#include <stdio.h>

struct Player{//guys add anything else that can be usefulfor your code
    char name[20];
    char player_grid[10][10];
    char opponent_grid[10][10];
    int ships_left;
};

int printGrid(struct Player p){//tobe changed by Tala
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 10; j++){
            printf("%c, ", p.player_grid[i][j]);
        }
        printf("\n");
    }
    return 0;
}

int placeShipHelper(struct Player *p, int num_of_cells_for_ship) { // Use pointer to Player
    char choice;
    printf("\nRemember your ship takes %d cells. Would you like to place your ship vertically (V) or horizontally (H)? ", num_of_cells_for_ship);
    scanf(" %c", &choice);
    printf("Please enter which cell you would like your ship to begin (e.g., A1): ");
    char cell[3];
    scanf("%s", cell);

    int row = cell[1] - '1'; // conversion from ASCII code
    int col = cell[0] - 'A';

    if (choice == 'V') {
        if (row + num_of_cells_for_ship > 9) {
            printf("Ship cannot fit!\n");
        } else {
            for (int i = 0; i < num_of_cells_for_ship; i++) {
                if(p->player_grid[row + i][col] == '#'){
                    printf("A ship already exists in one of these cells!");
                    return 0;
                }
            }
        }
    } else if (choice == 'H') {
        if (col + num_of_cells_for_ship > 9) {
            printf("Ship cannot fit!\n");
        } else {
            for (int i = 0; i < num_of_cells_for_ship; i++) {
                if(p->player_grid[row][col +i] == '#'){
                    printf("A ship already exists in one of these cells!");
                    return 0;
                }
            }
        }
    }


    if (choice == 'V') {
        for (int i = 0; i < num_of_cells_for_ship; i++) {
            p->player_grid[row + i][col] = '#';
        }
        printGrid(*p);
        return 1;
    }
    else if (choice == 'H') {
        for (int i = 0; i < num_of_cells_for_ship; i++) {
            p->player_grid[row][col + i] = '#';
        }
        printGrid(*p);
        return 1;
    }

    return 0; // Return 0 if placement was unsuccessful
}

int placeShip(struct Player *p){
    printf("Let's start filling the ships! %s\nLet's start with placing a carrier.\n", p->name);

    int is_carrier_placed = 0;
    while(is_carrier_placed == 0){
        is_carrier_placed = placeShipHelper(p, 5);
    }

    printf("\nLet's now place your battleship!");
    int is_battleship_placed = 0;
    while(is_battleship_placed == 0){
        is_battleship_placed = placeShipHelper(p, 4);
    }

    printf("\nLet's now place your destroyer!");
    int is_destroyer_placed = 0;
    while(is_destroyer_placed == 0){
        is_destroyer_placed = placeShipHelper(p, 3);
    }

    printf("\nLet's now place your submarine!");
    int is_submarine_placed = 0;
    while(is_submarine_placed == 0){
        is_submarine_placed = placeShipHelper(p, 2);
    }

    printGrid(*p);
    return 0;
}

int fire(struct Player *attacker, struct Player *being_attacked){
    char coordinates[2];
    printf("\nPlease enter coordinates of where you want to attack. ");
    scanf("%s", coordinates);
    int row = coordinates[0] - '1';
    int col = coordinates[1] - 'A';

    if(being_attacked->player_grid[row][col] == '~'){
        attacker->opponent_grid[row][col] == 'o';
        printf("Miss!!");
    }

    if(being_attacked->player_grid[row][col] == '#'){
        attacker->opponent_grid[row][col] == '*';
        printf("Hit!!");
    }

    //someone write a function to check if a ship was down
}

int main(){
    struct Player p1;
    struct Player p2;

    //Tala to add name and difficulity

    p1.ships_left = 4;
    p2.ships_left = 4;

    char player_grid[10][10];
    char opponent_grid[10][10];
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 10; j++){
            p1.player_grid[i][j] = '~';
            p1.opponent_grid[i][j] = '~';
            p2.player_grid[i][j] = '~';
            p2.opponent_grid[i][j] = '~';
        }
    }

    printGrid(p1);
    placeShip(&p1);
    printGrid(p2);
    placeShip(&p2);

    while(p1.ships_left != 0 || p2.ships_left != 0){}

    return 0;
}