#ifndef LOBBY_H
#define LOBBY_H

#define LOBBY_MAX_PLAYERS 10
#define LOBBY_MAX_NAME 32

typedef enum {
    GAME_BLACKJACK,
    GAME_TEXAS_HOLDEM,
    GAME_OLD_MAID,
    GAME_VC
} LobbyGameType;

typedef enum {
    LOBBY_WAITING,
    LOBBY_READY,
    LOBBY_IN_GAME,
    LOBBY_CLOSED
} LobbyState;

typedef struct {
    int id;
    char name[LOBBY_MAX_NAME];
    int connected;
    int ready;
    int isHost;
} LobbyPlayer;

typedef struct {
    int id;
    char name[LOBBY_MAX_NAME];

    LobbyGameType gameType;
    LobbyState state;

    LobbyPlayer players[LOBBY_MAX_PLAYERS];
    int playerCount;
    int maxPlayers;

    int hostPlayerId;
} Lobby;

int lobby_init(Lobby *lobby, int lobbyId, const char *name, LobbyGameType gameType, int maxPlayers);

int lobby_add_player(Lobby *lobby, int playerId, const char *playerName);
int lobby_remove_player(Lobby *lobby, int playerId);

int lobby_set_ready(Lobby *lobby, int playerId, int ready);
int lobby_can_start(Lobby *lobby);
int lobby_start_game(Lobby *lobby);

int lobby_find_player_index(Lobby *lobby, int playerId);

const char *lobby_game_name(LobbyGameType gameType);
const char *lobby_state_name(LobbyState state);

void lobby_print(const Lobby *lobby);

#endif