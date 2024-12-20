#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GRID_SIZE 10 // Define the size of the grid
#define MAX_RADAR_SWEEPS 3 // Maximum radar sweeps allowed
#define MAX_SMOKE_SCREENS 3 // Maximum smoke screens allowed

// Struct to represent a player
typedef struct {
    char playerName[50];
    char cells[GRID_SIZE][GRID_SIZE]; // Grid for player
    char opponent_grid[GRID_SIZE][GRID_SIZE]; // Opponent's grid
    int ships_left; // Number of ships left
    int radar_sweeps_used; // Radar sweeps used
    int smoke_screens_used; // Smoke screens used
} Player;

// Function to initialize the grid with a specified character
void initializeGrid(char grid[GRID_SIZE][GRID_SIZE], char fillChar) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = fillChar; // Fill each cell with water
        }
    }
}

// Function to display the grid
void displayGrid(const char grid[GRID_SIZE][GRID_SIZE]) {
    printf("  A B C D E F G H I J\n");
    for (int i = 0; i < GRID_SIZE; i++) {
        printf("%2d ", i + 1);
        for (int j = 0; j < GRID_SIZE; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}

// Function to get player names
void getPlayerNames(Player *player1, Player *player2) {
    printf("Enter name for Player 1: ");
    scanf("%s", player1->playerName);
    printf("Enter name for Player 2: ");
    scanf("%s", player2->playerName);
}

// Function to get difficulty level
int getDifficultyLevel() {
    int difficulty;
    printf("Select difficulty level (1 for Easy, 2 for Hard): ");
    scanf("%d", &difficulty);
    return difficulty;
}

// Function to print the grid
void printGrid(Player *p) {
    displayGrid(p->cells);
}

// Function to place ships on the grid
int placeShipHelper(Player *p, int num_of_cells_for_ship) {
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
            return 0;
        } else {
            for (int i = 0; i < num_of_cells_for_ship; i++) {
                if(p->cells[row + i][col] == '#'){
                    printf("A ship already exists in one of these cells!\n");
                    return 0;
                }
            }
        }
        for (int i = 0; i < num_of_cells_for_ship; i++) {
            p->cells[row + i][col] = '#';
        }
    } else if (choice == 'H') {
        if (col + num_of_cells_for_ship > 9) {
            printf("Ship cannot fit!\n");
            return 0;
        } else {
            for (int i = 0; i < num_of_cells_for_ship; i++) {
                if(p->cells[row][col + i] == '#'){
                    printf("A ship already exists in one of these cells!\n");
                    return 0;
                }
            }
        }
        for (int i = 0; i < num_of_cells_for_ship; i++) {
            p->cells[row][col + i] = '#';
        }
    } else {
        printf("Invalid choice!\n");
        return 0;
    }
    printGrid(p);
    return 1;
}

// Function to place ships for a player
void placeShip(Player *p){
    printf("Let's start filling the ships! %s\n", p->playerName);

    int is_carrier_placed = 0;
    while(is_carrier_placed == 0){
        is_carrier_placed = placeShipHelper(p, 5);
    }

    printf("\nLet's now place your battleship!\n");
    int is_battleship_placed = 0;
    while(is_battleship_placed == 0){
        is_battleship_placed = placeShipHelper(p, 4);
    }

    printf("\nLet's now place your destroyer!\n");
    int is_destroyer_placed = 0;
    while(is_destroyer_placed == 0){
        is_destroyer_placed = placeShipHelper(p, 3);
    }

    printf("\nLet's now place your submarine!\n");
    int is_submarine_placed = 0;
    while(is_submarine_placed == 0){
        is_submarine_placed = placeShipHelper(p, 2);
    }

    printGrid(p);
}

// Function to fire at the opponent
void fire(Player *attacker, Player *being_attacked) {
    char coordinates[3];
    printf("\nPlease enter coordinates of where you want to attack (e.g., A1): ");
    scanf("%s", coordinates);
    int row = coordinates[1] - '1';
    int col = coordinates[0] - 'A';

    if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) {
        printf("Invalid coordinates! You lose your turn.\n");
        return;
    }

    if (being_attacked->cells[row][col] == '~') {
        attacker->opponent_grid[row][col] = 'o';
        printf("Miss!!\n");
    } else if (being_attacked->cells[row][col] == '#') {
        attacker->opponent_grid[row][col] = '*';
        printf("Hit!!\n");
        being_attacked->ships_left--; // Reduce the count of ships left
        if (being_attacked->ships_left == 0) {
            printf("%s has sunk a ship!\n", attacker->playerName);
        }
    } else {
        printf("You have already targeted this location.\n");
    }
}

// Function for radar sweep
void radarSweep(Player *attacker, Player *being_attacked) {
    if (attacker->radar_sweeps_used >= MAX_RADAR_SWEEPS) {
        printf("You have used all your radar sweeps!\n");
        return;
    }

    char coordinates[3];
    printf("Please enter top-left coordinate for radar sweep (e.g., A1): ");
    scanf("%s", coordinates);
    int start_row = coordinates[1] - '1';
    int start_col = coordinates[0] - 'A';

    if (start_row < 0 || start_row >= GRID_SIZE - 1 || start_col < 0 || start_col >= GRID_SIZE - 1) {
        printf("Invalid coordinates for radar sweep!\n");
        return;
    }

    // Check 2x2 area
    int ships_found = 0;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            if (being_attacked->cells[start_row + i][start_col + j] == '#') {
                ships_found = 1;
                break;
            }
        }
    }

    if (ships_found) {
        printf("Enemy ships found!\n");
    } else {
        printf("No enemy ships found.\n");
    }

    attacker->radar_sweeps_used++;
}

// Function for smoke screen
void smokeScreen(Player *attacker) {
    if (attacker->smoke_screens_used >= attacker->ships_left) {
        printf("You have used all your smoke screens!\n");
        return;
    }

    char coordinates[3];
    printf("Please enter top-left coordinate for smoke screen (e.g., A1): ");
    scanf("%s", coordinates);
    int start_row = coordinates[1] - '1';
    int start_col = coordinates[0] - 'A';

    if (start_row < 0 || start_row >= GRID_SIZE - 1 || start_col < 0 || start_col >= GRID_SIZE - 1) {
        printf("Invalid coordinates for smoke screen!\n");
        return;
    }

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            attacker->cells[start_row + i][start_col + j] = 'X'; // Mark area with smoke
        }
    }

    printf("Smoke screen activated!\n");
    attacker->smoke_screens_used++;
}

// Function for artillery attack
void artillery(Player *attacker, Player *being_attacked) {
    char coordinates[3];
    printf("Please enter top-left coordinate for artillery attack (e.g., A1): ");
    scanf("%s", coordinates);
    int start_row = coordinates[1] - '1';
    int start_col = coordinates[0] - 'A';

    if (start_row < 0 || start_row >= GRID_SIZE - 1 || start_col < 0 || start_col >= GRID_SIZE - 1) {
        printf("Invalid coordinates for artillery attack!\n");
        return;
    }

    int hit = 0;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            if (being_attacked->cells[start_row + i][start_col + j] == '#') {
                hit = 1;
                being_attacked->ships_left--;
                being_attacked->cells[start_row + i][start_col + j] = '*'; // Mark hit
                printf("Hit at %c%d!\n", start_col + 'A', start_row + i + 1);
            } else if (being_attacked->cells[start_row + i][start_col + j] == '~') {
                being_attacked->cells[start_row + i][start_col + j] = 'o'; // Mark miss
                printf("Miss at %c%d!\n", start_col + 'A', start_row + i + 1);
            }
        }
    }

    if (hit) {
        printf("Artillery attack successful!\n");
    } else {
        printf("Artillery attack missed!\n");
    }
}

// Function for torpedo attack
void torpedo(Player *attacker, Player *being_attacked) {
    char coordinate[3];
    printf("Please enter the column (e.g., A) for torpedo attack: ");
    scanf("%s", coordinate);
    char col = coordinate[0];
    
    if (col < 'A' || col >= 'A' + GRID_SIZE) {
        printf("Invalid column!\n");
        return;
    }

    int col_index = col - 'A';
    int hit = 0;

    for (int row = 0; row < GRID_SIZE; row++) {
        if (being_attacked->cells[row][col_index] == '#') {
            hit = 1;
            being_attacked->ships_left--;
            being_attacked->cells[row][col_index] = '*'; // Mark hit
            printf("Hit at %c%d!\n", col, row + 1);
        } else if (being_attacked->cells[row][col_index] == '~') {
            being_attacked->cells[row][col_index] = 'o'; // Mark miss
            printf("Miss at %c%d!\n", col, row + 1);
        }
    }

    if (hit) {
        printf("Torpedo attack successful!\n");
    } else {
        printf("Torpedo attack missed!\n");
    }
}

// Main function
int main() {
    Player p1, p2;

    // Initialize both players' grids with water
    char fillCharacter = '~'; // Define the character to fill the grid with
    initializeGrid(p1.cells, fillCharacter);
    initializeGrid(p2.cells, fillCharacter);
    initializeGrid(p1.opponent_grid, fillCharacter);
    initializeGrid(p2.opponent_grid, fillCharacter);
    p1.radar_sweeps_used = 0;
    p1.smoke_screens_used = 0;
    p2.radar_sweeps_used = 0;
    p2.smoke_screens_used = 0;

    // Get player names
    getPlayerNames(&p1, &p2);

    // Get difficulty level
    int difficulty = getDifficultyLevel();

    // Set initial ships left
    p1.ships_left = 4;
    p2.ships_left = 4;

    // Place ships for both players
    placeShip(&p1);
    placeShip(&p2);

    // Game loop
    while (p1.ships_left > 0 && p2.ships_left > 0) {

        char choice_of_player;

        //p1 turn
        printf("\n%s's turn:\n", p1.playerName);
        printf("\nWhat would you like to do? Enter F for fire, R for radar sweep, S for smoke screen, or A for artillery: ");
        scanf("%c", &choice_of_player);
        if(choice_of_player == 'F')
            fire(&p1, &p2);
        else if(choice_of_player == 'R')
            radarSweep(&p1, &p2);
        else if(choice_of_player == 'S')
            smokeScreen(&p1);
        else if(choice_of_player == 'A')
            artillery(&p1, &p2);
        else
            printf("Invalid input!");

        if (p2.ships_left <= 0) {//checking if player sank all ships
            printf("%s wins!\n", p1.playerName);
            break;
        }


        printf("\n%s's turn:\n", p2.playerName);
        printf("\nWhat would you like to do? Enter F for fire, R for radar sweep, S for smoke screen, or A for artillery: ");
        scanf("%c", &choice_of_player);
        if(choice_of_player == 'F')
            fire(&p2, &p1);
        else if(choice_of_player == 'R')
            radarSweep(&p2, &p1);
        else if(choice_of_player == 'S')
            smokeScreen(&p2);
        else if(choice_of_player == 'A')
            artillery(&p2, &p1);
        else
            printf("Invalid input!");
        
        if (p1.ships_left <= 0) {
            printf("%s wins!\n", p2.playerName);
            break;
        }
    }

    return 0; // Indicate successful completion
}