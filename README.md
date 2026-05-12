

# Terminal Card Game Engine (C)

A terminal-based card game engine written entirely in vanilla C using a standard 52-card deck.

This project is being built from scratch to deepen understanding of:
- Low-level programming
- Memory management
- Data structures
- Game architecture
- Rendering systems
- Networking concepts
- Multiplayer systems
- Terminal UI/animation
- Performance optimization

The long-term goal is to evolve this project from a simple card game into a modular multiplayer game framework with animated terminal rendering and online play capabilities.

---

# Features

## Current Features
- Standard 52-card deck
- Deck initialization
- Deck shuffling
- Card drawing system
- Hand rendering in terminal
- Modular code structure

## Planned Features
- ASCII/Unicode card rendering
- Terminal animations
- Mouse/keyboard input system
- Turn-based game loop
- AI opponents
- Multiplayer networking
- Lobby system
- Movable player avatars
- Save/load systems
- Replay system
- Custom rule sets
- Cross-platform compatibility

---

# Tech Stack

- Language: C
- Compiler: GCC / Clang
- Build System: Make (planned)
- Platform: Linux/macOS initially
- Graphics: Terminal rendering (ASCII/Unicode)

---

# Project Structure

```txt
/card-game
│
├── src/
│   ├── main.c
│   ├── Deck.c
│   ├── Game.c
│   └── UI.c
│
├── include/
│   ├── Deck.h
│   ├── Game.h
│   └── UI.h
│
├── assets/
│
├── docs/
│
├── screenshots/
│
├── LICENSE
└── README.md
```

---

# Example Card Structure

```c
typedef struct {
    int rank;
    int suit;
} Card;
```

---

# Example Deck Structure

```c
typedef struct {
    Card cards[52];
    int topCard;
} Deck;
```

---

# Compilation

## Linux/macOS

```bash
gcc src/*.c -o cardgame
./cardgame
```

---

# Learning Goals

This project is intentionally being built without external game engines in order to develop:
- Systems programming skills
- Problem-solving ability
- Understanding of memory/layout
- Networking fundamentals
- Software architecture discipline

---

# Future Networking Goals

Planned multiplayer features include:
- Client/server architecture
- Socket programming
- Matchmaking
- Session management
- State synchronization
- Anti-cheat concepts
- Lightweight network optimization

---

# Why This Project Exists

Modern developers often rely heavily on engines and frameworks.

This project is an exercise in understanding what happens underneath the abstraction layers by building systems manually from the ground up.

---

# License

This project is licensed under the MIT License.

See the LICENSE file for details.

---

# Author

Built by ALvin Bryant

GitHub: https://github.com/alvinjbryant
