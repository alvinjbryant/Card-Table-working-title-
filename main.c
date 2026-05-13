#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Deck.h"

void clearScreen() {
    system("clear"); // use "cls" on Windows
}

void printSpaces(int count) {

    for (int i = 0; i < count; i++) {
        printf(" ");
    }
}

const char* getSuitColor(Suit suit) {

    if (suit == HEARTS ||
        suit == DIAMONDS) {

        return "\033[31m"; // red
    }

    return "\033[37m"; // white
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
       printf("│ %s%-2s\033[0m      │ ",
       getSuitColor(hand[i].suit),
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
       printf("│    %s%s\033[0m    │ ",
       getSuitColor(hand[i].suit),
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
        printf("│      %s%-2s\033[0m │ ",
       getSuitColor(hand[i].suit),
       ranks[hand[i].rank]);
    }
    printf("\n");

    // BOTTOM BORDER
    for (int i = 0; i < count; i++) {
        printf("└─────────┘ ");
    }
    printf("\n");
}

void animateDraw(Deck *deck, Card hand[], int handCount) {

    for (int frame = 0; frame < 4; frame++) {

        clearScreen();

        printf("=====================================\n");
        printf("            CARD TABLE              \n");
        printf("=====================================\n\n");

        printf("Cards Remaining: %d\n\n",
               DECK_SIZE - deck->top);

        printf("DRAWING CARD");

        // fake animation dots
        for (int i = 0; i < frame; i++) {
            printf(".");
        }

        printf("\n\n");

        // render existing hand
        if (handCount > 0) {
            printHand(hand, handCount);
        }

        fflush(stdout);

        // animation speed

        usleep(150000);

        // Windows:
        // Sleep(150);
    }
}

void animateDrawSlide(Deck *deck,
                      Card hand[],
                      int handCount) {

    const char *suits[] = {"♥", "♦", "♣", "♠"};
    const char *ranks[] = {
        "", "", "2","3","4","5","6","7","8","9",
        "10","J","Q","K","A"
    };

    // card being animated
    Card nextCard = deck->cards[deck->top];

    // slide frames
    for (int offset = 0;
         offset <= handCount * 12;
         offset += 3) {

        clearScreen();

        printf("=====================================\n");
        printf("            CARD TABLE              \n");
        printf("=====================================\n\n");

        printf("Cards Remaining: %d\n\n",
               DECK_SIZE - deck->top);

        printf("YOUR HAND:\n\n");

        // existing hand
        if (handCount > 0) {
            printHand(hand, handCount);
        }

        printf("\n");

        // animated moving card

        printSpaces(offset);
        printf("┌─────────┐\n");

        printSpaces(offset);
        printf("│ %-2s      │\n",
               ranks[nextCard.rank]);

        printSpaces(offset);
        printf("│         │\n");

        printSpaces(offset);
        printf("│    %s    │\n",
               suits[nextCard.suit]);

        printSpaces(offset);
        printf("│         │\n");

        printSpaces(offset);
        printf("│      %-2s │\n",
               ranks[nextCard.rank]);

        printSpaces(offset);
        printf("└─────────┘\n");

        fflush(stdout);

        usleep(50000);
    }
}


void render(Deck *deck, Card hand[], int handCount) {

    clearScreen();

    printf("=====================================\n");
    printf("            CARD TABLE              \n");
    printf("=====================================\n\n");

    printf("Cards Remaining: %d\n\n",
           DECK_SIZE - deck->top);

    printf("YOUR HAND:\n\n");

    if (handCount > 0) { printHand(hand, handCount);
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

Player players[4];

snprintf(players[0].name,
         sizeof(players[0].name),
         "Player 1");

players[0].hand.count = 0;
players[0].score = 0;


    char input;

    while (1) {

        render(&deck, players[0].hand.cards, players[0].hand.count);

        input = getchar();

        // quit
        if (input == 'q') {
            break;
        }

        // draw card if:
        // 1. hand not full
        // 2. deck still has cards
        if (players[0].hand.count < 8 &&deck.top < DECK_SIZE) 
        {

            animateDrawSlide( &deck, players[0].hand.cards, players[0].hand.count);

            Player *player = &players[0];

            player->hand.cards[player->hand.count]
                 = drawCard(&deck);

            player->hand.count++;
        }

        // flush newline
        while (getchar() != '\n');
    }

    return 0;
}
