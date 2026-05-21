#include <stdio.h>
#include <string.h>

#include "LobbyManager.h"

void lobby_manager_init(LobbyManager *manager) {
    if (manager == NULL) {
        return;
    }

    memset(manager, 0, sizeof(LobbyManager));
    manager->nextLobbyId = 1;
}

Lobby *lobby_manager_find_lobby(LobbyManager *manager, int lobbyId) {
    if (manager == NULL) {
        return NULL;
    }

    for (int i = 0; i < manager->lobbyCount; i++) {
        if (manager->lobbies[i].id == lobbyId) {
            return &manager->lobbies[i];
        }
    }

    return NULL;
}

int lobby_manager_create_lobby(
    LobbyManager *manager,
    const char *name,
    LobbyGameType gameType,
    int maxPlayers
) {
    if (manager == NULL || name == NULL) {
        return -1;
    }

    if (manager->lobbyCount >= MAX_LOBBIES) {
        return -1;
    }

    int lobbyId = manager->nextLobbyId++;

    Lobby *lobby = &manager->lobbies[manager->lobbyCount];

    if (!lobby_init(lobby, lobbyId, name, gameType, maxPlayers)) {
        return -1;
    }

    manager->lobbyCount++;

    return lobbyId;
}

int lobby_manager_delete_lobby(LobbyManager *manager, int lobbyId) {
    if (manager == NULL) {
        return 0;
    }

    for (int i = 0; i < manager->lobbyCount; i++) {
        if (manager->lobbies[i].id == lobbyId) {

            for (int j = i; j < manager->lobbyCount - 1; j++) {
                manager->lobbies[j] = manager->lobbies[j + 1];
            }

            manager->lobbyCount--;
            return 1;
        }
    }

    return 0;
}

int lobby_manager_join_lobby(
    LobbyManager *manager,
    int lobbyId,
    int playerId,
    const char *playerName
) {
    Lobby *lobby = lobby_manager_find_lobby(manager, lobbyId);

    if (lobby == NULL) {
        return 0;
    }

    return lobby_add_player(lobby, playerId, playerName);
}

int lobby_manager_leave_lobby(
    LobbyManager *manager,
    int lobbyId,
    int playerId
) {
    Lobby *lobby = lobby_manager_find_lobby(manager, lobbyId);

    if (lobby == NULL) {
        return 0;
    }

    int result = lobby_remove_player(lobby, playerId);

    if (lobby->state == LOBBY_CLOSED || lobby->playerCount == 0) {
        lobby_manager_delete_lobby(manager, lobbyId);
    }

    return result;
}

int lobby_manager_set_player_ready(
    LobbyManager *manager,
    int lobbyId,
    int playerId,
    int ready
) {
    Lobby *lobby = lobby_manager_find_lobby(manager, lobbyId);

    if (lobby == NULL) {
        return 0;
    }

    return lobby_set_ready(lobby, playerId, ready);
}

int lobby_manager_start_lobby(
    LobbyManager *manager,
    int lobbyId,
    int hostPlayerId
) {
    Lobby *lobby = lobby_manager_find_lobby(manager, lobbyId);

    if (lobby == NULL) {
        return 0;
    }

    if (lobby->hostPlayerId != hostPlayerId) {
        return 0;
    }

    return lobby_start_game(lobby);
}

void lobby_manager_print_lobbies(LobbyManager *manager) {
    if (manager == NULL) {
        return;
    }

    printf("\n========== LOBBIES ==========\n");

    if (manager->lobbyCount == 0) {
        printf("No active lobbies.\n");
        printf("=============================\n");
        return;
    }

    for (int i = 0; i < manager->lobbyCount; i++) {
        Lobby *lobby = &manager->lobbies[i];

        printf("ID: %d | %s | %s | %s | %d/%d players\n",
               lobby->id,
               lobby->name,
               lobby_game_name(lobby->gameType),
               lobby_state_name(lobby->state),
               lobby->playerCount,
               lobby->maxPlayers);
    }

    printf("=============================\n");
}