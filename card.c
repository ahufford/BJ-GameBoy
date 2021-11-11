#include <gb/gb.h>
#include <stdlib.h>
#include <rand.h>


struct Card {
    char value;
    char suit;
    BOOLEAN faceDown;
};

struct Card* randomCard() {
    struct Card *c;
    c = malloc(sizeof(struct Card));
    c->value = (((uint8_t) rand()) % (uint8_t) 13) + 1;
    c->suit = (((uint8_t) rand()) % (uint8_t) 4) + 1;
    c->faceDown = FALSE;
    return c;
}

struct Card* giveCard(char v, char s, BOOLEAN fd) {
    struct Card *c;
    c = malloc(sizeof(struct Card));
    c->value = v;
    c->suit = s;
    c->faceDown = fd;
    return c;
}
