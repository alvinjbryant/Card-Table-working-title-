#include <string.h>

#include "LobbyResult.h"

LobbyResult lobby_result_make(
    LobbyResultCode code,
    LobbyEventType event,
    int lobbyId,
    int playerId,
    int hostPlayerId,
    const char *message
) {
    LobbyResult result;

    memset(&result, 0, sizeof(LobbyResult));

    result.code = code;
    result.event = event;
    result.success = (code == LOBBY_RESULT_OK);

    result.lobbyId = lobbyId;
    result.playerId = playerId;
    result.hostPlayerId = hostPlayerId;

    if (message != NULL) {
        strncpy(result.message, message, LOBBY_RESULT_MESSAGE_SIZE - 1);
    }

    return result;
}

const char *lobby_result_code_name(LobbyResultCode code) {
    switch (code) {
        case LOBBY_RESULT_OK: return "OK";
        case LOBBY_RESULT_ERROR: return "ERROR";
        case LOBBY_RESULT_LOBBY_NOT_FOUND: return "LOBBY_NOT_FOUND";
        case LOBBY_RESULT_LOBBY_FULL: return "LOBBY_FULL";
        case LOBBY_RESULT_INVALID_PLAYER: return "INVALID_PLAYER";
        case LOBBY_RESULT_PLAYER_ALREADY_IN_LOBBY: return "PLAYER_ALREADY_IN_LOBBY";
        case LOBBY_RESULT_PLAYER_NOT_IN_LOBBY: return "PLAYER_NOT_IN_LOBBY";
        case LOBBY_RESULT_NOT_HOST: return "NOT_HOST";
        case LOBBY_RESULT_NOT_ENOUGH_PLAYERS: return "NOT_ENOUGH_PLAYERS";
        case LOBBY_RESULT_NOT_ALL_READY: return "NOT_ALL_READY";
        case LOBBY_RESULT_INVALID_GAME_TYPE: return "INVALID_GAME_TYPE";
        case LOBBY_RESULT_INVALID_PLAYER_COUNT: return "INVALID_PLAYER_COUNT";
        case LOBBY_RESULT_LOBBY_ALREADY_IN_GAME: return "LOBBY_ALREADY_IN_GAME";
        case LOBBY_RESULT_MAX_LOBBIES_REACHED: return "MAX_LOBBIES_REACHED";
        default: return "UNKNOWN";
    }
}

const char *lobby_event_type_name(LobbyEventType event) {
    switch (event) {
        case LOBBY_EVENT_NONE: return "NONE";
        case LOBBY_EVENT_CREATED: return "CREATED";
        case LOBBY_EVENT_DELETED: return "DELETED";
        case LOBBY_EVENT_PLAYER_JOINED: return "PLAYER_JOINED";
        case LOBBY_EVENT_PLAYER_LEFT: return "PLAYER_LEFT";
        case LOBBY_EVENT_PLAYER_READY_CHANGED: return "PLAYER_READY_CHANGED";
        case LOBBY_EVENT_GAME_STARTED: return "GAME_STARTED";
        case LOBBY_EVENT_HOST_CHANGED: return "HOST_CHANGED";
        case LOBBY_EVENT_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}