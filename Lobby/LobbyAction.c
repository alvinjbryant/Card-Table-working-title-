#include <string.h>

#include "LobbyAction.h"

static int safe_string_valid(const char *text) {
    if (text == NULL) {
        return 0;
    }

    if (text[0] == '\0') {
        return 0;
    }

    return 1;
}

static void safe_copy_name(char *dest, const char *src) {
    if (dest == NULL) {
        return;
    }

    dest[0] = '\0';

    if (src == NULL) {
        return;
    }

    strncpy(dest, src, LOBBY_ACTION_NAME_SIZE - 1);
    dest[LOBBY_ACTION_NAME_SIZE - 1] = '\0';
}

LobbyAction lobby_action_create_empty(void) {
    LobbyAction action;
    memset(&action, 0, sizeof(LobbyAction));

    action.type = LOBBY_ACTION_NONE;
    action.lobbyId = -1;
    action.playerId = -1;
    action.hostPlayerId = -1;

    return action;
}

LobbyAction lobby_action_create_lobby(
    int playerId,
    const char *lobbyName,
    LobbyGameType gameType,
    int maxPlayers
) {
    LobbyAction action = lobby_action_create_empty();

    action.type = LOBBY_ACTION_CREATE;
    action.playerId = playerId;
    action.gameType = gameType;
    action.maxPlayers = maxPlayers;

    safe_copy_name(action.lobbyName, lobbyName);

    return action;
}

LobbyAction lobby_action_join_lobby(
    int lobbyId,
    int playerId,
    const char *playerName
) {
    LobbyAction action = lobby_action_create_empty();

    action.type = LOBBY_ACTION_JOIN;
    action.lobbyId = lobbyId;
    action.playerId = playerId;

    safe_copy_name(action.playerName, playerName);

    return action;
}

LobbyAction lobby_action_leave_lobby(
    int lobbyId,
    int playerId
) {
    LobbyAction action = lobby_action_create_empty();

    action.type = LOBBY_ACTION_LEAVE;
    action.lobbyId = lobbyId;
    action.playerId = playerId;

    return action;
}

LobbyAction lobby_action_set_ready(
    int lobbyId,
    int playerId,
    int ready
) {
    LobbyAction action = lobby_action_create_empty();

    action.type = LOBBY_ACTION_SET_READY;
    action.lobbyId = lobbyId;
    action.playerId = playerId;
    action.ready = ready ? 1 : 0;

    return action;
}

LobbyAction lobby_action_start_game(
    int lobbyId,
    int hostPlayerId
) {
    LobbyAction action = lobby_action_create_empty();

    action.type = LOBBY_ACTION_START_GAME;
    action.lobbyId = lobbyId;
    action.hostPlayerId = hostPlayerId;

    return action;
}

LobbyResult lobby_action_validate(const LobbyAction *action) {
    if (action == NULL) {
        return lobby_result_make(
            LOBBY_RESULT_ERROR,
            LOBBY_EVENT_ERROR,
            -1,
            -1,
            -1,
            "Lobby action was NULL."
        );
    }

    switch (action->type) {
        case LOBBY_ACTION_CREATE:
            if (action->playerId < 0) {
                return lobby_result_make(
                    LOBBY_RESULT_INVALID_PLAYER,
                    LOBBY_EVENT_ERROR,
                    -1,
                    action->playerId,
                    -1,
                    "Invalid player ID."
                );
            }

            if (!safe_string_valid(action->lobbyName)) {
                return lobby_result_make(
                    LOBBY_RESULT_ERROR,
                    LOBBY_EVENT_ERROR,
                    -1,
                    action->playerId,
                    -1,
                    "Lobby name is required."
                );
            }

            if (action->maxPlayers <= 0 || action->maxPlayers > LOBBY_MAX_PLAYERS) {
                return lobby_result_make(
                    LOBBY_RESULT_INVALID_PLAYER_COUNT,
                    LOBBY_EVENT_ERROR,
                    -1,
                    action->playerId,
                    -1,
                    "Invalid lobby player count."
                );
            }

            return lobby_result_make(
                LOBBY_RESULT_OK,
                LOBBY_EVENT_NONE,
                -1,
                action->playerId,
                -1,
                "Lobby action is valid."
            );

        case LOBBY_ACTION_JOIN:
            if (action->lobbyId < 0) {
                return lobby_result_make(
                    LOBBY_RESULT_LOBBY_NOT_FOUND,
                    LOBBY_EVENT_ERROR,
                    action->lobbyId,
                    action->playerId,
                    -1,
                    "Invalid lobby ID."
                );
            }

            if (action->playerId < 0) {
                return lobby_result_make(
                    LOBBY_RESULT_INVALID_PLAYER,
                    LOBBY_EVENT_ERROR,
                    action->lobbyId,
                    action->playerId,
                    -1,
                    "Invalid player ID."
                );
            }

            if (!safe_string_valid(action->playerName)) {
                return lobby_result_make(
                    LOBBY_RESULT_ERROR,
                    LOBBY_EVENT_ERROR,
                    action->lobbyId,
                    action->playerId,
                    -1,
                    "Player name is required."
                );
            }

            return lobby_result_make(
                LOBBY_RESULT_OK,
                LOBBY_EVENT_NONE,
                action->lobbyId,
                action->playerId,
                -1,
                "Lobby action is valid."
            );

        case LOBBY_ACTION_LEAVE:
        case LOBBY_ACTION_SET_READY:
            if (action->lobbyId < 0 || action->playerId < 0) {
                return lobby_result_make(
                    LOBBY_RESULT_ERROR,
                    LOBBY_EVENT_ERROR,
                    action->lobbyId,
                    action->playerId,
                    -1,
                    "Lobby ID and player ID are required."
                );
            }

            return lobby_result_make(
                LOBBY_RESULT_OK,
                LOBBY_EVENT_NONE,
                action->lobbyId,
                action->playerId,
                -1,
                "Lobby action is valid."
            );

        case LOBBY_ACTION_START_GAME:
            if (action->lobbyId < 0 || action->hostPlayerId < 0) {
                return lobby_result_make(
                    LOBBY_RESULT_ERROR,
                    LOBBY_EVENT_ERROR,
                    action->lobbyId,
                    -1,
                    action->hostPlayerId,
                    "Lobby ID and host player ID are required."
                );
            }

            return lobby_result_make(
                LOBBY_RESULT_OK,
                LOBBY_EVENT_NONE,
                action->lobbyId,
                -1,
                action->hostPlayerId,
                "Lobby action is valid."
            );

        case LOBBY_ACTION_DELETE:
        case LOBBY_ACTION_LIST:
        case LOBBY_ACTION_INSPECT:
        case LOBBY_ACTION_NONE:
        default:
            return lobby_result_make(
                LOBBY_RESULT_ERROR,
                LOBBY_EVENT_ERROR,
                action->lobbyId,
                action->playerId,
                action->hostPlayerId,
                "Unsupported lobby action."
            );
    }
}

const char *lobby_action_type_name(LobbyActionType type) {
    switch (type) {
        case LOBBY_ACTION_NONE: return "NONE";
        case LOBBY_ACTION_CREATE: return "CREATE";
        case LOBBY_ACTION_JOIN: return "JOIN";
        case LOBBY_ACTION_LEAVE: return "LEAVE";
        case LOBBY_ACTION_SET_READY: return "SET_READY";
        case LOBBY_ACTION_START_GAME: return "START_GAME";
        case LOBBY_ACTION_DELETE: return "DELETE";
        case LOBBY_ACTION_LIST: return "LIST";
        case LOBBY_ACTION_INSPECT: return "INSPECT";
        default: return "UNKNOWN";
    }
}