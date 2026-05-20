#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "OldMaid.h"

static void remove_card_from_hand(OldMaidPlayer *player, int index) {
    for (int i = index; i < player->handSize - 1; i++) {
        player->hand[i] = player->hand[i + 1];
    }

    player->handSize--;
}

static void add_card_to_hand(OldMaidPlayer *player, Card card) {
    player->hand[player->handSize++] = card;
}

static void print_card(Card card) {
    const char *suits[] = {"H", "D", "C", "S"};

    const char *ranks[] = {
        "", "", "2", "3", "4", "5", "6",
        "7", "8", "9", "10",
        "J", "Q", "K", "A"
    };

    printf("[%s%s]", ranks[card.rank], suits[card.suit]);
}

int oldmaid_init_game(OldMaidGame *game, int playerCount) {
    if (playerCount < OLDMAID_MIN_PLAYERS ||
        playerCount > OLDMAID_MAX_PLAYERS) {
        return 0;
    }

    memset(game, 0, sizeof(OldMaidGame));

    game->playerCount = playerCount;
    game->state = OLDMAID_WAITING;

    for (int i = 0; i < playerCount; i++) {
        game->players[i].seat = i;
        game->players[i].active = 1;

        sprintf(game->players[i].name,
                "Player %d",
                i + 1);
    }

    return 1;
}

static void remove_queen_of_clubs(Deck *deck) {
    for (int i = 0; i < DECK_SIZE; i++) {

        Card card = deck->cards[i];

        if (card.rank == QUEEN &&
            card.suit == CLUBS) {

            for (int j = i; j < DECK_SIZE - 1; j++) {
                deck->cards[j] = deck->cards[j + 1];
            }

            deck->top--;
            return;
        }
    }
}

void oldmaid_remove_pairs(OldMaidPlayer *player) {

    int removed;

    do {

        removed = 0;

        for (int i = 0; i < player->handSize; i++) {

            for (int j = i + 1; j < player->handSize; j++) {

                if (player->hand[i].rank ==
                    player->hand[j].rank) {

                    remove_card_from_hand(player, j);
                    remove_card_from_hand(player, i);

                    removed = 1;
                    break;
                }
            }

            if (removed) break;
        }

    } while (removed);
}

void oldmaid_start_game(OldMaidGame *game) {

    initDeck(&game->deck);
    shuffleDeck(&game->deck);

    remove_queen_of_clubs(&game->deck);

    int currentPlayer = 0;

    while (game->deck.top > 0) {

        add_card_to_hand(
            &game->players[currentPlayer],
            drawCard(&game->deck)
        );

        currentPlayer++;
        currentPlayer %= game->playerCount;
    }

    for (int i = 0; i < game->playerCount; i++) {
        oldmaid_remove_pairs(&game->players[i]);
    }

    game->currentTurn = 0;
    game->state = OLDMAID_PLAYING;
}

int oldmaid_next_active_player(
    OldMaidGame *game,
    int currentSeat
) {

    for (int i = 1; i <= game->playerCount; i++) {

        int seat =
            (currentSeat + i) % game->playerCount;

        if (game->players[seat].active &&
            !game->players[seat].eliminated) {

            return seat;
        }
    }

    return -1;
}

void oldmaid_draw_from_player(
    OldMaidGame *game,
    int drawingSeat,
    int targetSeat,
    int targetCardIndex
) {

    OldMaidPlayer *drawingPlayer =
        &game->players[drawingSeat];

    OldMaidPlayer *targetPlayer =
        &game->players[targetSeat];

    if (targetCardIndex < 0 ||
        targetCardIndex >= targetPlayer->handSize) {
        return;
    }

    Card drawn =
        targetPlayer->hand[targetCardIndex];

    add_card_to_hand(drawingPlayer, drawn);

    remove_card_from_hand(
        targetPlayer,
        targetCardIndex
    );

    oldmaid_remove_pairs(drawingPlayer);
}

int oldmaid_remaining_players(OldMaidGame *game) {

    int count = 0;

    for (int i = 0; i < game->playerCount; i++) {

        if (!game->players[i].eliminated) {
            count++;
        }
    }

    return count;
}

void oldmaid_check_elimination(OldMaidGame *game) {

    for (int i = 0; i < game->playerCount; i++) {

        OldMaidPlayer *player =
            &game->players[i];

        if (!player->eliminated &&
            player->handSize == 0) {

            player->eliminated = 1;

            printf("%s is out!\n",
                   player->name);
        }
    }

    if (oldmaid_remaining_players(game) == 1) {

        for (int i = 0; i < game->playerCount; i++) {

            if (!game->players[i].eliminated) {

                game->loserSeat = i;
                game->state = OLDMAID_GAME_OVER;

                break;
            }
        }
    }
}

void oldmaid_print_state(OldMaidGame *game) {

    printf("\n========== OLD MAID ==========\n");

    for (int i = 0; i < game->playerCount; i++) {

        OldMaidPlayer *player =
            &game->players[i];

        printf("%s | Cards: %d",
               player->name,
               player->handSize);

        if (player->eliminated) {
            printf(" | OUT");
        }

        if (i == game->currentTurn) {
            printf(" | TURN");
        }

        printf("\n");
    }

    printf("==============================\n");
}

void oldmaid_play_turn(
    OldMaidGame *game,
    int drawingSeat,
    int targetCardIndex
) {

    if (game->state != OLDMAID_PLAYING) {
        return;
    }

    OldMaidPlayer *drawingPlayer =
        &game->players[drawingSeat];

    if (drawingPlayer->eliminated) {
        return;
    }

    int targetSeat =
        oldmaid_next_active_player(
            game,
            drawingSeat
        );

    if (targetSeat == -1) {
        return;
    }

    oldmaid_draw_from_player(
        game,
        drawingSeat,
        targetSeat,
        targetCardIndex
    );

    oldmaid_check_elimination(game);

    if (game->state == OLDMAID_GAME_OVER) {
        return;
    }

    game->currentTurn =
        oldmaid_next_active_player(
            game,
            drawingSeat
        );
}

void oldmaid_start(void) {

    OldMaidGame game;

    int playerCount;

    printf("Enter player count (2-10): ");
    scanf("%d", &playerCount);

    if (!oldmaid_init_game(
            &game,
            playerCount)) {

        printf("Invalid player count.\n");
        return;
    }

    oldmaid_start_game(&game);

    while (game.state != OLDMAID_GAME_OVER) {

        oldmaid_print_state(&game);

        OldMaidPlayer *player =
            &game.players[game.currentTurn];

        int targetSeat =
            oldmaid_next_active_player(
                &game,
                game.currentTurn
            );

        OldMaidPlayer *target =
            &game.players[targetSeat];

        printf("\n%s's turn.\n",
               player->name);

        printf("Target player: %s\n",
               target->name);

        printf("Target has %d cards.\n",
               target->handSize);

        printf("Choose a card index (0-%d): ",
               target->handSize - 1);

        int index;
        scanf("%d", &index);

        oldmaid_play_turn(
            &game,
            game.currentTurn,
            index
        );
    }

    printf("\n=====================\n");
    printf("OLD MAID LOSER:\n");
    printf("%s\n",
           game.players[game.loserSeat].name);
    printf("=====================\n");
}