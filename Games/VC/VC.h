#ifndef VC_H
#define VC_H

#include "../../Deck.h"

#define VC_MIN_PLAYERS 2
#define VC_MAX_PLAYERS 4
#define VC_MAX_HAND 13
#define VC_MAX_NAME 32
#define VC_MAX_COMBO 13

typedef enum {
    VC_WAITING,
    VC_PLAYING,
    VC_ROUND_RESET,
    VC_GAME_OVER
} VCState;

typedef enum {
    VC_COMBO_INVALID,
    VC_COMBO_SINGLE,
    VC_COMBO_PAIR,
    VC_COMBO_TRIPLE,
    VC_COMBO_FOUR_KIND,
    VC_COMBO_STRAIGHT
} VCComboType;

typedef struct {
    VCComboType type;
    Card cards[VC_MAX_COMBO];
    int count;
    int powerRank;
    int powerSuit;
} VCCombo;

typedef struct {
    int seat;
    char name[VC_MAX_NAME];

    Card hand[VC_MAX_HAND];
    int handSize;

    int active;
    int passed;
    int finished;
} VCPlayer;

typedef struct {
    Deck deck;

    VCPlayer players[VC_MAX_PLAYERS];
    int playerCount;

    int currentTurn;
    int lastPlayedSeat;

    VCCombo currentCombo;

    VCState state;
    int winnerSeat;
} VCGame;

int vc_init_game(VCGame *game, int playerCount);
void vc_start_game(VCGame *game);

int vc_play_cards(VCGame *game, int seat, int cardIndexes[], int count);
void vc_pass_turn(VCGame *game, int seat);

VCCombo vc_evaluate_combo(Card cards[], int count);
int vc_combo_beats(VCCombo challenger, VCCombo current);

int vc_next_active_player(VCGame *game, int currentSeat);
int vc_remaining_active_players(VCGame *game);

const char *vc_combo_name(VCComboType type);

void vc_start(void);

#endif