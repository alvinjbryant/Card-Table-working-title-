#include <stdio.h>
#include <stdlib.h>
#include "Deck.h"

void clearScreen() {
    system("clear"); // use "cls" on Windows
}

void printHand(Card hand[], int count) {

    const char *suits[] = {"♥", "♦", "♣", "♠"};
    const char *ranks[] = {
        "", "", "2","3","4","5","6","7","8","9",
        "10","J","Q","K","A"
    };

    // TOP BORDER
    for (int i = 0; i < count; i++) {
        printf("┌─────────┐ ");
    }
    printf("\n");

    // TOP RANK
    for (int i = 0; i < count; i++) {
        printf("│ %-2s      │ ",
               ranks[hand[i].rank]);
    }
    printf("\n");

    // EMPTY ROW
    for (int i = 0; i < count; i++) {
        printf("│         │ ");
    }
    printf("\n");

    // SUIT ROW
    for (int i = 0; i < count; i++) {
        printf("│    %s    │ ",
               suits[hand[i].suit]);
    }
    printf("\n");

    // EMPTY ROW
    for (int i = 0; i < count; i++) {
        printf("│         │ ");
    }
    printf("\n");

    // BOTTOM RANK
    for (int i = 0; i < count; i++) {
        printf("│      %-2s │ ",
               ranks[hand[i].rank]);
    }
    printf("\n");

    // BOTTOM BORDER
    for (int i = 0; i < count; i++) {
        printf("└─────────┘ ");
    }
    printf("\n");
}

void render(Deck *deck, Card hand[], int handCount) {

    clearScreen();

    printf("=====================================\n");
    printf("            CARD TABLE              \n");
    printf("=====================================\n\n");

    printf("Cards Remaining: %d\n\n",
           DECK_SIZE - deck->top);

    printf("YOUR HAND:\n\n");

    if (handCount > 0) {
        printHand(hand, handCount);
    } else {
        printf("No cards drawn yet.\n");
    }

    printf("\n-------------------------------------\n");
    printf("[ENTER] Draw Card\n");
    printf("[q] Quit\n");
    printf("-------------------------------------\n");
}

int main() {

    Deck deck;

    initDeck(&deck);
    shuffleDeck(&deck);

    // HAND ARRAY
    Card hand[8];

    // HOW MANY CARDS CURRENTLY IN HAND
    int handCount = 0;

    char input;

    while (1) {

        render(&deck, hand, handCount);

        input = getchar();

        // quit
        if (input == 'q') {
            break;
        }

        // draw card if:
        // 1. hand not full
        // 2. deck still has cards
        if (handCount < 8 &&
            deck.top < DECK_SIZE) {

            hand[handCount] = drawCard(&deck);

            handCount++;
        }

        // flush newline
        while (getchar() != '\n');
    }

    return 0;
}
