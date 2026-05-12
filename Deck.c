#include "Deck.h"
#include <stdlib.h>
#include <time.h>

// -------------------- INITIALIZE DECK --------------------

void initDeck(Deck *deck) {
    int index = 0;

    deck->top = 0;

    for (int suit = HEARTS; suit <= SPADES; suit++) {

        for (int rank = TWO; rank <= ACE; rank++) {

            deck->cards[index].suit = (Suit)suit;
            deck->cards[index].rank = (Rank)rank;

            index++;
        }
    }
}

// -------------------- DRAW CARD --------------------

Card drawCard(Deck *deck) {
    return deck->cards[deck->top++];
}

// -------------------- SHUFFLE DECK --------------------

void shuffleDeck(Deck *deck) {

    srand((unsigned int)time(NULL));

    for (int i = DECK_SIZE - 1; i > 0; i--) {

        int j = rand() % (i + 1);

        Card temp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temp;
    }

    deck->top = 0;
}
