#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "VC.h"

static int vc_rank_value(Rank rank) {
    if (rank == THREE) return 3;
    if (rank == FOUR) return 4;
    if (rank == FIVE) return 5;
    if (rank == SIX) return 6;
    if (rank == SEVEN) return 7;
    if (rank == EIGHT) return 8;
    if (rank == NINE) return 9;
    if (rank == TEN) return 10;
    if (rank == JACK) return 11;
    if (rank == QUEEN) return 12;
    if (rank == KING) return 13;
    if (rank == ACE) return 14;
    if (rank == TWO) return 15;

    return 0;
}

static int vc_suit_value(Suit suit) {
    if (suit == SPADES) return 1;
    if (suit == CLUBS) return 2;
    if (suit == DIAMONDS) return 3;
    if (suit == HEARTS) return 4;

    return 0;
}

const char *vc_combo_name(VCComboType type) {
    switch (type) {
        case VC_COMBO_SINGLE: return "Single";
        case VC_COMBO_PAIR: return "Pair";
        case VC_COMBO_TRIPLE: return "Triple";
        case VC_COMBO_FOUR_KIND: return "Four of a Kind";
        case VC_COMBO_STRAIGHT: return "Straight";
        default: return "Invalid";
    }
}

static void print_card(Card card) {
    const char *suits[] = {"H", "D", "C", "S"};

    const char *ranks[] = {
        "", "", "2", "3", "4", "5", "6", "7",
        "8", "9", "10", "J", "Q", "K", "A"
    };

    printf("[%s%s]", ranks[card.rank], suits[card.suit]);
}

static void sort_cards(Card cards[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            int rankA = vc_rank_value(cards[i].rank);
            int rankB = vc_rank_value(cards[j].rank);

            int suitA = vc_suit_value(cards[i].suit);
            int suitB = vc_suit_value(cards[j].suit);

            if (rankB < rankA ||
                (rankB == rankA && suitB < suitA)) {
                Card temp = cards[i];
                cards[i] = cards[j];
                cards[j] = temp;
            }
        }
    }
}

static void remove_card_from_hand(VCPlayer *player, int index) {
    for (int i = index; i < player->handSize - 1; i++) {
        player->hand[i] = player->hand[i + 1];
    }

    player->handSize--;
}

static int hand_contains_three_of_spades(VCPlayer *player) {
    for (int i = 0; i < player->handSize; i++) {
        if (player->hand[i].rank == THREE &&
            player->hand[i].suit == SPADES) {
            return 1;
        }
    }

    return 0;
}

int vc_init_game(VCGame *game, int playerCount) {
    if (playerCount < VC_MIN_PLAYERS ||
        playerCount > VC_MAX_PLAYERS) {
        return 0;
    }

    memset(game, 0, sizeof(VCGame));

    game->playerCount = playerCount;
    game->state = VC_WAITING;
    game->winnerSeat = -1;
    game->lastPlayedSeat = -1;
    game->currentCombo.type = VC_COMBO_INVALID;

    for (int i = 0; i < playerCount; i++) {
        game->players[i].seat = i;
        game->players[i].active = 1;

        sprintf(game->players[i].name, "Player %d", i + 1);
    }

    return 1;
}

void vc_start_game(VCGame *game) {
    initDeck(&game->deck);
    shuffleDeck(&game->deck);

    for (int i = 0; i < game->playerCount; i++) {
        game->players[i].handSize = 0;
        game->players[i].passed = 0;
        game->players[i].finished = 0;
        game->players[i].active = 1;
    }

    for (int card = 0; card < VC_MAX_HAND; card++) {
        for (int seat = 0; seat < game->playerCount; seat++) {
            game->players[seat].hand[game->players[seat].handSize++] =
                drawCard(&game->deck);
        }
    }

    for (int i = 0; i < game->playerCount; i++) {
        sort_cards(game->players[i].hand, game->players[i].handSize);

        if (hand_contains_three_of_spades(&game->players[i])) {
            game->currentTurn = i;
        }
    }

    game->state = VC_PLAYING;
}

VCCombo vc_evaluate_combo(Card cards[], int count) {
    VCCombo combo;
    memset(&combo, 0, sizeof(VCCombo));

    combo.type = VC_COMBO_INVALID;
    combo.count = count;

    if (count <= 0 || count > VC_MAX_COMBO) {
        return combo;
    }

    for (int i = 0; i < count; i++) {
        combo.cards[i] = cards[i];
    }

    sort_cards(combo.cards, count);

    int sameRank = 1;

    for (int i = 1; i < count; i++) {
        if (combo.cards[i].rank != combo.cards[0].rank) {
            sameRank = 0;
            break;
        }
    }

    if (count == 1) {
        combo.type = VC_COMBO_SINGLE;
        combo.powerRank = vc_rank_value(combo.cards[0].rank);
        combo.powerSuit = vc_suit_value(combo.cards[0].suit);
        return combo;
    }

    if (sameRank && count == 2) {
        combo.type = VC_COMBO_PAIR;
        combo.powerRank = vc_rank_value(combo.cards[1].rank);
        combo.powerSuit = vc_suit_value(combo.cards[1].suit);
        return combo;
    }

    if (sameRank && count == 3) {
        combo.type = VC_COMBO_TRIPLE;
        combo.powerRank = vc_rank_value(combo.cards[2].rank);
        combo.powerSuit = vc_suit_value(combo.cards[2].suit);
        return combo;
    }

    if (sameRank && count == 4) {
        combo.type = VC_COMBO_FOUR_KIND;
        combo.powerRank = vc_rank_value(combo.cards[3].rank);
        combo.powerSuit = vc_suit_value(combo.cards[3].suit);
        return combo;
    }

    if (count >= 3) {
        int straight = 1;

        for (int i = 0; i < count; i++) {
            if (combo.cards[i].rank == TWO) {
                straight = 0;
                break;
            }
        }

        for (int i = 1; i < count; i++) {
            int previous = vc_rank_value(combo.cards[i - 1].rank);
            int current = vc_rank_value(combo.cards[i].rank);

            if (current != previous + 1) {
                straight = 0;
                break;
            }
        }

        if (straight) {
            combo.type = VC_COMBO_STRAIGHT;
            combo.powerRank = vc_rank_value(combo.cards[count - 1].rank);
            combo.powerSuit = vc_suit_value(combo.cards[count - 1].suit);
            return combo;
        }
    }

    return combo;
}

int vc_combo_beats(VCCombo challenger, VCCombo current) {
    if (challenger.type == VC_COMBO_INVALID) {
        return 0;
    }

    if (current.type == VC_COMBO_INVALID) {
        return 1;
    }

    if (challenger.type == VC_COMBO_FOUR_KIND &&
        current.type == VC_COMBO_SINGLE &&
        current.powerRank == vc_rank_value(TWO)) {
        return 1;
    }

    if (challenger.type != current.type) {
        return 0;
    }

    if (challenger.count != current.count) {
        return 0;
    }

    if (challenger.powerRank > current.powerRank) {
        return 1;
    }

    if (challenger.powerRank == current.powerRank &&
        challenger.powerSuit > current.powerSuit) {
        return 1;
    }

    return 0;
}

int vc_next_active_player(VCGame *game, int currentSeat) {
    for (int i = 1; i <= game->playerCount; i++) {
        int seat = (currentSeat + i) % game->playerCount;

        if (game->players[seat].active &&
            !game->players[seat].finished &&
            !game->players[seat].passed) {
            return seat;
        }
    }

    return -1;
}

int vc_remaining_active_players(VCGame *game) {
    int count = 0;

    for (int i = 0; i < game->playerCount; i++) {
        if (!game->players[i].finished) {
            count++;
        }
    }

    return count;
}

static void reset_passes(VCGame *game) {
    for (int i = 0; i < game->playerCount; i++) {
        game->players[i].passed = 0;
    }
}

static void start_new_round(VCGame *game, int startingSeat) {
    game->currentCombo.type = VC_COMBO_INVALID;
    game->currentCombo.count = 0;
    game->lastPlayedSeat = -1;

    reset_passes(game);

    game->currentTurn = startingSeat;
    game->state = VC_PLAYING;
}

static int all_others_passed(VCGame *game) {
    int activeNotPassed = 0;

    for (int i = 0; i < game->playerCount; i++) {
        if (!game->players[i].finished &&
            !game->players[i].passed) {
            activeNotPassed++;
        }
    }

    return activeNotPassed <= 1;
}

int vc_play_cards(VCGame *game, int seat, int cardIndexes[], int count) {
    if (game->state != VC_PLAYING) {
        return 0;
    }

    if (seat != game->currentTurn) {
        return 0;
    }

    VCPlayer *player = &game->players[seat];

    if (player->finished || player->passed) {
        return 0;
    }

    if (count <= 0 || count > player->handSize) {
        return 0;
    }

    Card selected[VC_MAX_COMBO];

    for (int i = 0; i < count; i++) {
        if (cardIndexes[i] < 0 ||
            cardIndexes[i] >= player->handSize) {
            return 0;
        }

        for (int j = i + 1; j < count; j++) {
            if (cardIndexes[i] == cardIndexes[j]) {
                return 0;
            }
        }

        selected[i] = player->hand[cardIndexes[i]];
    }

    VCCombo combo = vc_evaluate_combo(selected, count);

    if (!vc_combo_beats(combo, game->currentCombo)) {
        return 0;
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (cardIndexes[j] > cardIndexes[i]) {
                int temp = cardIndexes[i];
                cardIndexes[i] = cardIndexes[j];
                cardIndexes[j] = temp;
            }
        }
    }

    for (int i = 0; i < count; i++) {
        remove_card_from_hand(player, cardIndexes[i]);
    }

    game->currentCombo = combo;
    game->lastPlayedSeat = seat;

    if (player->handSize == 0) {
        player->finished = 1;
        player->active = 0;

        if (game->winnerSeat == -1) {
            game->winnerSeat = seat;
            game->state = VC_GAME_OVER;
            return 1;
        }
    }

    reset_passes(game);

    int next = vc_next_active_player(game, seat);

    if (next == -1) {
        start_new_round(game, seat);
    } else {
        game->currentTurn = next;
    }

    return 1;
}

void vc_pass_turn(VCGame *game, int seat) {
    if (game->state != VC_PLAYING) {
        return;
    }

    if (seat != game->currentTurn) {
        return;
    }

    if (game->currentCombo.type == VC_COMBO_INVALID) {
        return;
    }

    game->players[seat].passed = 1;

    if (all_others_passed(game)) {
        start_new_round(game, game->lastPlayedSeat);
        return;
    }

    int next = vc_next_active_player(game, seat);

    if (next != -1) {
        game->currentTurn = next;
    }
}

static void print_hand(VCPlayer *player) {
    for (int i = 0; i < player->handSize; i++) {
        printf("%d:", i);
        print_card(player->hand[i]);
        printf(" ");
    }

    printf("\n");
}

static void print_state(VCGame *game) {
    printf("\n========== VC ==========\n");

    printf("Current turn: %s\n",
           game->players[game->currentTurn].name);

    printf("Current combo: %s\n",
           vc_combo_name(game->currentCombo.type));

    if (game->currentCombo.type != VC_COMBO_INVALID) {
        printf("Last played by: %s\n",
               game->players[game->lastPlayedSeat].name);
    }

    printf("\nPlayers:\n");

    for (int i = 0; i < game->playerCount; i++) {
        printf("%s | Cards: %d",
               game->players[i].name,
               game->players[i].handSize);

        if (game->players[i].passed) {
            printf(" | PASSED");
        }

        if (game->players[i].finished) {
            printf(" | OUT");
        }

        printf("\n");
    }

    printf("========================\n");
}

void vc_start(void) {
    VCGame game;
    int playerCount;

    printf("Enter player count (2-4): ");
    scanf("%d", &playerCount);

    if (!vc_init_game(&game, playerCount)) {
        printf("Invalid player count.\n");
        return;
    }

    vc_start_game(&game);

    while (game.state != VC_GAME_OVER) {
        print_state(&game);

        VCPlayer *player = &game.players[game.currentTurn];

        printf("\n%s's hand:\n", player->name);
        print_hand(player);

        printf("\nChoose action:\n");
        printf("1. Play cards\n");
        printf("2. Pass\n");
        printf("> ");

        int choice;
        scanf("%d", &choice);

        if (choice == 2) {
            vc_pass_turn(&game, game.currentTurn);
            continue;
        }

        if (choice == 1) {
            int count;
            int indexes[VC_MAX_COMBO];

            printf("How many cards? ");
            scanf("%d", &count);

            if (count <= 0 || count > VC_MAX_COMBO) {
                printf("Invalid count.\n");
                continue;
            }

            printf("Enter card indexes separated by spaces:\n");

            for (int i = 0; i < count; i++) {
                scanf("%d", &indexes[i]);
            }

            if (!vc_play_cards(&game, game.currentTurn, indexes, count)) {
                printf("Illegal play.\n");
            }

            continue;
        }

        printf("Invalid choice.\n");
    }

    printf("\n====================\n");
    printf("VC WINNER: %s\n",
           game.players[game.winnerSeat].name);
    printf("====================\n");
}