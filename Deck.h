//preprocessor guards
#ifndef DECK_H
#define DECK_H



#define DECK_SIZE 52



typedef enum {
    HEARTS,
    DIAMONDS,
    CLUBS,
    SPADES
} Suit;

typedef enum {
    TWO = 2, THREE, FOUR, FIVE,
    SIX, SEVEN, EIGHT, NINE, TEN,
    JACK, QUEEN, KING, ACE
} Rank;

typedef struct {
    Suit suit;
    Rank rank;
} Card;

typedef struct {
    Card cards[8];
    int count;
} Hand;


typedef struct {
    char name[32];
    Hand hand;
    int score;
} Player;


typedef struct {
    Card cards[DECK_SIZE];
    int top;
} Deck;

void initDeck(Deck *deck);
void shuffleDeck(Deck *deck);
Card drawCard(Deck *deck);





#endif
