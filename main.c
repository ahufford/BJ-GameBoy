#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>
#include <stdio.h>
#include <gb/drawing.h>
#include <string.h>

//#include "CardTiles.h"
#include "assets/neoBjTiles.h"
#include "assets/neoBjMap.h"

#include "card.c"
#include "beg.h"

/************************** Tile defs **************************/

#define CARD_TILES_COUNT 75

#define FONT_NUMBERS_START 76
#define FONT_NUMBERS_COUNT 10

#define FONT_LETTERS_START 86
#define FONT_LETTERS_COUNT 26

#define FONT_EXCALMATION 112
#define FONT_QUESTION 113
#define FONT_SLASH 114
#define FONT_MONEY 75

#define PLAYER_HAND_X 2
#define PLAYER_HAND_Y 12

#define DEALER_HAND_X 2
#define DEALER_HAND_Y 2

#define PLAYER_HAND_VALUE_X 17
#define PLAYER_HAND_VALUE_Y 14

#define DEALER_HAND_VALUE_X 17
#define DEALER_HAND_VALUE_Y 2

#define MONEY_X 7
#define MONEY_Y 16

#define BET_X 1
#define BET_Y 7

#define RESULT_X 6
#define RESULT_Y 8

#define HAND_COUNT_X 2
#define HAND_COUNT_Y 16

UBYTE SPRITE_SIZE = 8;

unsigned char numbersTile = 0x10;
unsigned char letterTile = 0x21;

char startTile = 0x00;
unsigned char blankTile = 0x80;

unsigned char valueTiles = 0x81;
unsigned char suitTiles = 0x9D;
unsigned char cardBlankTiles = 0x9B;
unsigned char cardBacks = 0xA5;

unsigned char paletteFace1 = 0x00;
unsigned char paletteBack1 = 0x02;


const UWORD bgPalettes[] = {

    neoBjCardsCGBPal0c0, 
    neoBjCardsCGBPal0c1, 
    neoBjCardsCGBPal0c2, 
    neoBjCardsCGBPal0c3,

    neoBjCardsCGBPal1c0,
    neoBjCardsCGBPal1c1,
    neoBjCardsCGBPal1c2,
    neoBjCardsCGBPal1c3,

    neoBjCardsCGBPal2c0,
    neoBjCardsCGBPal2c1,
    neoBjCardsCGBPal2c2,
    neoBjCardsCGBPal2c3

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
uint16_t seed = 0;

BOOLEAN bust[5] = {0, 0, 0, 0, 0};
BOOLEAN blackJack[5] = {0, 0, 0, 0, 0};

BOOLEAN stay = FALSE;
BOOLEAN dealerBust = FALSE;
BOOLEAN firstCard = TRUE;
BOOLEAN betting = TRUE;
BOOLEAN doubledDown = FALSE;

uint8_t splitCount = 0;

// For loops 
uint8_t i = 0;
/******************************************************************************/


/************************** Function Declarations **************************/
static void wait(int8_t loops);

void loadTileData();
void loadMap();
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
void showInstructions();
void initalDeal();

uint8_t playerTotal();
uint8_t dealerTotal();
/******************************************************************************/

void main(void)
{
    //Loads the standard font tiles
    printf(" ");
    loadTileData();

    showInstructions();

    //Use the user input to generate a seed #
    waitpad(0xFF);
    seed = LY_REG;
    seed |= (uint16_t)DIV_REG << 8;
    initrand(seed);

    // clear the bg
    uint8_t z;
    for (z = 0; z < 18; z++) {
        for (i = 0; i < 20; i++) {
            set_bkg_tiles(i, z, 1, 1, &blankTile);
        }
    }

    loadMap();

    while(1) {
        for (i = 0; i < 5; i++) {
            bust[i] = FALSE;
            blackJack[i] = FALSE;
        }
        stay = FALSE;
        dealerBust = FALSE;
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

        drawString("PLACE BET", RESULT_X, RESULT_Y);

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
                wait(4);          
            }
            if (joydata & J_DOWN) {
                betSize--;
                if (betSize == 0) {
                    betSize = 1;
                }
                printMoney();
                wait(4);          
            }
            if (joydata & J_LEFT) {
                betSize -= 10;
                if (betSize < 1) {
                    betSize = 1;
                }
                printMoney();
                wait(4);          
            }
            if (joydata & J_RIGHT) {
                betSize += 10;
                if (betSize > money) {
                    betSize = money;
                }
                printMoney();
                wait(4);          
            }

            wait(1);          
        }
        clearResult();

        money -= betSize;
        printMoney();

        initalDeal();

        updateTotals(playerTotal(), dealerTotal());

        if(playerTotal() == 21){
            blackJack[currentHand] = TRUE;
        }
        waitpadup();
        
        firstCard = TRUE;
        if (blackJack[currentHand] == FALSE){
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
                        drawString("DOUBLE DOWN!", RESULT_X, RESULT_Y);
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
                //******************* SPLIT ****************//
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
                        
                        if(playerTotal() == 21){
                            drawString("BLACK JACK!", RESULT_X, RESULT_Y);
                            blackJack[currentHand] = TRUE;
                            wait(100);
                            clearResult();
                            goToNextHand();
                        }

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
            } else if (blackJack[currentHand] == TRUE) {
                money += betSize * 3;
                printResult(3);
            } else if(dealerBust == TRUE) { // WIN
                money += betSize * 2;
                printResult(0);
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

        if(money == 0){
            drawString("U BROKE NOOB!", RESULT_X, RESULT_Y);
            waitpad(J_A);
            money = beg();
            move_bkg(0, 0);
            betSize = 5;
            loadMap();
        }
    }

}

/**************************************************************************/

/************************** Game Functions *******************************/
void initalDeal() {
        // inital deal
        // HACK to get a specific card
        // playerHand[0][0] = giveCard(1, 2, FALSE);
        // playerCardsCount[0]++;
        // playerHand[0][1] = giveCard(1, 1, FALSE);
        // playerCardsCount[0]++;

        dealToPlayer();
        drawPlayersCards();
        updateTotals(playerTotal(), dealerTotal());
        wait(50);

        dealToDealer(TRUE);
        drawDealersCards();
        wait(50);

        dealToPlayer();
        drawPlayersCards();
        updateTotals(playerTotal(), dealerTotal());
        wait(50);

        dealToDealer(FALSE);
        drawDealersCards();
        updateTotals(playerTotal(), dealerTotal());
}

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
    if(playerTotal() == 21){
        blackJack[currentHand] = TRUE;
        if (playerHandCount != currentHand + 1) {
            goToNextHand();
        } else {
            stay = TRUE;
        }
    }
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

        // positions
        char b0 = cardBacks;
        char b1 = cardBacks + sizeof(char);
        char b2 = cardBacks + sizeof(char) * 2;
        char b3 = cardBacks + sizeof(char) * 3;
        char b4 = cardBacks + sizeof(char) * 4;
        char b5 = cardBacks + sizeof(char) * 5;

        //set Palette
        VBK_REG = 1;
        set_bkg_tiles(x, y, 1, 1, &paletteBack1);
        set_bkg_tiles(x + 1, y, 1, 1, &paletteBack1);
        set_bkg_tiles(x, y + 1, 1, 1, &paletteBack1);
        set_bkg_tiles(x + 1, y + 1, 1, 1, &paletteBack1);
        set_bkg_tiles(x, y + 2, 1, 1, &paletteBack1);
        set_bkg_tiles(x + 1, y + 2, 1, 1, &paletteBack1);

        //set tiles
        VBK_REG = 0;
        set_bkg_tiles(x, y, 1, 1, &b0);
        set_bkg_tiles(x + 1, y, 1, 1, &b1);
        set_bkg_tiles(x, y + 1, 1, 1, &b2);
        set_bkg_tiles(x + 1, y + 1, 1, 1, &b3);
        set_bkg_tiles(x, y + 2, 1, 1, &b4);
        set_bkg_tiles(x + 1, y + 2, 1, 1, &b5);

    } else { 
        //set Palette
        VBK_REG = 1;
        set_bkg_tiles(x, y, 1, 1, &paletteFace1);
        set_bkg_tiles(x + 1, y, 1, 1, &paletteFace1);
        set_bkg_tiles(x, y + 1, 1, 1, &paletteFace1);
        set_bkg_tiles(x + 1, y + 1, 1, 1, &paletteFace1);
        set_bkg_tiles(x, y + 2, 1, 1, &paletteFace1);
        set_bkg_tiles(x + 1, y + 2, 1, 1, &paletteFace1);

        VBK_REG = 0;

        char v = valueTiles + (c->value - 1);
        // Value
        set_bkg_tiles(x, y, 1, 1, &v);
        v += sizeof(char) * 13;
        set_bkg_tiles(x + 1, y + 2, 1, 1, &v);

        // Blanks
        v = cardBlankTiles;
        set_bkg_tiles(x, y + 1, 1, 1, &v);
        v += sizeof(char);
        set_bkg_tiles(x + 1, y + 1, 1, 1, &v);

        // Suits
        v = suitTiles + ((c->suit - 1) * 2);
        set_bkg_tiles(x + 1, y, 1, 1, &v);
        v = suitTiles + ((c->suit - 1) * 2) + sizeof(char);
        set_bkg_tiles(x, y + 2, 1, 1, &v);
    }
}

void clearCard(int8_t x, int8_t y) {
    set_bkg_tiles(x, y, 1, 1, &blankTile);
    set_bkg_tiles(x + 1, y, 1, 1, &blankTile);
    set_bkg_tiles(x, y + 1, 1, 1, &blankTile);
    set_bkg_tiles(x + 1, y + 1, 1, 1, &blankTile);
    set_bkg_tiles(x, y + 2, 1, 1, &blankTile);
    set_bkg_tiles(x + 1, y + 2, 1, 1, &blankTile);

}

void drawPlayersCards() {
    uint8_t bigHand = 3;
    if (playerCardsCount[currentHand] > 5) {
        if (playerCardsCount[currentHand] == 6) {
            clearPlayerCards();
        }
        bigHand = 2;
    }

    uint8_t i;
    for (i = 0; i < playerCardsCount[currentHand]; i++) {
        drawCard(playerHand[currentHand][i], PLAYER_HAND_X + (i*bigHand), PLAYER_HAND_Y);
    }
}

void clearPlayerCards() {
    uint8_t i;
    for (i = 0; i < 13; i++) {
        clearCard(PLAYER_HAND_X + i, PLAYER_HAND_Y);
    }

}

void drawDealersCards() {
    uint8_t bigHand = 3;
    if (dealerCardsCount > 5) {
        if (dealerCardsCount == 6) {
            clearDealersCards();
        }
        bigHand = 2;
    }
    uint8_t i;
    for (i = 0; i < dealerCardsCount; i++) {
        drawCard(dealersHand[i], DEALER_HAND_X + (i*bigHand), DEALER_HAND_Y);
    }
}
void clearDealersCards() {
    uint8_t i;
    for (i = 0; i < 13; i++) {
        clearCard(DEALER_HAND_X + i, DEALER_HAND_Y);
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
    sprintf(splitString, "%d/%d", currentHand + 1, playerHandCount);
    drawString(splitString, HAND_COUNT_X, HAND_COUNT_Y);
    free(splitString);

}
void clearHandCount() {
    uint8_t i;
    for(i = HAND_COUNT_X; i < HAND_COUNT_X + 4; i++){
        set_bkg_tiles(i, HAND_COUNT_Y, 1, 1, &blankTile);
    }
    
}

void clearTotals(){
        set_bkg_tiles(PLAYER_HAND_VALUE_X, PLAYER_HAND_VALUE_Y, 1, 1, &blankTile);
        set_bkg_tiles(PLAYER_HAND_VALUE_X + 1, PLAYER_HAND_VALUE_Y, 1, 1, &blankTile);

        set_bkg_tiles(DEALER_HAND_VALUE_X, DEALER_HAND_VALUE_Y, 1, 1, &blankTile);
        set_bkg_tiles(DEALER_HAND_VALUE_X +1, DEALER_HAND_VALUE_Y, 1, 1, &blankTile);
}

void updateTotals(uint8_t p, uint8_t d) {


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

    //updating dealers total
    ones = d % 10;
    tens = d / 10;
    o = numbersTile + ones;
    t = numbersTile + tens;

    set_bkg_tiles(DEALER_HAND_VALUE_X + 1, DEALER_HAND_VALUE_Y, 1, 1, &o);
    if (tens != 0) {
        set_bkg_tiles(DEALER_HAND_VALUE_X, DEALER_HAND_VALUE_Y, 1, 1, &t);
    } else {
        set_bkg_tiles(DEALER_HAND_VALUE_X, DEALER_HAND_VALUE_Y, 1, 1, &blankTile);
    }

   


}

void printResult(uint8_t result){
    if (result == 0){ // win
        drawString("YOU WIN!", RESULT_X, RESULT_Y);
    }
    else if (result == 1){ //lose
        drawString("YOU LOSE", RESULT_X, RESULT_Y);
    }
    else if (result == 2){ // push
        drawString("PUSH", RESULT_X, RESULT_Y);
    }
    else { //bj
        drawString("BLACK JACK!", RESULT_X, RESULT_Y);
    }
    
}

void clearResult() {
    uint8_t i;
    for(i = RESULT_X; i < RESULT_X + 15; i++){
        set_bkg_tiles(i, RESULT_Y, 1, 1, &blankTile);
    }
    
}

void printMoney(){

    uint8_t l;
    char str[10];
    //gotta be a better way lol
    if (money > 9999) {
        sprintf(str, "$%d", money);
    } else if (money < 10000 && money > 999) {
        sprintf(str, "$%d ", money);
    } else if (money < 1000 && money > 99) {
        sprintf(str, "$%d  ", money);
    } else if (money < 100 && money > 9) {
        sprintf(str, "$%d   ", money);
    } else {
        sprintf(str, "$%d    ", money);   
    }

    l = strlen(str);
    drawString(str, MONEY_X, MONEY_Y);

    char str2[10];
    if (betSize > 99) {
        sprintf(str2, "$%d", betSize);
    } else if (betSize < 100 && betSize > 9) {
        sprintf(str2, "$%d ", betSize);
    } else {
        sprintf(str2, "$%d  ", betSize);
    }

    l = strlen(str2);
    drawString(str2, BET_X, BET_Y + 1);

}

void clearMoney(){
    uint8_t i;
    for(i = MONEY_X; i < MONEY_X + 5; i++){
        set_bkg_tiles(i, MONEY_Y, 1, 1, &blankTile);
    }
    for(i = BET_X; i < BET_Y + 3; i++){
        set_bkg_tiles(i, BET_Y, 1, 1, &blankTile);
    }
    wait(1);
}

void showInstructions() {

    drawString("A      HIT", 1, 5);
    drawString("B      STAY", 1, 7);
    drawString("START  SPLIT", 1, 9);
    drawString("SELECT DOUBLE DOWN", 1 , 11);

}

void loadTileData() {

    //load 3 palettes
    set_bkg_palette(0, 3, &bgPalettes[0]);

    //load into the 2nd chunk of VRAM
    set_bkg_data(128u, CARD_TILES_COUNT, &neoBjCards[0]);
    
    //load space as black blanks tile
    set_bkg_data(0u, 1, &neoBjCards[0]);
    // Replace with our numbers
    set_bkg_data(16u, FONT_NUMBERS_COUNT, &neoBjCards[FONT_NUMBERS_START * 16]);
    // Replace with our capital letters
    set_bkg_data(33u, FONT_LETTERS_COUNT, &neoBjCards[FONT_LETTERS_START * 16]);

    // Replace some punctuation 
    set_bkg_data(1u, 1, &neoBjCards[FONT_EXCALMATION * 16]);
    set_bkg_data(15u, 1, &neoBjCards[FONT_SLASH * 16]);
    set_bkg_data(31u, 1, &neoBjCards[FONT_QUESTION * 16]);
    set_bkg_data(4u, 1, &neoBjCards[FONT_MONEY * 16]);

    SHOW_BKG;
    DISPLAY_ON;
}

void loadMap(){

    //change bank
    VBK_REG = 1;
    //load the palette properties
    set_bkg_tiles(0, 0, 20, 18, neoBjMapPLN1);

    VBK_REG = 0;
    set_bkg_tiles(0, 0, 20, 18, neoBjMapPLN0);

    //write BET (BG MAP messed up)
    drawString("BET", BET_X, BET_Y - 1);
}
/******************************************************************************/

static void wait(int8_t loops) {
    int8_t i;
    for(i = 0; i < loops; i++){
        wait_vbl_done();
    }
}
