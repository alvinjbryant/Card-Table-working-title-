#include <stdio.h>
#include <string.h>
#include "TexasHoldem.h"

#define EVAL_CARDS 7

static const char *handRankNames[] = {
    "",
    "High Card",
    "One Pair",
    "Two Pair",
    "Three of a Kind",
    "Straight",
    "Flush",
    "Full House",
    "Four of a Kind",
    "Straight Flush"
};

const char *holdem_hand_rank_name(HoldemHandRank rank) {
    return handRankNames[rank];
}

const char *holdem_street_name(HoldemStreet street) {
    switch (street) {
        case HOLDEM_PREFLOP: return "Pre-Flop";
        case HOLDEM_FLOP: return "Flop";
        case HOLDEM_TURN: return "Turn";
        case HOLDEM_RIVER: return "River";
        case HOLDEM_SHOWDOWN: return "Showdown";
        case HOLDEM_HAND_OVER: return "Hand Over";
        default: return "Unknown";
    }
}

static int next_active_seat(TexasHoldemGame *game, int startSeat) {
    for (int i = 1; i <= game->playerCount; i++) {
        int seat = (startSeat + i) % game->playerCount;

        if (game->players[seat].active &&
            !game->players[seat].folded &&
            !game->players[seat].allIn) {
            return seat;
        }
    }

    return -1;
}

static int active_players_remaining(TexasHoldemGame *game) {
    int count = 0;

    for (int i = 0; i < game->playerCount; i++) {
        if (game->players[i].active && !game->players[i].folded) {
            count++;
        }
    }

    return count;
}

static void commit_chips(TexasHoldemGame *game, int seat, int amount) {
    HoldemPlayer *player = &game->players[seat];

    if (amount >= player->chips) {
        amount = player->chips;
        player->allIn = 1;
    }

    player->chips -= amount;
    player->currentBet += amount;
    player->totalCommitted += amount;
    game->pot += amount;
}

static void post_blind(TexasHoldemGame *game, int seat, int amount) {
    commit_chips(game, seat, amount);

    if (game->players[seat].currentBet > game->currentBet) {
        game->currentBet = game->players[seat].currentBet;
    }
}

void holdem_init_game(TexasHoldemGame *game, int playerCount, int startingChips, int smallBlind, int bigBlind) {
    if (playerCount < HOLDEM_MIN_PLAYERS) playerCount = HOLDEM_MIN_PLAYERS;
    if (playerCount > HOLDEM_MAX_PLAYERS) playerCount = HOLDEM_MAX_PLAYERS;

    memset(game, 0, sizeof(TexasHoldemGame));

    game->playerCount = playerCount;
    game->smallBlind = smallBlind;
    game->bigBlind = bigBlind;
    game->minimumRaise = bigBlind;
    game->dealerButton = 0;
    game->street = HOLDEM_HAND_OVER;

    for (int i = 0; i < playerCount; i++) {
        game->players[i].seat = i;
        sprintf(game->players[i].name, "Player %d", i + 1);
        game->players[i].chips = startingChips;
        game->players[i].active = 1;
    }
}

void holdem_start_new_hand(TexasHoldemGame *game) {
    initDeck(&game->deck);
    shuffleDeck(&game->deck);

    game->communityCount = 0;
    game->pot = 0;
    game->currentBet = 0;
    game->minimumRaise = game->bigBlind;
    game->street = HOLDEM_PREFLOP;
    game->handNumber++;

    for (int i = 0; i < game->playerCount; i++) {
        game->players[i].currentBet = 0;
        game->players[i].totalCommitted = 0;
        game->players[i].folded = 0;
        game->players[i].allIn = 0;

        if (game->players[i].chips <= 0) {
            game->players[i].active = 0;
        }
    }

    game->dealerButton = next_active_seat(game, game->dealerButton);

    if (game->playerCount == 2) {
        game->smallBlindSeat = game->dealerButton;
        game->bigBlindSeat = next_active_seat(game, game->smallBlindSeat);
    } else {
        game->smallBlindSeat = next_active_seat(game, game->dealerButton);
        game->bigBlindSeat = next_active_seat(game, game->smallBlindSeat);
    }

    post_blind(game, game->smallBlindSeat, game->smallBlind);
    post_blind(game, game->bigBlindSeat, game->bigBlind);

    for (int card = 0; card < HOLDEM_HOLE_CARDS; card++) {
        for (int seat = 0; seat < game->playerCount; seat++) {
            if (game->players[seat].active) {
                game->players[seat].hole[card] = drawCard(&game->deck);
            }
        }
    }

    game->currentTurn = next_active_seat(game, game->bigBlindSeat);
}

int holdem_apply_action(TexasHoldemGame *game, int playerSeat, HoldemActionType action, int amount) {
    if (game->street == HOLDEM_SHOWDOWN || game->street == HOLDEM_HAND_OVER) {
        return 0;
    }

    if (playerSeat != game->currentTurn) {
        return 0;
    }

    HoldemPlayer *player = &game->players[playerSeat];

    if (!player->active || player->folded || player->allIn) {
        return 0;
    }

    int callAmount = game->currentBet - player->currentBet;

    switch (action) {
        case ACTION_FOLD:
            player->folded = 1;
            break;

        case ACTION_CHECK:
            if (callAmount != 0) return 0;
            break;

        case ACTION_CALL:
            if (callAmount <= 0) return 0;
            commit_chips(game, playerSeat, callAmount);
            break;

        case ACTION_BET:
            if (game->currentBet != 0) return 0;
            if (amount < game->bigBlind) return 0;

            commit_chips(game, playerSeat, amount);
            game->currentBet = player->currentBet;
            game->minimumRaise = amount;
            break;

        case ACTION_RAISE:
            if (game->currentBet == 0) return 0;
            if (amount < callAmount + game->minimumRaise) return 0;

            commit_chips(game, playerSeat, amount);

            {
                int raiseSize = player->currentBet - game->currentBet;
                game->currentBet = player->currentBet;
                game->minimumRaise = raiseSize;
            }
            break;

        default:
            return 0;
    }

    if (active_players_remaining(game) == 1) {
        game->street = HOLDEM_HAND_OVER;
        return 1;
    }

    game->currentTurn = next_active_seat(game, playerSeat);

    return 1;
}

static void reset_round_bets(TexasHoldemGame *game) {
    for (int i = 0; i < game->playerCount; i++) {
        game->players[i].currentBet = 0;
    }

    game->currentBet = 0;
    game->minimumRaise = game->bigBlind;
}

static void burn_card(TexasHoldemGame *game) {
    drawCard(&game->deck);
}

void holdem_advance_street(TexasHoldemGame *game) {
    reset_round_bets(game);

    if (game->street == HOLDEM_PREFLOP) {
        burn_card(game);

        game->community[0] = drawCard(&game->deck);
        game->community[1] = drawCard(&game->deck);
        game->community[2] = drawCard(&game->deck);

        game->communityCount = 3;
        game->street = HOLDEM_FLOP;
    } else if (game->street == HOLDEM_FLOP) {
        burn_card(game);

        game->community[3] = drawCard(&game->deck);
        game->communityCount = 4;
        game->street = HOLDEM_TURN;
    } else if (game->street == HOLDEM_TURN) {
        burn_card(game);

        game->community[4] = drawCard(&game->deck);
        game->communityCount = 5;
        game->street = HOLDEM_RIVER;
    } else if (game->street == HOLDEM_RIVER) {
        game->street = HOLDEM_SHOWDOWN;
    }

    game->currentTurn = next_active_seat(game, game->dealerButton);
}

int holdem_compare_hands(HoldemHandValue a, HoldemHandValue b) {
    if (a.rank > b.rank) return 1;
    if (a.rank < b.rank) return -1;

    for (int i = 0; i < 5; i++) {
        if (a.tiebreakers[i] > b.tiebreakers[i]) return 1;
        if (a.tiebreakers[i] < b.tiebreakers[i]) return -1;
    }

    return 0;
}

static int find_straight_high_card(int rankCounts[]) {
    for (int high = ACE; high >= SIX; high--) {
        if (rankCounts[high] &&
            rankCounts[high - 1] &&
            rankCounts[high - 2] &&
            rankCounts[high - 3] &&
            rankCounts[high - 4]) {
            return high;
        }
    }

    if (rankCounts[ACE] &&
        rankCounts[FIVE] &&
        rankCounts[FOUR] &&
        rankCounts[THREE] &&
        rankCounts[TWO]) {
        return FIVE;
    }

    return 0;
}

static HoldemHandValue evaluate_seven_cards(Card cards[]) {
    HoldemHandValue value;
    value.rank = HIGH_CARD;

    for (int i = 0; i < 5; i++) {
        value.tiebreakers[i] = 0;
    }

    int rankCounts[15] = {0};
    int suitCounts[4] = {0};

    for (int i = 0; i < EVAL_CARDS; i++) {
        rankCounts[cards[i].rank]++;
        suitCounts[cards[i].suit]++;
    }

    int flushSuit = -1;

    for (int suit = HEARTS; suit <= SPADES; suit++) {
        if (suitCounts[suit] >= 5) {
            flushSuit = suit;
            break;
        }
    }

    if (flushSuit != -1) {
        int flushRankCounts[15] = {0};

        for (int i = 0; i < EVAL_CARDS; i++) {
            if (cards[i].suit == flushSuit) {
                flushRankCounts[cards[i].rank]++;
            }
        }

        int high = find_straight_high_card(flushRankCounts);

        if (high) {
            value.rank = STRAIGHT_FLUSH;
            value.tiebreakers[0] = high;
            return value;
        }
    }

    for (int rank = ACE; rank >= TWO; rank--) {
        if (rankCounts[rank] == 4) {
            value.rank = FOUR_OF_A_KIND;
            value.tiebreakers[0] = rank;

            for (int kicker = ACE; kicker >= TWO; kicker--) {
                if (kicker != rank && rankCounts[kicker]) {
                    value.tiebreakers[1] = kicker;
                    return value;
                }
            }
        }
    }

    int triple = 0;
    int pair = 0;

    for (int rank = ACE; rank >= TWO; rank--) {
        if (rankCounts[rank] >= 3) {
            if (!triple) triple = rank;
            else if (!pair) pair = rank;
        } else if (rankCounts[rank] >= 2 && !pair) {
            pair = rank;
        }
    }

    if (triple && pair) {
        value.rank = FULL_HOUSE;
        value.tiebreakers[0] = triple;
        value.tiebreakers[1] = pair;
        return value;
    }

    if (flushSuit != -1) {
        value.rank = FLUSH;

        int index = 0;

        for (int rank = ACE; rank >= TWO && index < 5; rank--) {
            for (int i = 0; i < EVAL_CARDS; i++) {
                if (cards[i].suit == flushSuit && cards[i].rank == rank) {
                    value.tiebreakers[index++] = rank;
                    break;
                }
            }
        }

        return value;
    }

    int straightHigh = find_straight_high_card(rankCounts);

    if (straightHigh) {
        value.rank = STRAIGHT;
        value.tiebreakers[0] = straightHigh;
        return value;
    }

    for (int rank = ACE; rank >= TWO; rank--) {
        if (rankCounts[rank] == 3) {
            value.rank = THREE_OF_A_KIND;
            value.tiebreakers[0] = rank;

            int index = 1;

            for (int kicker = ACE; kicker >= TWO && index < 3; kicker--) {
                if (kicker != rank && rankCounts[kicker]) {
                    value.tiebreakers[index++] = kicker;
                }
            }

            return value;
        }
    }

    int pairs[2] = {0};
    int pairCount = 0;

    for (int rank = ACE; rank >= TWO; rank--) {
        if (rankCounts[rank] >= 2 && pairCount < 2) {
            pairs[pairCount++] = rank;
        }
    }

    if (pairCount == 2) {
        value.rank = TWO_PAIR;
        value.tiebreakers[0] = pairs[0];
        value.tiebreakers[1] = pairs[1];

        for (int kicker = ACE; kicker >= TWO; kicker--) {
            if (kicker != pairs[0] && kicker != pairs[1] && rankCounts[kicker]) {
                value.tiebreakers[2] = kicker;
                return value;
            }
        }
    }

    if (pairCount == 1) {
        value.rank = ONE_PAIR;
        value.tiebreakers[0] = pairs[0];

        int index = 1;

        for (int kicker = ACE; kicker >= TWO && index < 4; kicker--) {
            if (kicker != pairs[0] && rankCounts[kicker]) {
                value.tiebreakers[index++] = kicker;
            }
        }

        return value;
    }

    value.rank = HIGH_CARD;

    int index = 0;

    for (int rank = ACE; rank >= TWO && index < 5; rank--) {
        if (rankCounts[rank]) {
            value.tiebreakers[index++] = rank;
        }
    }

    return value;
}

HoldemHandValue holdem_evaluate_player(TexasHoldemGame *game, int playerSeat) {
    Card seven[EVAL_CARDS];

    seven[0] = game->players[playerSeat].hole[0];
    seven[1] = game->players[playerSeat].hole[1];

    for (int i = 0; i < HOLDEM_COMMUNITY_CARDS; i++) {
        seven[i + 2] = game->community[i];
    }

    return evaluate_seven_cards(seven);
}

void holdem_showdown(TexasHoldemGame *game) {
    HoldemHandValue best;
    int hasBest = 0;

    int winners[HOLDEM_MAX_PLAYERS];
    int winnerCount = 0;

    for (int i = 0; i < game->playerCount; i++) {
        if (!game->players[i].active || game->players[i].folded) {
            continue;
        }

        HoldemHandValue value = holdem_evaluate_player(game, i);

        if (!hasBest || holdem_compare_hands(value, best) > 0) {
            best = value;
            winners[0] = i;
            winnerCount = 1;
            hasBest = 1;
        } else if (holdem_compare_hands(value, best) == 0) {
            winners[winnerCount++] = i;
        }
    }

    if (winnerCount == 0) return;

    int share = game->pot / winnerCount;

    for (int i = 0; i < winnerCount; i++) {
        game->players[winners[i]].chips += share;
    }

    game->pot = 0;
    game->street = HOLDEM_HAND_OVER;
}

static void print_card(Card card) {
    const char *suits[] = {"H", "D", "C", "S"};
    const char *ranks[] = {
        "", "", "2", "3", "4", "5", "6", "7", "8", "9",
        "10", "J", "Q", "K", "A"
    };

    printf("[%s%s]", ranks[card.rank], suits[card.suit]);
}

static void print_public_state(TexasHoldemGame *game) {
    printf("\n====================================\n");
    printf("Street: %s\n", holdem_street_name(game->street));
    printf("Pot: %d\n", game->pot);
    printf("Current bet: %d\n", game->currentBet);

    printf("Board: ");
    for (int i = 0; i < game->communityCount; i++) {
        print_card(game->community[i]);
        printf(" ");
    }
    printf("\n");

    printf("Turn: %s\n", game->players[game->currentTurn].name);
    printf("====================================\n");
}

static void print_player_private(HoldemPlayer *player) {
    printf("%s | Chips: %d | Hole: ", player->name, player->chips);
    print_card(player->hole[0]);
    printf(" ");
    print_card(player->hole[1]);
    printf("\n");
}

void texas_holdem_start(void) {
    TexasHoldemGame game;
    int playerCount;

    printf("Enter number of players 2-10: ");
    scanf("%d", &playerCount);

    holdem_init_game(&game, playerCount, 1000, 5, 10);
    holdem_start_new_hand(&game);

    while (game.street != HOLDEM_HAND_OVER) {
        if (game.street == HOLDEM_SHOWDOWN) {
            holdem_showdown(&game);
            break;
        }

        print_public_state(&game);
        print_player_private(&game.players[game.currentTurn]);

        printf("\nChoose action:\n");
        printf("1. Fold\n");
        printf("2. Check\n");
        printf("3. Call\n");
        printf("4. Bet\n");
        printf("5. Raise\n");
        printf("6. Advance street manually\n");
        printf("> ");

        int choice;
        int amount = 0;
        scanf("%d", &choice);

        if (choice == 6) {
            holdem_advance_street(&game);
            continue;
        }

        HoldemActionType action;

        switch (choice) {
            case 1:
                action = ACTION_FOLD;
                break;
            case 2:
                action = ACTION_CHECK;
                break;
            case 3:
                action = ACTION_CALL;
                break;
            case 4:
                action = ACTION_BET;
                printf("Bet amount: ");
                scanf("%d", &amount);
                break;
            case 5:
                action = ACTION_RAISE;
                printf("Total amount to put in this action: ");
                scanf("%d", &amount);
                break;
            default:
                printf("Invalid choice.\n");
                continue;
        }

        if (!holdem_apply_action(&game, game.currentTurn, action, amount)) {
            printf("Illegal action.\n");
        }

        if (active_players_remaining(&game) == 1) {
            for (int i = 0; i < game.playerCount; i++) {
                if (game.players[i].active && !game.players[i].folded) {
                    game.players[i].chips += game.pot;
                    game.pot = 0;
                    printf("\n%s wins because everyone else folded.\n", game.players[i].name);
                    break;
                }
            }

            game.street = HOLDEM_HAND_OVER;
        }
    }

    if (game.street == HOLDEM_HAND_OVER) {
        printf("\nFinal chip counts:\n");

        for (int i = 0; i < game.playerCount; i++) {
            printf("%s: %d chips\n", game.players[i].name, game.players[i].chips);
        }
    }
}