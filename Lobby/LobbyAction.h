#ifndef LOBBY_ACTION_H
#define LOBBY_ACTION_H

#include "Lobby.h"
#include "LobbyResult.h"

#define LOBBY_ACTION_NAME_SIZE 32

typedef enum {
    LOBBY_ACTION_NONE,

    LOBBY_ACTION_CREATE,
    LOBBY_ACTION_JOIN,
    LOBBY_ACTION_LEAVE,
    LOBBY_ACTION_SET_READY,
    LOBBY_ACTION_START_GAME,
    LOBBY_ACTION_DELETE,
    LOBBY_ACTION_LIST,
    LOBBY_ACTION_INSPECT
} LobbyActionType;

typedef struct {
    LobbyActionType type;

    int lobbyId;
    int playerId;
    int hostPlayerId;

    LobbyGameType gameType;

    int maxPlayers;
    int ready;

    char lobbyName[LOBBY_ACTION_NAME_SIZE];
    char playerName[LOBBY_ACTION_NAME_SIZE];
} LobbyAction;

LobbyAction lobby_action_create_empty(void);

LobbyAction lobby_action_create_lobby(
    int playerId,
    const char *lobbyName,
    LobbyGameType gameType,
    int maxPlayers
);

LobbyAction lobby_action_join_lobby(
    int lobbyId,
    int playerId,
    const char *playerName
);

LobbyAction lobby_action_leave_lobby(
    int lobbyId,
    int playerId
);

LobbyAction lobby_action_set_ready(
    int lobbyId,
    int playerId,
    int ready
);

LobbyAction lobby_action_start_game(
    int lobbyId,
    int hostPlayerId
);

LobbyResult lobby_action_validate(const LobbyAction *action);

const char *lobby_action_type_name(LobbyActionType type);

#endif