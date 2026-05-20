#ifndef TEXAS_HOLDEM_H
#define TEXAS_HOLDEM_H

#include "../../Deck.h"

#define HOLDEM_MIN_PLAYERS 2
#define HOLDEM_MAX_PLAYERS 10
#define HOLDEM_HOLE_CARDS 2
#define HOLDEM_COMMUNITY_CARDS 5
#define HOLDEM_MAX_NAME 32

typedef enum {
    HOLDEM_PREFLOP,
    HOLDEM_FLOP,
    HOLDEM_TURN,
    HOLDEM_RIVER,
    HOLDEM_SHOWDOWN,
    HOLDEM_HAND_OVER
} HoldemStreet;

typedef enum {
    ACTION_FOLD,
    ACTION_CHECK,
    ACTION_CALL,
    ACTION_BET,
    ACTION_RAISE
} HoldemActionType;

typedef enum {
    HIGH_CARD = 1,
    ONE_PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    STRAIGHT_FLUSH
} HoldemHandRank;

typedef struct {
    HoldemHandRank rank;
    int tiebreakers[5];
} HoldemHandValue;

typedef struct {
    int seat;
    char name[HOLDEM_MAX_NAME];

    Card hole[HOLDEM_HOLE_CARDS];

    int chips;
    int currentBet;
    int totalCommitted;

    int folded;
    int allIn;
    int active;
} HoldemPlayer;

typedef struct {
    Deck deck;

    HoldemPlayer players[HOLDEM_MAX_PLAYERS];
    int playerCount;

    Card community[HOLDEM_COMMUNITY_CARDS];
    int communityCount;

    int dealerButton;
    int smallBlindSeat;
    int bigBlindSeat;
    int currentTurn;

    int smallBlind;
    int bigBlind;
    int currentBet;
    int minimumRaise;
    int pot;

    HoldemStreet street;
    int handNumber;
} TexasHoldemGame;

void holdem_init_game(TexasHoldemGame *game, int playerCount, int startingChips, int smallBlind, int bigBlind);
void holdem_start_new_hand(TexasHoldemGame *game);

int holdem_apply_action(TexasHoldemGame *game, int playerSeat, HoldemActionType action, int amount);

void holdem_advance_street(TexasHoldemGame *game);
void holdem_showdown(TexasHoldemGame *game);

HoldemHandValue holdem_evaluate_player(TexasHoldemGame *game, int playerSeat);
int holdem_compare_hands(HoldemHandValue a, HoldemHandValue b);

const char *holdem_hand_rank_name(HoldemHandRank rank);
const char *holdem_street_name(HoldemStreet street);

void texas_holdem_start(void);

#endif