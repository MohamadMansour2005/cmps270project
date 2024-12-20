#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define GRID_SIZE 10
#define MAX_RADAR_SWEEPS 5
#define MAX_SMOKE_SCREENS 5
#define MAX_TURNS 1000 //is this necessary?
#define EASY 0
#define MEDIUM 1
#define HARD 2
#define DICE_SIDES 10  // Weighted dice

// Struct for Player and Bot
typedef struct {
    char name[50];
    char grid[GRID_SIZE][GRID_SIZE];
    char visibleGrid[GRID_SIZE][GRID_SIZE];
    int radarSweepsUsed;
    int smokeScreensUsed;
    int shipsSunk;
    int artilleryUnlocked;
    int torpedoUnlocked;
    char difficulty[10];
    int isBot;
    int treasuresDiscovered;
    clock_t timeUsed;
    int bonusMoves;  // Track additional bonus moves (e.g., extra turns)

    int rowOfHitCell;
    int colOfHitCell;
    int directionToHit; //0 is nothing, 1 is left, 2 is up, 3 is right, 4 is down
    int isHorizontal;
} Player;

// Function prototypes
void initializeGrid(char grid[GRID_SIZE][GRID_SIZE]);
void displayGrid(const char grid[GRID_SIZE][GRID_SIZE], int displayMisses);
void getPlayerNames(Player *p1, Player *p2);
void placeShip(Player *p, int shipSize);
void clearScreen();
Player *chooseStartingPlayer(Player *p1, Player *p2);
void takeTurn(Player *currentPlayer, Player *opponent);
int checkVictory(Player *p);
void radarSweep(Player *currentPlayer, Player *opponent, char coord[]);
void smokeScreen(Player *currentPlayer, char coord[]);
void artillery(Player *currentPlayer, Player *opponent, char coord[]);
void torpedo(Player *currentPlayer, Player *opponent, char coord[]);
void updateVisibleGrid(Player *opponent);
int processFire(Player *currentPlayer, Player *opponent, char coord[]); //changed to int for bot
void displayEndGameMessage(Player *winner);
void setupPlayerShips(Player *player);
int checkIfShipSunk(Player *player, int row, int col);
void botMove(Player *bot, Player *opponent);
void botEasyMove(Player *bot, Player *opponent);
void botMediumMove(Player *bot, Player *opponent);
void botHardMove(Player *bot, Player *opponent);
void rollWeightedDice(Player *bot, Player *opponent);
void placeTreasure(Player *player);
void discoverTreasure(Player *currentPlayer, Player *opponent, int row, int col);
void updateProbabilityGrid(Player *bot, Player *opponent);
void battleRoyale();
void playerBonusMove(Player *player, Player *opponent);
void botBonusMove(Player *bot, Player *opponent);

int main() {
    Player p1, p2;
    srand(time(0));

    printf("Welcome to Battleship with Bot Implementation!\n");

    // Battle Royale Setup
    printf("Would you like to run a Battle Royale simulation? (y/n): ");
    char choice;
    scanf(" %c", &choice);
    if (choice == 'y' || choice == 'Y') {
        battleRoyale();
        return 0;
    }

    // Standard game setup
    initializeGrid(p1.grid);
    initializeGrid(p2.grid);
    initializeGrid(p1.visibleGrid);
    initializeGrid(p2.visibleGrid);

    p1.radarSweepsUsed = 0;
    p1.smokeScreensUsed = 0;
    p1.shipsSunk = 0;
    p1.artilleryUnlocked = 0;
    p1.torpedoUnlocked = 0;
    p1.isBot = 0;
    p1.treasuresDiscovered = 0;
    p1.timeUsed = 0;
    p1.bonusMoves = 0;

    p2.radarSweepsUsed = 0;
    p2.smokeScreensUsed = 0;
    p2.shipsSunk = 0;
    p2.artilleryUnlocked = 0;
    p2.torpedoUnlocked = 0;
    p2.isBot = 1; // Bot
    p2.treasuresDiscovered = 0;
    p2.timeUsed = 0;
    p2.bonusMoves = 0;
    p2.rowOfHitCell = 0;
    p2.colOfHitCell = 0;
    p2.isHorizontal = 2;

    // Get player names and bot difficulty
    getPlayerNames(&p1, &p2);

    printf("Choose difficulty for %s (Easy, Medium, Hard): ", p2.name);
    scanf("%s", p2.difficulty);

    // Place ships and treasure
    setupPlayerShips(&p1);
    setupPlayerShips(&p2);
    placeTreasure(&p2);

    // Choose starting player
    Player *currentPlayer = chooseStartingPlayer(&p1, &p2);
    Player *opponent = (currentPlayer == &p1) ? &p2 : &p1;

    printf("The starting player is: %s\n\n", currentPlayer->name);

    // Game loop
    int turnCount = 0;
    while (turnCount < MAX_TURNS) {
        clock_t start = clock();
        takeTurn(currentPlayer, opponent);
        clock_t end = clock();
        currentPlayer->timeUsed += (end - start);

        if (checkVictory(opponent)) {
            displayEndGameMessage(currentPlayer);
            break;
        }

        // Switch players
        Player *temp = currentPlayer;
        currentPlayer = opponent;
        opponent = temp;

        turnCount++;
    }

    if (turnCount >= MAX_TURNS) {
        printf("Game ended in a draw after reaching the maximum turn limit.\n");
    }

    return 0;
}

// Function for Battle Royale Simulation
void battleRoyale() {
    printf("\nBattle Royale Simulation starting...\n");

    // Simulate multiple bot battles
    const int numBots = 4;
    Player bots[numBots];
    for (int i = 0; i < numBots; i++) {
        sprintf(bots[i].name, "Bot_%d", i + 1);
        bots[i].isBot = 1;
        initializeGrid(bots[i].grid);
        initializeGrid(bots[i].visibleGrid);
        setupPlayerShips(&bots[i]);
        placeTreasure(&bots[i]);
        bots[i].timeUsed = 0;
        if (i < numBots / 3) {
            strcpy(bots[i].difficulty, "Easy");
        } else if (i < (2 * numBots) / 3) {
            strcpy(bots[i].difficulty, "Medium");
        } else {
            strcpy(bots[i].difficulty, "Hard");
        }
    }

    // Simulate pairwise matches
    for (int i = 0; i < numBots; i++) {
        for (int j = i + 1; j < numBots; j++) {
            printf("\nMatch between %s (%s) and %s (%s)\n", bots[i].name, bots[i].difficulty, bots[j].name, bots[j].difficulty);
            Player *current = &bots[i];
            Player *opponent = &bots[j];

            int turnCount = 0;
            while (turnCount < MAX_TURNS) {
                clock_t start = clock();
                takeTurn(current, opponent);
                clock_t end = clock();
                current->timeUsed += (end - start);

                if (checkVictory(opponent)) {
                    printf("%s wins the match!\n", current->name);
                    break;
                }

                // Switch players
                Player *temp = current;
                current = opponent;
                opponent = temp;

                turnCount++;
            }

            if (turnCount >= MAX_TURNS) {
                printf("Match ended in a draw after reaching the maximum turn limit.\n");
            }
        }
    }
}

// Function to place treasure for a bot
void placeTreasure(Player *player) {
    int row = rand() % GRID_SIZE;
    int col = rand() % GRID_SIZE;
    while (player->grid[row][col] != '~') {
        row = rand() % GRID_SIZE;
        col = rand() % GRID_SIZE;
    }
    player->grid[row][col] = 'T'; // Mark the treasure
}

// Function to discover treasure
void discoverTreasure(Player *currentPlayer, Player *opponent, int row, int col) {
    if (opponent->grid[row][col] == 'T') {
        printf("%s discovered a treasure at %c%d!\n", currentPlayer->name, 'A' + col, row + 1);
        currentPlayer->treasuresDiscovered++;
        opponent->grid[row][col] = '~'; // Remove treasure after discovery

        // Bot or player bonus
        if (currentPlayer->isBot) {
            printf("Bot gains extra Radar Sweeps!\n");
            currentPlayer->radarSweepsUsed = 0; // Reset Radar usage
        } else {
            printf("%s unlocks Artillery early!\n", currentPlayer->name);
            currentPlayer->artilleryUnlocked = 1;
        }
    }
}

// Function to roll a weighted dice for the bot's move (70% chance to use powerful moves)
void rollWeightedDice(Player *bot, Player *opponent) {
    int roll = rand() % DICE_SIDES;
    if (roll < 7) { // 70% chance
        if (bot->artilleryUnlocked) {
            char coord[3];
            sprintf(coord, "%c%d", 'A' + rand() % GRID_SIZE, 1 + rand() % GRID_SIZE);
            printf("Bot uses Artillery at %s!\n", coord);
            artillery(bot, opponent, coord);
        } else if (bot->radarSweepsUsed < MAX_RADAR_SWEEPS) {
            char coord[3];
            sprintf(coord, "%c%d", 'A' + rand() % GRID_SIZE, 1 + rand() % GRID_SIZE);
            printf("Bot uses Radar Sweep at %s!\n", coord);
            radarSweep(bot, opponent, coord);
            bot->radarSweepsUsed++;
        } else {
            torpedo(bot, opponent, "3");
        }
    } else { // 30% chance to fire normally
        botMove(bot, opponent);
    }
}

// Function to check if a player has won
int checkVictory(Player *p) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (p->grid[i][j] == '#') {
                return 0; // If there is still a ship, no victory
            }
        }
    }
    return 1; // All ships are sunk
}

// Function to check if a ship is sunk
int checkIfShipSunk(Player *player, int row, int col) {
    int i = row;
    while (i >= 0 && player->grid[i][col] == '#') {
        i--;
    }
    i++;
    while (i < GRID_SIZE && (player->grid[i][col] == '*' || player->grid[i][col] == '#')) {
        if (player->grid[i][col] == '#') {
            return 0;
        }
        i++;
    }

    int j = col;
    while (j >= 0 && player->grid[row][j] == '#') {
        j--;
    }
    j++;
    while (j < GRID_SIZE && (player->grid[row][j] == '*' || player->grid[row][j] == '#')) {
        if (player->grid[row][j] == '#') {
            return 0;
        }
        j++;
    }

    player->shipsSunk++;
    return 1;
}

// Function to process each player's move
int processFire(Player *currentPlayer, Player *opponent, char coord[]) { //I edited to int function so in bot process fire i can know if we hit something
    int row = coord[1] - '1';
    int col = coord[0] - 'A';

    if (opponent->grid[row][col] == '#') { // Hit a ship
        opponent->grid[row][col] = '*';
        currentPlayer->visibleGrid[row][col] = '*';
        printf("Hit at %s!\n", coord);

        if (checkIfShipSunk(opponent, row, col)) {
            printf("You sunk a ship!\n");
            currentPlayer->artilleryUnlocked = 1;
        }
        return 1;
    } else if (opponent->grid[row][col] == 'T') { // Hit a treasure
        discoverTreasure(currentPlayer, opponent, row, col);
    } else {
        opponent->grid[row][col] = 'o'; // Mark as missed
        currentPlayer->visibleGrid[row][col] = 'o';
        printf("Miss at %s.\n", coord);
    }
    return 0;
}
// Function to initialize the grid (filling it with water '~')
void initializeGrid(char grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = '~';  // Water symbol
        }
    }
}

// Function to display the grid for the player
void displayGrid(const char grid[GRID_SIZE][GRID_SIZE], int displayMisses) {
    printf("   A B C D E F G H I J\n");
    for (int i = 0; i < GRID_SIZE; i++) {
        printf("%2d ", i + 1);
        for (int j = 0; j < GRID_SIZE; j++) {
            if (displayMisses || grid[i][j] == '*' || grid[i][j] == 'o' || grid[i][j] == 'T') {
                printf("%c ", grid[i][j]);
            } else {
                printf("~ "); // Hide misses for the opponent
            }
        }
        printf("\n");
    }
}

// Function to get player names (including bot)
void getPlayerNames(Player *p1, Player *p2) {
    printf("Enter name for Player 1: ");
    scanf("%s", p1->name);
    printf("Enter name for Player 2: ");
    scanf("%s", p2->name);
}

// Function to clear the screen (ANSI escape code for Unix systems)
void clearScreen() {
    printf("\033[H\033[J"); // Clears screen
}

// Function to choose starting player randomly
Player *chooseStartingPlayer(Player *p1, Player *p2) {
    return rand() % 2 == 0 ? p1 : p2;
}

// Function to set up the player's ships with different sizes
void setupPlayerShips(Player *player) {
    int shipSizes[] = {5, 4, 3, 3, 2};  // Ship sizes: Carrier (5), Battleship (4), Submarine (3), Destroyer (3), Patrol Boat (2)
    int numShips = sizeof(shipSizes) / sizeof(shipSizes[0]);

    printf("%s, place your ships.\n", player->name);
    for (int i = 0; i < numShips; i++) {
        placeShip(player, shipSizes[i]);
    }
    clearScreen();
}

// Function to place a ship on the grid (checks for valid placement)
void placeShip(Player *p, int shipSize) {
    char orientation;
    char startCell[3];
    int row, col;

    displayGrid(p->grid, 1);  // Display grid before placing ship
    printf("%s, place your ship of size %d (example: B3 H for horizontal, V for vertical):\n", p->name, shipSize);

    while (1) {
        printf("Enter starting position and orientation (H/V): ");
        scanf("%s %c", startCell, &orientation);
        row = startCell[1] - '1';
        col = startCell[0] - 'A';

        if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) {
            printf("Invalid position! Try again.\n");
            continue;
        }

        int validPlacement = 1;

        if (orientation == 'H') {
            if (col + shipSize > GRID_SIZE) {
                printf("Ship extends beyond the grid horizontally! Try again.\n");
                continue;
            }
            for (int i = 0; i < shipSize; i++) {
                if (p->grid[row][col + i] == '#') {
                    printf("Ship overlaps with another! Try again.\n");
                    validPlacement = 0;
                    break;
                }
            }
            if (validPlacement) {
                for (int i = 0; i < shipSize; i++) {
                    p->grid[row][col + i] = '#';
                }
                break;
            }
        } else if (orientation == 'V') {
            if (row + shipSize > GRID_SIZE) {
                printf("Ship extends beyond the grid vertically! Try again.\n");
                continue;
            }
            for (int i = 0; i < shipSize; i++) {
                if (p->grid[row + i][col] == '#') {
                    printf("Ship overlaps with another! Try again.\n");
                    validPlacement = 0;
                    break;
                }
            }
            if (validPlacement) {
                for (int i = 0; i < shipSize; i++) {
                    p->grid[row + i][col] = '#';
                }
                break;
            }
        } else {
            printf("Invalid orientation! Try again.\n");
        }
    }

    displayGrid(p->grid, 1); // Show the updated grid after placing the ship
}

// Function to handle the bot's move based on its difficulty
void botMove(Player *bot, Player *opponent) {
    if (strcmp(bot->difficulty, "Easy") == 0) {
        botEasyMove(bot, opponent);
    } else if (strcmp(bot->difficulty, "Medium") == 0) {
        botMediumMove(bot, opponent);
    } else if (strcmp(bot->difficulty, "Hard") == 0) {
        botHardMove(bot, opponent);
    }
}

// Easy bot move strategy: Follows a simple checkerboard pattern
void botEasyMove(Player *bot, Player *opponent) {//I edited this
    int didItHit;
    if(bot->directionToHit == 0 && bot->isHorizontal == 2){
    
        int row = rand() % GRID_SIZE;
        int col = rand() % GRID_SIZE;

        while (opponent->visibleGrid[row][col] != '~') {
            row = rand() % GRID_SIZE;
            col = rand() % GRID_SIZE;
        }

        printf("%s makes a move at %c%d\n", bot->name, 'A' + col, row + 1);
        int didItHit = processFire(bot, opponent, (char[]){'A' + col, row + '1', '\0'});

        if(didItHit == 1){
            bot->rowOfHitCell = row;
            bot->colOfHitCell = col;
            bot->directionToHit = 1;
        }
    }
    else if(bot->directionToHit != 0 && bot->isHorizontal == 2){
        if(bot->isHorizontal == 2){
            if(bot->directionToHit = 1 && bot->colOfHitCell - 1 >= 0){//for left side
                didItHit = processFire(bot, opponent, (char[]){'A' + bot->colOfHitCell - 1, bot->rowOfHitCell + '1', '\0'});

                if(didItHit != 1){
                    bot->directionToHit = 2;
                }
                else{
                    bot->isHorizontal = 1;
                }
            }

            else if(bot->directionToHit = 2 && bot->rowOfHitCell - 1 >= 0){
                didItHit = processFire(bot, opponent, (char[]){'A' + bot->colOfHitCell, bot->rowOfHitCell + '1' - 1, '\0'});

                if(didItHit != 1){
                    bot->directionToHit = 3;
                }
                else{
                    bot->isHorizontal = 0;
                }
            }

            else if(bot->directionToHit = 3 && bot->colOfHitCell + 1 <= 9){
                didItHit = processFire(bot, opponent, (char[]){'A' + bot->colOfHitCell + 1, bot->rowOfHitCell + '1', '\0'});

                if(didItHit != 1){
                    bot->directionToHit = 4;
                }
                else{
                    bot->isHorizontal = 1;
                }
            }

            else if(bot->directionToHit = 4 && bot->rowOfHitCell + 1 <= 9){
                didItHit = processFire(bot, opponent, (char[]){'A' + bot->colOfHitCell, bot->rowOfHitCell + '1' + 1, '\0'});

                if(didItHit != 1){
                    bot->directionToHit = 0;
                }
                else{
                    bot->isHorizontal = 0;
                }
            }
        }
    }

    else if(bot->isHorizontal == 0){
        char prevCellSign = bot->visibleGrid[bot->rowOfHitCell][0];
        for(int i = 1; i < 10; i++){
            char currentCellSign = bot->visibleGrid[bot->rowOfHitCell][i];
            if(currentCellSign == 'o' || currentCellSign == '~' || (currentCellSign == '*' && prevCellSign == '*') || (currentCellSign == '*' && prevCellSign == 'o')){
                prevCellSign = currentCellSign;
            }
            else if(currentCellSign == '*' && prevCellSign == '~'){
                processFire(bot, opponent, (char[]){'A' + i, bot->rowOfHitCell + '1', '\0'});
            }
        }
    }

    else if(bot->isHorizontal == 1){
        char prevCellSign = bot->visibleGrid[0][bot->colOfHitCell];
        for(int i = 1; i < 10; i++){
            char currentCellSign = bot->visibleGrid[i][bot->colOfHitCell];
            if(currentCellSign == 'o' || currentCellSign == '~' || (currentCellSign == '*' && prevCellSign == '*') || (currentCellSign == '*' && prevCellSign == 'o')){
                prevCellSign = currentCellSign;
            }
            else if(currentCellSign == '*' && prevCellSign == '~'){
                processFire(bot, opponent, (char[]){'A' + bot->colOfHitCell, i + '1', '\0'});
            }
        }
    }
}

// Medium bot move strategy: Uses probability grid based on past moves
void botMediumMove(Player *bot, Player *opponent) {
    updateProbabilityGrid(bot, opponent);
    int row = rand() % GRID_SIZE;
    int col = rand() % GRID_SIZE;

    while (opponent->visibleGrid[row][col] != '~') {
        row = rand() % GRID_SIZE;
        col = rand() % GRID_SIZE;
    }

    printf("%s makes a move at %c%d\n", bot->name, 'A' + col, row + 1);
    processFire(bot, opponent, (char[]){'A' + col, row + '1', '\0'});
}

// Hard bot move strategy: Uses a weighted dice system to decide on attacks
void botHardMove(Player *bot, Player *opponent) {
    rollWeightedDice(bot, opponent);
}

// Function for weighted dice roll: Determines the next move based on a 70% chance for special moves
void rollWeightedDice(Player *bot, Player *opponent) {
    int roll = rand() % DICE_SIDES;
    if (roll < 7) {  // 70% chance for powerful moves
        if (bot->artilleryUnlocked) {
            char coord[3];
            sprintf(coord, "%c%d", 'A' + rand() % GRID_SIZE, 1 + rand() % GRID_SIZE);
            printf("Bot uses Artillery at %s!\n", coord);
            artillery(bot, opponent, coord);
        } else if (bot->radarSweepsUsed < MAX_RADAR_SWEEPS) {
            char coord[3];
            sprintf(coord, "%c%d", 'A' + rand() % GRID_SIZE, 1 + rand() % GRID_SIZE);
            printf("Bot uses Radar Sweep at %s!\n", coord);
            radarSweep(bot, opponent, coord);
            bot->radarSweepsUsed++;
        } else {
            torpedo(bot, opponent, "3");
        }
    } else {  // 30% chance for normal attack
        botMove(bot, opponent);
    }
}

// Function to display the end of the game message and declare the winner
void displayEndGameMessage(Player *winner) {
    printf("Game over! %s wins!\n", winner->name);
}

// Function to update the visible grid (reveals hits and misses)
void updateVisibleGrid(Player *opponent) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (opponent->grid[i][j] == '*' || opponent->grid[i][j] == 'o') {
                opponent->visibleGrid[i][j] = opponent->grid[i][j];
            }
        }
    }
}

// Function to check victory (all ships are sunk)
int checkVictory(Player *p) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (p->grid[i][j] == '#') {
                return 0;  // Game is still ongoing if any ship is left
            }
        }
    }
    return 1;  // Victory: all ships are sunk
}

// Function to check if a ship is sunk
int checkIfShipSunk(Player *player, int row, int col) {
    // Check if the ship is fully sunk (no remaining part of the ship)
    int i = row;
    while (i >= 0 && player->grid[i][col] == '#') {
        i--;
    }
    i++;
    while (i < GRID_SIZE && (player->grid[i][col] == '*' || player->grid[i][col] == '#')) {
        if (player->grid[i][col] == '#') {
            return 0;  // Ship not sunk yet
        }
        i++;
    }

    int j = col;
    while (j >= 0 && player->grid[row][j] == '#') {
        j--;
    }
    j++;
    while (j < GRID_SIZE && (player->grid[row][j] == '*' || player->grid[row][j] == '#')) {
        if (player->grid[row][j] == '#') {
            return 0;  // Ship not sunk yet
        }
        j++;
    }

    player->shipsSunk++;

    //need editing here for resets

    return 1;  // Ship sunk successfully
}

// Function to perform radar sweep (reveals ships in a 2x2 area)
void radarSweep(Player *currentPlayer, Player *opponent, char coord[]) {
    int row = coord[1] - '1';
    int col = coord[0] - 'A';

    if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) {
        printf("Invalid radar coordinates!\n");
        return;
    }

    printf("Radar Sweep at %s: ", coord);
    for (int i = row; i < row + 2 && i < GRID_SIZE; i++) {
        for (int j = col; j < col + 2 && j < GRID_SIZE; j++) {
            if (opponent->grid[i][j] == '#') {
                printf("Ship at %c%d! ", 'A' + j, i + 1);
            } else {
                printf("Water at %c%d. ", 'A' + j, i + 1);
            }
        }
    }
    printf("\n");
}

// Function to perform smoke screen (obscures a 2x2 area from the opponent)
void smokeScreen(Player *currentPlayer, char coord[]) {
    int row = coord[1] - '1';
    int col = coord[0] - 'A';

    if (row < 0 || row >= GRID_SIZE - 1 || col < 0 || col >= GRID_SIZE - 1) {
        printf("Invalid smoke screen coordinates!\n");
        return;
    }

    // Clear the area on the opponent's visible grid
    printf("Smoke Screen at %s!\n", coord);
    for (int i = row; i <= row + 1; i++) {
        for (int j = col; j <= col + 1; j++) {
            if (i < GRID_SIZE && j < GRID_SIZE) {
                currentPlayer->visibleGrid[i][j] = '?';  // Obscure with '?'
            }
        }
    }
    currentPlayer->smokeScreensUsed++;
}

// Function to fire artillery (attacks a 2x2 area)
void artillery(Player *currentPlayer, Player *opponent, char coord[]) {
    int row = coord[1] - '1';
    int col = coord[0] - 'A';

    if (row < 0 || row >= GRID_SIZE - 1 || col < 0 || col >= GRID_SIZE - 1) {
        printf("Invalid artillery coordinates!\n");
        return;
    }

    printf("Firing Artillery at %s!\n", coord);
    for (int i = row; i <= row + 1; i++) {
        for (int j = col; j <= col + 1; j++) {
            if (i < GRID_SIZE && j < GRID_SIZE) {
                if (opponent->grid[i][j] == '#') {
                    opponent->grid[i][j] = '*';  // Hit the ship
                    currentPlayer->visibleGrid[i][j] = '*';
                    printf("Hit at %c%d!\n", 'A' + j, i + 1);
                } else if (opponent->grid[i][j] == '~') {
                    opponent->grid[i][j] = 'o';  // Missed attack
                    currentPlayer->visibleGrid[i][j] = 'o';
                    printf("Miss at %c%d.\n", 'A' + j, i + 1);
                }
            }
        }
    }
    currentPlayer->artilleryUnlocked = 0;  // Reset artillery after use
}

// Function to perform torpedo (attacks an entire row or column)
void torpedo(Player *currentPlayer, Player *opponent, char coord[]) {
    int row, col;
    if (coord[0] >= 'A' && coord[0] <= 'J') {  // Torpedo along column
        col = coord[0] - 'A';
        printf("Firing Torpedo along column %c!\n", coord[0]);
        for (row = 0; row < GRID_SIZE; row++) {
            if (opponent->grid[row][col] == '#') {
                opponent->grid[row][col] = '*';  // Hit
                currentPlayer->visibleGrid[row][col] = '*';
                printf("Hit at %c%d!\n", coord[0], row + 1);
            } else if (opponent->grid[row][col] == '~') {
                opponent->grid[row][col] = 'o';  // Miss
                currentPlayer->visibleGrid[row][col] = 'o';
                printf("Miss at %c%d.\n", coord[0], row + 1);
            }
        }
    } else if (coord[1] >= '1' && coord[1] <= '9') {  // Torpedo along row
        row = coord[1] - '1';
        printf("Firing Torpedo along row %d!\n", row + 1);
        for (col = 0; col < GRID_SIZE; col++) {
            if (opponent->grid[row][col] == '#') {
                opponent->grid[row][col] = '*';  // Hit
                currentPlayer->visibleGrid[row][col] = '*';
                printf("Hit at %c%d!\n", 'A' + col, row + 1);
            } else if (opponent->grid[row][col] == '~') {
                opponent->grid[row][col] = 'o';  // Miss
                currentPlayer->visibleGrid[row][col] = 'o';
                printf("Miss at %c%d.\n", 'A' + col, row + 1);
            }
        }
    } else {
        printf("Invalid torpedo coordinates!\n");
    }
    currentPlayer->torpedoUnlocked = 0;  // Reset torpedo after use
}

// Function to update the visible grid based on the opponent's grid (for the player to see hits/misses)
void updateVisibleGrid(Player *opponent) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (opponent->grid[i][j] == '*' || opponent->grid[i][j] == 'o') {
                opponent->visibleGrid[i][j] = opponent->grid[i][j];  // Show hits and misses
            }
        }
    }
}

// Function to display end game message when a winner is found
void displayEndGameMessage(Player *winner) {
    printf("Game Over! %s wins!\n", winner->name);
}

void takeTurn(Player *currentPlayer, Player *opponent) {////////////////////////////////////////////////////////////////////////////////////////////////////
    char command[20], coord[3];
    clock_t start, end;

    printf("\n%s's turn\n", currentPlayer->name);
    updateVisibleGrid(opponent);  // Update visible grid after previous moves
    displayGrid(opponent->visibleGrid, 1);  // Show the visible grid of the opponent

    printf("Available moves:\n");
    printf("1. Fire [coordinate]\n");
    printf("2. Radar [top-left coordinate]\n");
    printf("3. Smoke [top-left coordinate]\n");
    printf("4. Artillery [top-left coordinate] (unlocked if you sink a ship)\n");
    printf("5. Torpedo [row/column] (unlocked after sinking 3 ships)\n");

    printf("%s, enter your command: ", currentPlayer->name);
    start = clock();  // Start timer

    scanf("%s %s", command, coord);

    if (strcmp(command, "Fire") == 0) {
        processFire(currentPlayer, opponent, coord);
    } else if (strcmp(command, "Radar") == 0) {
        if (currentPlayer->radarSweepsUsed < MAX_RADAR_SWEEPS) {
            radarSweep(currentPlayer, opponent, coord);
            currentPlayer->radarSweepsUsed++;
        } else {
            printf("Radar Sweep limit reached! You lose your turn.\n");
        }
    } else if (strcmp(command, "Smoke") == 0) {
        smokeScreen(currentPlayer, coord);
    } else if (strcmp(command, "Artillery") == 0) {
        if (currentPlayer->artilleryUnlocked) {
            artillery(currentPlayer, opponent, coord);
        } else {
            printf("Artillery not unlocked yet!\n");
        }
    } else if (strcmp(command, "Torpedo") == 0) {
        if (currentPlayer->torpedoUnlocked) {
            torpedo(currentPlayer, opponent, coord);
        } else {
            printf("Torpedo not unlocked yet!\n");
        }
    } else {
        printf("Invalid command! You lose your turn.\n");
    }

    end = clock();  // End timer
    currentPlayer->timeUsed += (end - start);  // Update total time used for the player
}

int checkVictory(Player *p1, Player *p2) {
    if (p1->shipsSunk == 5) {  // Check if Player 1 has sunk all ships
        printf("%s wins!\n", p1->name);
        if (p1->timeUsed < p2->timeUsed) {
            printf("%s wins the tiebreaker due to less time taken!\n", p1->name);
        } else if (p1->timeUsed > p2->timeUsed) {
            printf("%s wins the tiebreaker due to less time taken!\n", p2->name);
        }
        return 1;  // Game over
    }
    return 0;  // No winner yet
}