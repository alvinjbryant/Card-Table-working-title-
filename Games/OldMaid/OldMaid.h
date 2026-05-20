#ifndef OLD_MAID_H
#define OLD_MAID_H

#include "../../Deck.h"

#define OLDMAID_MIN_PLAYERS 2
#define OLDMAID_MAX_PLAYERS 10
#define OLDMAID_MAX_NAME 32
#define OLDMAID_MAX_HAND 52

typedef enum {
    OLDMAID_WAITING,
    OLDMAID_PLAYING,
    OLDMAID_GAME_OVER
} OldMaidState;

typedef struct {
    int seat;
    char name[OLDMAID_MAX_NAME];

    Card hand[OLDMAID_MAX_HAND];
    int handSize;

    int active;
    int eliminated;
} OldMaidPlayer;

typedef struct {
    Deck deck;

    OldMaidPlayer players[OLDMAID_MAX_PLAYERS];
    int playerCount;

    int currentTurn;
    int loserSeat;

    OldMaidState state;
} OldMaidGame;

int oldmaid_init_game(OldMaidGame *game, int playerCount);

void oldmaid_start_game(OldMaidGame *game);

void oldmaid_remove_pairs(OldMaidPlayer *player);

void oldmaid_draw_from_player(
    OldMaidGame *game,
    int drawingSeat,
    int targetSeat,
    int targetCardIndex
);

int oldmaid_next_active_player(
    OldMaidGame *game,
    int currentSeat
);

int oldmaid_remaining_players(OldMaidGame *game);

void oldmaid_check_elimination(OldMaidGame *game);

void oldmaid_print_state(OldMaidGame *game);

void oldmaid_play_turn(
    OldMaidGame *game,
    int drawingSeat,
    int targetCardIndex
);

void oldmaid_start(void);

#endif