#include <stdio.h>
#include "../../Deck.h"
#include "blackjack.h"

#define MAX_HAND 12

typedef struct {
    Card cards[MAX_HAND];
    int count;
} BJHand;

int cardValue(Card c) {
    if (c.rank >= JACK && c.rank <= KING)
        return 10;
    if (c.rank == ACE)
        return 11; // simplified (no soft ace handling yet)
    return c.rank;
}

int handTotal(BJHand *h) {
    int total = 0;
    for (int i = 0; i < h->count; i++) {
        total += cardValue(h->cards[i]);
    }
    return total;
}

void addCard(BJHand *h, Card c) {
    h->cards[h->count++] = c;
}

void printSimpleHand(char *name, BJHand *h) {
    printf("%s: ", name);
    for (int i = 0; i < h->count; i++) {
        printf("[%d] ", h->cards[i].rank);
    }
    printf(" (Total: %d)\n", handTotal(h));
}

void blackjack_start() {

    Deck deck;
    initDeck(&deck);
    shuffleDeck(&deck);

    BJHand player = { .count = 0 };
    BJHand dealer = { .count = 0 };

    // initial deal
    addCard(&player, drawCard(&deck));
    addCard(&dealer, drawCard(&deck));
    addCard(&player, drawCard(&deck));
    addCard(&dealer, drawCard(&deck));

    int input;

    while (1) {

        printf("\n================ BLACKJACK ================\n");

        printSimpleHand("PLAYER", &player);
        printf("DEALER: [%d] [?]\n", dealer.cards[0].rank);

        printf("\n(1) Hit\n(2) Stand\n> ");
        scanf("%d", &input);

        if (input == 1) {
            addCard(&player, drawCard(&deck));

            if (handTotal(&player) > 21) {
                printf("\nBUST! Dealer wins.\n");
                return;
            }
        }

        if (input == 2) {
            break;
        }
    }

    // dealer turn (simple AI)
    while (handTotal(&dealer) < 17) {
        addCard(&dealer, drawCard(&deck));
    }

    printf("\n=========== FINAL RESULT ===========\n");
    printSimpleHand("PLAYER", &player);
    printSimpleHand("DEALER", &dealer);

    int p = handTotal(&player);
    int d = handTotal(&dealer);

    if (d > 21 || p > d)
        printf("\nPLAYER WINS\n");
    else if (p < d)
        printf("\nDEALER WINS\n");
    else
        printf("\nPUSH\n");
}