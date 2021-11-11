#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include <stdio.h>
#include <gb/drawing.h>
#include <string.h>

#include "CardTiles.h"
#include "card.c"

/************************** Tile defs **************************/

#define SPRITE_TILES_COUNT 32

#define PLAYER_HAND_VALUE_X 17
#define PLAYER_HAND_VALUE_Y 16

#define DEALER_HAND_VALUE_X 17
#define DEALER_HAND_VALUE_Y 2

#define MONEY_X 15
#define MONEY_Y 10

#define RESULT_X 6
#define RESULT_Y 8

#define HAND_COUNT_X 4
#define HAND_COUNT_Y 16

UBYTE SPRITE_SIZE = 8;

unsigned char numbersTile = 0x10;
unsigned char lettersCapsTile = 0x21;
unsigned char lettersLowerTile = 0x41;
unsigned char punctuationTile = 0x01;

char startTile = 0x00;
unsigned char blankTile = 0x00;
unsigned char spadesTile = 0x80;
unsigned char spadesTile2 = 0x81;
unsigned char heartsTile = 0x82;
unsigned char heartsTile2 = 0x83;
unsigned char clubsTile = 0x84;
unsigned char clubsTile2 = 0x85;
unsigned char diamondsTile = 0x86;
unsigned char diamondsTile2 = 0x87;
unsigned char topRightTile = 0x88;
unsigned char valueTiles = 0x89;
//Card backTiles
unsigned char b1Tiles = 0x98;
unsigned char b2Tiles = 0x9C;

const UWORD bgPalette[] = {

    CardTilesCGBPal0c0, 
    CardTilesCGBPal0c1, 
    CardTilesCGBPal0c2, 
    CardTilesCGBPal0c3 

};

/******************************************************************************/


/************************** Game Controller Members **************************/
// Declare and init variables here to make them in RAM and not a stack variable <- STACK = SLOWWWWW
struct Card *playerHand[5][10]= {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
uint8_t playerCardsCount[5] = {0, 0, 0, 0, 0};

uint8_t playerHandCount = 1;
uint8_t currentHand = 0;


struct Card *dealersHand[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t dealerCardsCount = 0;

int16_t money = 105;
int16_t betSize = 5;

int8_t x = 0;
uint8_t joydata = 0;

BOOLEAN bust[5] = {0, 0, 0, 0, 0};
BOOLEAN stay = FALSE;
BOOLEAN dealerBust = FALSE;
BOOLEAN blackJack = FALSE;
BOOLEAN firstCard = TRUE;
BOOLEAN betting = TRUE;
BOOLEAN doubledDown = FALSE;

uint8_t splitCount = 0;

// For loops 
uint8_t i = 0;
/******************************************************************************/


/************************** Function Declarations **************************/
void loadTileData();
void wait(int8_t loops);
void drawCard(struct Card *c, int8_t x, int8_t y);
void clearCard(int8_t x, int8_t y);
void dealToPlayer();
void dealToDealer(BOOLEAN faceDown);
void drawPlayersCards();
void clearPlayerCards();
void drawDealersCards();
void clearDealersCards();
void updateTotals(uint8_t p, uint8_t d);
void clearTotals();
void printResult(uint8_t result);
void clearResult();
void printMoney();
void clearMoney();
void drawString(char *s, uint8_t x, uint8_t y);
void clearHands();
void goToNextHand();
void drawHandCount();
void clearHandCount();

uint8_t playerTotal();
uint8_t dealerTotal();
/******************************************************************************/


void main(void)
{
    loadTileData();

    // this also loads the characters/numbers for us
    printf("\n");
    printf("\n\nA        Hit");
    printf("\n\nB        Stay");
    printf("\n\nStart    Split");
    printf("\n\nSelect   Double Down");

    //Use the user input to generate a seed #
    waitpad(0xFF);
    uint16_t seed = LY_REG;
    seed |= (uint16_t)DIV_REG << 8;
    initrand(seed);

    // clear the bg
    uint8_t z;
    for (z = 0; z < 18; z++) {
        for (i = 0; i < 20; i++) {
            set_bkg_tiles(i, z, 1, 1, &blankTile);
        }
    }
    SHOW_SPRITES;
    SHOW_BKG;


    while(money > 0) {
        for (i = 0; i < 5; i++) {
            bust[i] = FALSE;
        }
        stay = FALSE;
        dealerBust = FALSE;
        blackJack = FALSE;
        splitCount = 0;
        doubledDown = FALSE;

        clearPlayerCards();
        clearDealersCards();
        clearTotals();
        clearResult();
        clearHandCount();

        betting = TRUE;
        if (betSize > money) {
            betSize = money;
        }

        printMoney();

        drawString("Place bet", RESULT_X, RESULT_Y);

        while(betting == TRUE) {
            joydata = joypad();
            if (joydata & J_A) {
                betting = FALSE;
            }           
            if (joydata & J_UP) {
                betSize++;
                if (betSize == money + 1) {
                    betSize = money;
                }
                printMoney();
            }
            if (joydata & J_DOWN) {
                betSize--;
                if (betSize == 0) {
                    betSize = 1;
                }
                printMoney();
            }
            if (joydata & J_LEFT) {
                betSize -= 10;
                if (betSize < 0) {
                    betSize = 1;
                }
                printMoney();
            }
            if (joydata & J_RIGHT) {
                betSize += 10;
                if (betSize > money) {
                    betSize = money;
                }
                printMoney();
            }

            wait(5);          
        }
        clearResult();

        money -= betSize;
        // inital deal
        // HACK to get a specific card
        //playerHand[0][0] = giveCard(8, 2, FALSE);
        //playerCardsCount[0]++;
        //playerHand[0][1] = giveCard(8, 1, FALSE);
        //playerCardsCount[0]++;
        dealToPlayer();
        dealToPlayer();

        dealToDealer(TRUE);
        dealToDealer(FALSE);

        drawPlayersCards();
        drawDealersCards();
        updateTotals(playerTotal(), dealerTotal());
        printMoney();

        if(playerTotal() == 21){
            blackJack = TRUE;
        }
        waitpadup();
        
        firstCard = TRUE;
        if (blackJack == FALSE){
            // Players Turn
            while (stay == FALSE) {
                //Get Joypad input
                joydata = joypad();

                // A is HIT
                if (joydata & J_A) 
                {
                    // Flag 1st card so we cant double / split anymore
                    firstCard = FALSE;
                    dealToPlayer();
                    drawPlayersCards();
                    updateTotals(playerTotal(), dealerTotal());

                    //check bust
                    if (playerTotal() > 21) {
                        bust[currentHand] = TRUE;
                        // Check this is our last hand (if we split)
                        if (playerHandCount == currentHand + 1) {
                            stay = TRUE; // mark stay to end players turn
                        } else {
                            wait(100);
                            goToNextHand();
                        }
                    } 
                    waitpadup();
                    
                }
                // B is to stay
                if (joydata & J_B) {
                    // If this is our last hand
                    if (playerHandCount == currentHand + 1) {
                        stay = TRUE;
                    } else { // Move to next hand if not our last
                        
                        goToNextHand();
                    }
                    waitpadup();
                }
                // Select is to double down!
                if (joydata & J_SELECT) {
                    // Make sure we have enough $$ and its 1st card
                    if(money - 1 > betSize && firstCard == TRUE) {
                        drawString("Double down!", RESULT_X - 2, RESULT_Y);
                        doubledDown = TRUE;
                        // Double the bet!
                        money -= betSize;
                        betSize *= 2;
                        printMoney();
                        wait(50);
                        dealToPlayer();
                        drawPlayersCards();
                        updateTotals(playerTotal(), dealerTotal());

                        //check bust
                        if (playerTotal() > 21) {
                            bust[currentHand] = TRUE;
                        }
                        // Check this is our last hand (if we split)
                        if (playerHandCount == currentHand + 1) {
                            stay = TRUE; // mark stay to end players turn
                        } else {
                            goToNextHand();

                        }

                        waitpadup();
                        wait(100);          

                    }
                }
                if (joydata & J_START) {
                    if (money - 1 > betSize && playerHand[currentHand][0]->value == playerHand[currentHand][1]->value && firstCard == TRUE) {
                        splitCount++;
                        money -= betSize;
                        printMoney();
                        playerHand[splitCount][0] = playerHand[currentHand][1];
                        playerCardsCount[currentHand]--;
                        playerCardsCount[splitCount]++;
                        playerHandCount++;

                        drawHandCount();

                        clearPlayerCards();
                        drawPlayersCards();
                        updateTotals(playerTotal(), dealerTotal());
                        wait(50);
                        dealToPlayer();
                        clearTotals();
                        updateTotals(playerTotal(), dealerTotal());
                        drawPlayersCards();
                    }
                }

                wait(1);
            }

            clearResult();

            // Check if all our hands busted
            BOOLEAN allBusts = TRUE;
            for (i = 0; i < playerHandCount; i++){
                if (bust[i] == FALSE){
                    allBusts = FALSE;
                }
            }
            // Computer Turn 
            if (allBusts == FALSE) {
                dealerBust = FALSE;
                //Flip the card
                dealersHand[0]->faceDown = FALSE;
                drawDealersCards();
                updateTotals(playerTotal(), dealerTotal());
                wait(50);

                while(dealerTotal() < 17 && dealerBust == FALSE) {
                    dealToDealer(FALSE);
                    drawDealersCards();
                    updateTotals(playerTotal(), dealerTotal());
                    if (dealerTotal() > 21) {
                        dealerBust = TRUE;
                    } else {
                        wait(50);
                    }
                }
            }
        }

        clearTotals();
        //Check Results
        for (currentHand = 0; currentHand < playerHandCount; currentHand++) {

            if (playerHandCount > 1) {
                drawHandCount();
            }

            clearPlayerCards();
            drawPlayersCards();
            updateTotals(playerTotal(), dealerTotal());

            if (bust[currentHand] == TRUE){ // LOSS
                printResult(1);
            } else if(dealerBust == TRUE) { // WIN
                money += betSize * 2;
                printResult(0);
            } else if (blackJack == TRUE) {
                money += betSize * 3;
                printResult(3);
            } else if (dealerTotal() == playerTotal()) { //TIE
                money += betSize;
                printResult(2);
            } else if (dealerTotal() < playerTotal()) { //WIN
                money += betSize * 2;
                printResult(0);
            } else { // LOSE dealer > player
                printResult(1);
            }
            printMoney();
            if (playerHandCount != currentHand + 1) {
                waitpad(J_A);
                waitpadup();
                clearTotals();
                clearResult();

            }

        }
        
        // if doubled down, put the bet back to what it was;
        if(doubledDown == TRUE){
            betSize = betSize / 2;
        }
        printMoney();
        clearHands();

        waitpad(J_A);
        waitpadup();

    }
    drawString("U BROKE!", RESULT_X, RESULT_Y);
}

/**************************************************************************/

/************************** Game Functions *******************************/
void goToNextHand() {

    currentHand++;
    drawHandCount();

    clearPlayerCards();
    drawPlayersCards();
    clearTotals();
    updateTotals(playerTotal(), dealerTotal());
    // wait a little, then deal the 2nd card
    wait(50);

    dealToPlayer();
    drawPlayersCards();
    clearTotals();
    updateTotals(playerTotal(), dealerTotal());
    // Reset so we can double down / split again
    firstCard = TRUE;
}


void clearHands() {
    uint8_t j;
    for(j = 0; j < playerHandCount; j ++) {
        for(i = 0; i < playerCardsCount[j]; i ++) {
            free(playerHand[j][i]);
        }
    }

    for(i = 0; i < dealerCardsCount; i ++) {
        free(dealersHand[i]);
    }
    for(i = 0; i < 5; i ++) {
        playerCardsCount[i] = 0;
    }
    
    playerHandCount = 1;
    dealerCardsCount = 0;
    currentHand = 0;
}


void dealToPlayer(){
    playerHand[currentHand][playerCardsCount[currentHand]] = randomCard();
    playerCardsCount[currentHand]++;
}

void dealToDealer(BOOLEAN faceDown){
    dealersHand[dealerCardsCount] = randomCard();
    if (faceDown == TRUE) {
        dealersHand[dealerCardsCount]->faceDown = TRUE;
    }
    dealerCardsCount++;
}

uint8_t playerTotal(){
    uint8_t total = 0;
    BOOLEAN hasAce = FALSE;
    uint8_t i;
    for (i = 0; i < playerCardsCount[currentHand]; i++){
        if(playerHand[currentHand][i]->value == 1){
            hasAce = TRUE;
        }
    }
    for (i = 0; i < playerCardsCount[currentHand]; i++){
        uint8_t v = playerHand[currentHand][i]->value;
        if (v > 10) {
            v = 10;
        }
        if(v == 1 && total + 11 < 22){
            v = 11;
        }
        total += v;
    }
    if (hasAce == TRUE && total > 21){
        total -= 10;
    }
    return total;
}

uint8_t dealerTotal(){
    uint8_t total = 0;
    BOOLEAN hasAce = FALSE;
    uint8_t i;
    for (i = 0; i < dealerCardsCount; i++){
        if(dealersHand[i]->value == 1){
            hasAce = TRUE;
        }
    }
    for (i = 0; i < dealerCardsCount; i++){
            if (dealersHand[i]->faceDown == FALSE){
            uint8_t v = dealersHand[i]->value;
            if (v > 10) {
                v = 10;
            }
            if(v == 1 && total + 11 < 22){
                v = 11;
            }
            total += v;
        }
    }
    if (hasAce == TRUE && total > 21){
        total -= 10;
    }
    return total;
}


/********************************** Drawing Functions *****************************/
//Takes in a card pointer and X&Y
void drawCard(struct Card *c, int8_t x, int8_t y) {
    if (c->faceDown == TRUE) {
        char b1 = b2Tiles;
        char b2 = b2Tiles + sizeof(char);
        char b3 = b2Tiles + sizeof(char) * 2;
        char b4 = b2Tiles + sizeof(char) * 3;
        set_bkg_tiles(x, y, 1, 1, &b1);
        set_bkg_tiles(x, y + 1, 1, 1, &b2);
        set_bkg_tiles(x + 1, y, 1, 1, &b3);
        set_bkg_tiles(x + 1, y + 1, 1, 1, &b4);

    } else { 
        char v = valueTiles + (c->value - 1);
        //Top left, value
        set_bkg_tiles(x, y, 1, 1, &v);

        //Top right always blank
        set_bkg_tiles(x + 1, y, 1, 1, &topRightTile);

        char s1 = spadesTile + ((c->suit - 1) * 2);
        char s2 = spadesTile2 + ((c->suit - 1) * 2);

        //Bottom 2 (Suit)
        set_bkg_tiles(x, y + 1, 1, 1, &s1);
        set_bkg_tiles(x + 1, y + 1, 1, 1, &s2);
        }
}

void clearCard(int8_t x, int8_t y) {
    set_bkg_tiles(x, y, 1, 1, &blankTile);
    set_bkg_tiles(x + 1, y, 1, 1, &blankTile);
    set_bkg_tiles(x, y + 1, 1, 1, &blankTile);
    set_bkg_tiles(x + 1, y + 1, 1, 1, &blankTile);

}

void drawPlayersCards() {
    uint8_t bigHand = 2;
    if (playerCardsCount[currentHand] > 10) {
        if (playerCardsCount[currentHand] == 11) {
            clearPlayerCards();
        }
        bigHand = 1;
    }
    uint8_t i;
    for (i = 0; i < playerCardsCount[currentHand]; i++) {
        drawCard(playerHand[currentHand][i], i * bigHand, 13);
    }
}
void clearPlayerCards() {
    uint8_t i;
    for (i = 0; i < 10; i++) {
        clearCard(i * 2, 13);
    }

}

void drawDealersCards() {
    uint8_t bigHand = 2;
    if (dealerCardsCount > 10) {
        if (dealerCardsCount == 11) {
            clearDealersCards();
        }
        bigHand = 1;
    }
    uint8_t i;
    for (i = 0; i < dealerCardsCount; i++) {
        drawCard(dealersHand[i], i * bigHand, 5);
    }
}
void clearDealersCards() {
    uint8_t i;
    for (i = 0; i < 10; i++) {
        clearCard(i * 2, 5);
    }

}
//******************************* String drawing *********************************/

void drawString(char *s, uint8_t x, uint8_t y){

    char l = strlen(s);
    char i;
    char c;
    for(i = 0; i < l; i++){
        c = s[i] - (char)32;
        set_bkg_tiles(x + i, y, 1, 1, &c); 
    }
    
}

void drawHandCount() {
    // create split string to display
    char *splitString = (char*)malloc(16 * sizeof(char));
    sprintf(splitString, "Hand: %d/%d", currentHand + 1, playerHandCount);
    drawString(splitString, HAND_COUNT_X, HAND_COUNT_Y);
    free(splitString);

}
void clearHandCount() {
    uint8_t i;
    for(i = HAND_COUNT_X; i < HAND_COUNT_X + 14; i++){
        set_bkg_tiles(i, HAND_COUNT_Y, 1, 1, &blankTile);
    }
    
}


void clearTotals(){
        set_bkg_tiles(PLAYER_HAND_VALUE_X - 2, PLAYER_HAND_VALUE_Y, 1, 1, &blankTile);
        set_bkg_tiles(PLAYER_HAND_VALUE_X - 1, PLAYER_HAND_VALUE_Y, 1, 1, &blankTile);
        set_bkg_tiles(PLAYER_HAND_VALUE_X, PLAYER_HAND_VALUE_Y, 1, 1, &blankTile);
        set_bkg_tiles(PLAYER_HAND_VALUE_X + 1, PLAYER_HAND_VALUE_Y, 1, 1, &blankTile);
        set_bkg_tiles(DEALER_HAND_VALUE_X - 2, DEALER_HAND_VALUE_Y, 1, 1, &blankTile);
        set_bkg_tiles(DEALER_HAND_VALUE_X - 1, DEALER_HAND_VALUE_Y, 1, 1, &blankTile);
        set_bkg_tiles(DEALER_HAND_VALUE_X, DEALER_HAND_VALUE_Y, 1, 1, &blankTile);
        set_bkg_tiles(DEALER_HAND_VALUE_X + 1, DEALER_HAND_VALUE_Y, 1, 1, &blankTile);  
}

void updateTotals(uint8_t p, uint8_t d) {

    //updating players total
    if (p < 22) {
        uint8_t ones = p % 10;
        uint8_t tens = p / 10;
        char o = numbersTile + ones;
        char t = numbersTile + tens;

        set_bkg_tiles(PLAYER_HAND_VALUE_X + 1, PLAYER_HAND_VALUE_Y, 1, 1, &o);
        if (tens != 0) {
            set_bkg_tiles(PLAYER_HAND_VALUE_X, PLAYER_HAND_VALUE_Y, 1, 1, &t);
        } else {
            set_bkg_tiles(PLAYER_HAND_VALUE_X, PLAYER_HAND_VALUE_Y, 1, 1, &blankTile);
        }
    } else {
        drawString("Bust  ", PLAYER_HAND_VALUE_X - 2, PLAYER_HAND_VALUE_Y);
    }

    if (d < 22) {
    
        //updating dealers total
        uint8_t ones = d % 10;
        uint8_t tens = d / 10;
        char o = numbersTile + ones;
        char t = numbersTile + tens;

        set_bkg_tiles(18, 2, 1, 1, &o);
        if (tens != 0) {
            set_bkg_tiles(17, 2, 1, 1, &t);
        } else {
            set_bkg_tiles(17, 2, 1, 1, &blankTile);
        }

    } else {
        drawString("Bust  ", DEALER_HAND_VALUE_X -2 , DEALER_HAND_VALUE_Y);
    }


}

void printResult(uint8_t result){
    if (result == 0){ // win
        drawString("You win!", RESULT_X, RESULT_Y);
    }
    else if (result == 1){ //lose
        drawString("You lose", RESULT_X, RESULT_Y);
    }
    else if (result == 2){ // push
        drawString("Push", RESULT_X, RESULT_Y);
    }
    else { //bj
        drawString("Black Jack!", RESULT_X - 2, RESULT_Y);
    }
    
}

void clearResult() {
    uint8_t i;
    for(i = RESULT_X - 4; i < RESULT_X + 15; i++){
        set_bkg_tiles(i, RESULT_Y, 1, 1, &blankTile);
    }
    
}

void printMoney(){

    uint8_t l;
    char str[10];
    sprintf(str, "$%d     ", money);
    l = strlen(str);
    drawString(str, MONEY_X, MONEY_Y);

    char str2[10];
    sprintf(str2, "%d    ", betSize);
    l = strlen(str2);
    drawString(str2, MONEY_X+ 1, MONEY_Y + 1);

}

void clearMoney(){
    uint8_t i;
    for(i = MONEY_X; i < MONEY_X + 5; i++){
        set_bkg_tiles(i, MONEY_Y, 1, 1, &blankTile);
        set_bkg_tiles(i - 4, MONEY_Y + 1, 1, 1, &blankTile);
    }
    wait(1);
}

void loadTileData() {

    set_bkg_data(128u, SPRITE_TILES_COUNT, CardTiles);

    //load palettes
    set_bkg_palette(0, 1, bgPalette);
}
/******************************************************************************/

void wait(int8_t loops) {
    int8_t i;
    for(i = 0; i < loops; i++){
        wait_vbl_done();
    }
}
