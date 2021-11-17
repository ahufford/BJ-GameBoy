#include "beg.h"
#include "assets/coinTiles.h"
#include "assets/trashBarrel.h"

#define COIN_COUNT 10
struct Coin
{
    uint8_t x , y;
    uint8_t state;
    BOOLEAN onScreen;
};


/************************** Tile defs **************************/

const UWORD coinPalette[] = {

    coinCGBPal0c0, 
    coinCGBPal0c1, 
    coinCGBPal0c2, 
    coinCGBPal0c3,
};
const UWORD barrelPalette[] = {

    TrashBarrelCGBPal0c0,
    TrashBarrelCGBPal0c1, 
    TrashBarrelCGBPal0c2, 
    TrashBarrelCGBPal0c3
};

const UWORD coinBgPalette[] = {

    0x7FFF, 
    0x0000, 
    0x0000, 
    0x0000,
};

/************************** Game Controller Members **************************/
uint8_t loop = 0;
struct Coin *coins[10] = {0,0,0,0,0,0,0,0,0,0};
uint16_t cycle = 0;
uint8_t playerX = 50;
uint8_t playerY = 144;
uint8_t BARREL_WIDTH = 8;
uint16_t scraps = 0;
uint8_t input = 0;

/************************** Function Declarations **************************/
static void wait(int8_t loops);
static void loadTileData();
static void setUpGame();
static void moveCoins();
static void checkCollision();
static void printNum(uint8_t num, uint8_t x, uint8_t y);
static void drawString(char *s, uint8_t x, uint8_t y);





/******************************* Game Functions ***********************************/
// Start of begging minigame
uint16_t beg(uint16_t s) {
    initrand(s);

    loadTileData();
    setUpGame();

    while (1)
    {
        input = joypad();

        if (input & J_LEFT) {
           playerX--; 
            move_sprite(0,playerX, playerY);               
        }
        if (input & J_RIGHT) {
           playerX++;
           move_sprite(0,playerX, playerY);        
        }

        for(loop = 0; loop < COIN_COUNT; loop++){
            if (coins[loop]-> onScreen == FALSE){
                uint8_t randy = (((uint8_t) rand()) % (uint8_t) 160);
                if(randy == 1) {
                    coins[loop]->y = 0;
                    coins[loop]->onScreen = TRUE;
                }
            }
        }
        moveCoins();
        checkCollision();

        wait(1);
        cycle++;
    }

    return 0;
}

static void checkCollision() {
     for(loop = 0; loop < COIN_COUNT; loop++){
        if (coins[loop]->onScreen == TRUE && coins[loop]->y == 142){
            if ((coins[loop]->x > playerX && coins[loop]->x < playerX + BARREL_WIDTH) 
            ||  (playerX > coins[loop]->x && playerX < coins[loop]->x + BARREL_WIDTH) ){
                coins[loop]->onScreen = FALSE;
                coins[loop]->x = (((uint8_t) rand()) % (uint8_t) 160);
                move_sprite(loop + 1, 0,0);
                scraps++;
                printNum(scraps, 1, 17);
            }
        }
     }
}

static void moveCoins() {
    for(loop = 0; loop < COIN_COUNT; loop++){
        if(coins[loop]->onScreen == TRUE) {
            coins[loop]->y += 1;
            if (coins[loop]->y % 10 == 0){
                if (coins[loop]->state == 6){
                    coins[loop]->state = 1;
                } else {
                    coins[loop]->state += 1;
                }
                set_sprite_tile(loop + 1,  coins[loop]->state);
            }

            move_sprite(loop + 1, coins[loop]->x, coins[loop]->y);
        }
        if (coins[loop]->y > 160){
            coins[loop]->onScreen = FALSE;
            // This gives way better distribution if we pick a random when its done
            coins[loop]->x = (((uint8_t) rand()) % (uint8_t) 160);

        }
    }
}

static void setUpGame(){

    for(loop = 0; loop < COIN_COUNT; loop++) {
        // coin data
        struct Coin *c;
        c = malloc( sizeof(struct Coin) );
        coins[loop] = c;
        coins[loop]->x = loop * 30;
        coins[loop]->y = 0;
        coins[loop]->state = 1;
        coins[loop]->onScreen = FALSE;

        // coin sprites
        set_sprite_tile(loop + 1, 0);

    }
}

/********************************** Drawing Functions *****************************/

static void loadTileData() {
    set_sprite_data(0, 1, TrashBarrel);
    set_sprite_data(1, 6, coin);

    set_sprite_palette(0, 1, barrelPalette);
    set_sprite_palette(1, 1, coinPalette);

    // set barrel to 0th palette
    set_sprite_prop(0, 0);
    for (loop = 0; loop < COIN_COUNT; loop ++) {
        //coins all 1st palette
        set_sprite_prop(loop + 1, 1);
    }
    set_sprite_tile(0, 0);

    move_sprite(0, 50, 144);

    set_bkg_palette(0, 1, coinBgPalette);

    SHOW_SPRITES;
    SHOW_BKG;
    DISPLAY_ON;
}

static void drawString(char *s, uint8_t x, uint8_t y){

    char l = strlen(s);
    char i;
    char c;
    for(i = 0; i < l; i++){
        c = s[i] - (char)32;
        set_bkg_tiles(x + i, y, 1, 1, &c); 
    }
    
}

static void printNum(uint8_t num, uint8_t x, uint8_t y) {
    // create split string to display
    char *splitString = (char*)malloc(16 * sizeof(char));
    sprintf(splitString, "%d", num);
    drawString(splitString, x, y);
    free(splitString);

}

static void wait(int8_t loops) {
    int8_t i;
    for(i = 0; i < loops; i++){
        wait_vbl_done();
    }
}