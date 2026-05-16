# Terminal Card Table Engine

A modular terminal-based card game engine written entirely in vanilla C.

This project began as a low-level programming exercise focused on understanding how card games, rendering systems, and game loops work beneath modern engines and frameworks. It has since evolved into a growing terminal game framework with plans for multiplayer networking, animated rendering, and scalable game architecture.

The engine currently includes:
- A fully modeled 52-card deck system
- Terminal-rendered playing cards using Unicode
- A modular game structure
- A playable Blackjack implementation
- Animated card draw effects in the terminal
- Makefile-based compilation

---

# Preview

## Unicode Card Rendering

```txt
┌─────────┐
│ A       │
│         │
│    ♠    │
│         │
│       A │
└─────────┘
```

## Current Games
- Blackjack
- Framework for additional games

## Planned Games
- Texas Hold'em
- Old Maid
- Multiplayer card tables
- Custom game modes

---

# Features

## Current Features
- Standard 52-card deck
- Deck initialization
- Fisher-Yates deck shuffling
- Card drawing system
- Unicode terminal card rendering
- ANSI color support
- Animated card draw effects
- Modular game structure
- Blackjack gameplay loop
- Menu system
- Makefile build support

---

# Planned Features

## Engine Features
- Cross-platform terminal support
- Better rendering abstraction
- Scene/state management
- Input handling system
- Save/load support
- Replay system
- Audio support

## Multiplayer Features
- Socket networking
- Client/server architecture
- Lobby system
- Matchmaking
- Synchronization systems
- Chat system

## Gameplay Features
- Texas Hold'em
- AI opponents
- Betting systems
- Multiple players
- Tournament support
- Rule customization

---

# Tech Stack

- Language: C
- Compiler: GCC / Clang
- Build System: Make
- Rendering: ASCII / Unicode terminal rendering
- Platform Target:
  - Linux
  - macOS
  - Windows support planned

---

# Project Structure

```txt
Card-Table-working-title/
│
├── main.c
├── Deck.c
├── Deck.h
├── Menu.c
├── Menu.h
├── Makefile
├── LICENSE
│
└── Games/
    └── Blackjack/
        ├── blackjack.c
        └── blackjack.h
```

---

# Core Architecture

## Deck System

The engine uses strongly typed enums and structs to model a real card deck.

### Card Structure

```c
typedef struct {
    Suit suit;
    Rank rank;
} Card;
```

### Deck Structure

```c
typedef struct {
    Card cards[DECK_SIZE];
    int top;
} Deck;
```

### Features
- Type-safe card representation
- Efficient array-based storage
- O(1) card draw operations
- Reusable deck system for multiple games

---

# Blackjack Module

The current playable implementation includes:
- Player hit/stand loop
- Dealer AI
- Hand scoring
- Bust detection
- Win/loss/push evaluation

The Blackjack game is intentionally modularized into:

```txt
Games/Blackjack/
```

This structure allows future games to be added cleanly without modifying the engine core.

---

# Compilation

## Linux / macOS

Build:

```bash
make build
```

Run:

```bash
./cardgame
```

---

## Manual Compilation

```bash
gcc main.c Deck.c Menu.c Games/Blackjack/blackjack.c -o cardgame
```

---

# Design Philosophy

This project intentionally avoids external libraries and engines in order to develop:
- Systems programming skills
- Memory management understanding
- Data structure fluency
- Software architecture discipline
- Rendering fundamentals
- Networking knowledge
- Performance awareness

The goal is to understand how systems work underneath abstraction layers rather than relying entirely on frameworks.

---

# Learning Goals

This project is being used to study and practice:
- Low-level C programming
- Modular architecture
- Terminal rendering
- Data-oriented design
- Multiplayer networking concepts
- Real-time synchronization
- Cross-platform compilation
- Input systems
- Game state management

---

# Roadmap

## Near-Term Goals
- Better Blackjack UI
- Hidden dealer cards
- Chip/betting system
- Input validation
- Cross-platform terminal clearing

## Mid-Term Goals
- Texas Hold'em
- Multiple players
- Terminal animations
- Game state manager
- Better rendering pipeline

## Long-Term Goals
- Online multiplayer
- Dedicated server support
- Matchmaking/lobbies
- Replay system
- Spectator mode
- Modular plugin-style game support

---

# Example Concepts Planned

## Networking
- TCP/UDP socket systems
- Lightweight packet protocols
- State replication
- Anti-cheat concepts
- Latency handling

## Rendering
- Animated card movement
- Layered terminal rendering
- Dynamic UI panels
- Cursor positioning
- Color abstraction

---

# License

This project is licensed under the MIT License.

See the LICENSE file for more information.

---

# Author

Built by A. Bryant

- GitHub: https://github.com/alvinjbryant

---

# Why This Project Exists

Modern software development often hides complexity behind layers of abstraction.

This project exists to learn those underlying systems directly by building them manually:
- card systems
- rendering
- game loops
- architecture
- networking
- memory handling

The long-term vision is to turn this into a fully modular multiplayer terminal card game framework written entirely in C.
