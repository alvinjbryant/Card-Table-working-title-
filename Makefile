CC = gcc

SRC = main.c \
      Deck.c \
      Menu.c \
      Lobby/Lobby.c \
	  Lobby/LobbyManager.c \
      Lobby/LobbyResult.c\
      Games/Blackjack/blackjack.c \
      Games/TexasHoldem/TexasHoldem.c \
      Games/OldMaid/OldMaid.c \
      Games/VC/VC.c

OUT = cardgame

build:
	$(CC) $(SRC) -o $(OUT)
