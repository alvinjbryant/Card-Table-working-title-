#ifndef LOBBY_MANAGER_H
#define LOBBY_MANAGER_H

#include "Lobby.h"
#include "LobbyResult.h"

#define MAX_LOBBIES 32

typedef struct {
    Lobby lobbies[MAX_LOBBIES];
    int lobbyCount;
    int nextLobbyId;
} LobbyManager;

void lobby_manager_init(LobbyManager *manager);

int lobby_manager_create_lobby(
    LobbyManager *manager,
    const char *name,
    LobbyGameType gameType,
    int maxPlayers
);

int lobby_manager_delete_lobby(LobbyManager *manager, int lobbyId);

Lobby *lobby_manager_find_lobby(LobbyManager *manager, int lobbyId);

int lobby_manager_join_lobby(
    LobbyManager *manager,
    int lobbyId,
    int playerId,
    const char *playerName
);

int lobby_manager_leave_lobby(
    LobbyManager *manager,
    int lobbyId,
    int playerId
);

int lobby_manager_set_player_ready(
    LobbyManager *manager,
    int lobbyId,
    int playerId,
    int ready
);

int lobby_manager_start_lobby(
    LobbyManager *manager,
    int lobbyId,
    int hostPlayerId
);

void lobby_manager_print_lobbies(LobbyManager *manager);

#endif