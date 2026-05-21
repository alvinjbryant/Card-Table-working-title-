#ifndef LOBBY_RESULT_H
#define LOBBY_RESULT_H

#define LOBBY_RESULT_MESSAGE_SIZE 128

typedef enum {
    LOBBY_RESULT_OK,

    LOBBY_RESULT_ERROR,
    LOBBY_RESULT_LOBBY_NOT_FOUND,
    LOBBY_RESULT_LOBBY_FULL,
    LOBBY_RESULT_INVALID_PLAYER,
    LOBBY_RESULT_PLAYER_ALREADY_IN_LOBBY,
    LOBBY_RESULT_PLAYER_NOT_IN_LOBBY,
    LOBBY_RESULT_NOT_HOST,
    LOBBY_RESULT_NOT_ENOUGH_PLAYERS,
    LOBBY_RESULT_NOT_ALL_READY,
    LOBBY_RESULT_INVALID_GAME_TYPE,
    LOBBY_RESULT_INVALID_PLAYER_COUNT,
    LOBBY_RESULT_LOBBY_ALREADY_IN_GAME,
    LOBBY_RESULT_MAX_LOBBIES_REACHED
} LobbyResultCode;

typedef enum {
    LOBBY_EVENT_NONE,

    LOBBY_EVENT_CREATED,
    LOBBY_EVENT_DELETED,
    LOBBY_EVENT_PLAYER_JOINED,
    LOBBY_EVENT_PLAYER_LEFT,
    LOBBY_EVENT_PLAYER_READY_CHANGED,
    LOBBY_EVENT_GAME_STARTED,
    LOBBY_EVENT_HOST_CHANGED,
    LOBBY_EVENT_ERROR
} LobbyEventType;

typedef struct {
    LobbyResultCode code;
    LobbyEventType event;

    int success;

    int lobbyId;
    int playerId;
    int hostPlayerId;

    char message[LOBBY_RESULT_MESSAGE_SIZE];
} LobbyResult;

LobbyResult lobby_result_make(
    LobbyResultCode code,
    LobbyEventType event,
    int lobbyId,
    int playerId,
    int hostPlayerId,
    const char *message
);

const char *lobby_result_code_name(LobbyResultCode code);
const char *lobby_event_type_name(LobbyEventType event);

#endif