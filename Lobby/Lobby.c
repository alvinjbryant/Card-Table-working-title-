#include <stdio.h>
#include <string.h>

#include "Lobby.h"

const char *lobby_game_name(LobbyGameType gameType) {
    switch (gameType) {
        case GAME_BLACKJACK: return "Blackjack";
        case GAME_TEXAS_HOLDEM: return "Texas Hold'em";
        case GAME_OLD_MAID: return "Old Maid";
        case GAME_VC: return "VC";
        default: return "Unknown";
    }
}

const char *lobby_state_name(LobbyState state) {
    switch (state) {
        case LOBBY_WAITING: return "Waiting";
        case LOBBY_READY: return "Ready";
        case LOBBY_IN_GAME: return "In Game";
        case LOBBY_CLOSED: return "Closed";
        default: return "Unknown";
    }
}

static int lobby_min_players(LobbyGameType gameType) {
    switch (gameType) {
        case GAME_BLACKJACK: return 1;
        case GAME_TEXAS_HOLDEM: return 2;
        case GAME_OLD_MAID: return 2;
        case GAME_VC: return 2;
        default: return 2;
    }
}

static int lobby_game_max_players(LobbyGameType gameType) {
    switch (gameType) {
        case GAME_BLACKJACK: return 7;
        case GAME_TEXAS_HOLDEM: return 10;
        case GAME_OLD_MAID: return 10;
        case GAME_VC: return 4;
        default: return LOBBY_MAX_PLAYERS;
    }
}

int lobby_init(Lobby *lobby, int lobbyId, const char *name, LobbyGameType gameType, int maxPlayers) {
    if (lobby == NULL || name == NULL) {
        return 0;
    }

    int gameMax = lobby_game_max_players(gameType);

    if (maxPlayers < lobby_min_players(gameType) || maxPlayers > gameMax) {
        return 0;
    }

    memset(lobby, 0, sizeof(Lobby));

    lobby->id = lobbyId;
    strncpy(lobby->name, name, LOBBY_MAX_NAME - 1);

    lobby->gameType = gameType;
    lobby->state = LOBBY_WAITING;

    lobby->maxPlayers = maxPlayers;
    lobby->playerCount = 0;
    lobby->hostPlayerId = -1;

    return 1;
}

int lobby_find_player_index(Lobby *lobby, int playerId) {
    if (lobby == NULL) {
        return -1;
    }

    for (int i = 0; i < lobby->playerCount; i++) {
        if (lobby->players[i].id == playerId) {
            return i;
        }
    }

    return -1;
}

int lobby_add_player(Lobby *lobby, int playerId, const char *playerName) {
    if (lobby == NULL || playerName == NULL) {
        return 0;
    }

    if (lobby->state != LOBBY_WAITING && lobby->state != LOBBY_READY) {
        return 0;
    }

    if (lobby->playerCount >= lobby->maxPlayers) {
        return 0;
    }

    if (lobby_find_player_index(lobby, playerId) != -1) {
        return 0;
    }

    LobbyPlayer *player = &lobby->players[lobby->playerCount];

    player->id = playerId;
    strncpy(player->name, playerName, LOBBY_MAX_NAME - 1);
    player->connected = 1;
    player->ready = 0;
    player->isHost = 0;

    if (lobby->playerCount == 0) {
        player->isHost = 1;
        lobby->hostPlayerId = playerId;
    }

    lobby->playerCount++;

    lobby->state = LOBBY_WAITING;

    return 1;
}

int lobby_remove_player(Lobby *lobby, int playerId) {
    if (lobby == NULL) {
        return 0;
    }

    int index = lobby_find_player_index(lobby, playerId);

    if (index == -1) {
        return 0;
    }

    int removedHost = lobby->players[index].isHost;

    for (int i = index; i < lobby->playerCount - 1; i++) {
        lobby->players[i] = lobby->players[i + 1];
    }

    lobby->playerCount--;

    if (lobby->playerCount == 0) {
        lobby->hostPlayerId = -1;
        lobby->state = LOBBY_CLOSED;
        return 1;
    }

    if (removedHost) {
        lobby->players[0].isHost = 1;
        lobby->hostPlayerId = lobby->players[0].id;
    }

    lobby->state = LOBBY_WAITING;

    return 1;
}

int lobby_set_ready(Lobby *lobby, int playerId, int ready) {
    if (lobby == NULL) {
        return 0;
    }

    int index = lobby_find_player_index(lobby, playerId);

    if (index == -1) {
        return 0;
    }

    lobby->players[index].ready = ready ? 1 : 0;

    if (lobby_can_start(lobby)) {
        lobby->state = LOBBY_READY;
    } else {
        lobby->state = LOBBY_WAITING;
    }

    return 1;
}

int lobby_can_start(Lobby *lobby) {
    if (lobby == NULL) {
        return 0;
    }

    if (lobby->state == LOBBY_IN_GAME || lobby->state == LOBBY_CLOSED) {
        return 0;
    }

    if (lobby->playerCount < lobby_min_players(lobby->gameType)) {
        return 0;
    }

    for (int i = 0; i < lobby->playerCount; i++) {
        if (!lobby->players[i].connected || !lobby->players[i].ready) {
            return 0;
        }
    }

    return 1;
}

int lobby_start_game(Lobby *lobby) {
    if (!lobby_can_start(lobby)) {
        return 0;
    }

    lobby->state = LOBBY_IN_GAME;
    return 1;
}

void lobby_print(const Lobby *lobby) {
    if (lobby == NULL) {
        return;
    }

    printf("\n========== LOBBY ==========\n");
    printf("Lobby: %s\n", lobby->name);
    printf("Game: %s\n", lobby_game_name(lobby->gameType));
    printf("State: %s\n", lobby_state_name(lobby->state));
    printf("Players: %d / %d\n", lobby->playerCount, lobby->maxPlayers);

    printf("\n");

    for (int i = 0; i < lobby->playerCount; i++) {
        const LobbyPlayer *player = &lobby->players[i];

        printf("%d. %s", i + 1, player->name);

        if (player->isHost) {
            printf(" | HOST");
        }

        if (player->ready) {
            printf(" | READY");
        } else {
            printf(" | NOT READY");
        }

        if (!player->connected) {
            printf(" | DISCONNECTED");
        }

        printf("\n");
    }

    printf("===========================\n");
}