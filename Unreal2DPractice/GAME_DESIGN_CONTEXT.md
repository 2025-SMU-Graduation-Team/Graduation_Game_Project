# GRADUATION PROJECT GAME 'BEHIND' – AI Context File

## Project Overview
HidingSpot is a horror chase game developed with Unreal Engine (C++).
The core gameplay revolves around a weak player character trying to survive
while being chased by a monster. The player must hide, interact with objects,
and make strategic decisions to avoid being caught.

The project is part of a university graduation project and is built with a
small team including programmers and a pixel-art designer.

---

# Engine & Technical Stack
Engine: Unreal Engine 5  
Language: C++ (with Blueprint integration where needed)

Core systems are implemented in C++ for gameplay logic.

Important Unreal classes used frequently:
- Actor
- ActorComponent
- GameInstance
- Widget (UMG)
- Data classes for sound and game data

---

# Gameplay Design

## Core Gameplay Loop

1. Player explores the environment.
2. Monster begins chasing the player.
3. Player must hide or avoid detection.
4. Environmental interactions help survival.
5. If caught → game over.
6. If conditions are met → escape / ending.

---

# Main Systems

## Hiding System

The player can hide inside specific objects or locations.

Example hiding locations:
- lockers
- closets
- small hiding spots in the environment

Relevant classes:
- `HidingSpot`
- interaction logic using interactable actors

Responsibilities:
- detect player interaction
- hide player character
- prevent monster detection

---

## Interaction System

Objects in the world can be interacted with.

Typical interactions include:
- opening doors
- triggering events
- hiding
- activating UI

Relevant classes:
- `InteractableActor`
- `DoorInteractionActor`
- interaction widgets

Interaction flow:

Player input → detect interactable actor → execute interaction logic → update UI or gameplay state.

---

## Audio System

Audio is centralized through an Audio Manager.

Class:
`AudioManager`

Responsibilities:
- manage SFX
- manage BGM
- play UI sounds
- play world sounds

Important Unreal functions used:

### PlaySound2D
Used for UI and non-spatial sounds.

Example:
- button click sound
- UI notifications
- menu sounds

### PlaySoundAtLocation
Used for world sounds.

Example:
- environmental sound
- monster sounds
- interaction sounds

Audio data is organized through data classes:

- `GameSFXData`
- `GameBGMData`
- `GameSoundData`

---
## Mini Games

The game contains two mini games.

1. Wire Puzzle
- Player presses 9 buttons in sequence
- Cutting the correct wire progresses the stage
- Wrong actions increase a buzzer sound

2. Control Room Puzzle
- Player inputs a subway station name
- The arriving train determines the ending

Normal Ending
- Player escapes the subway safely

Hidden Ending
- Player runs through underground mall
- Walls collapse from the ceiling
- Monster chase sequence occurs

---

## Game Instance

Custom GameInstance stores global data.

Class:
`MyGameInstance`

Used for:
- accessing sound data
- storing shared systems
- global references

Example usage:

AudioManager retrieves SFX and BGM data from GameInstance.

---

# UI System

UI uses Unreal UMG widgets.

Typical UI features:

- interaction prompts
- menu UI
- button interactions
- gameplay notifications

Example classes:

- `InteractionWidget`
- menu widgets
- UI sound feedback

UI Types
- Game UI
- Interaction UI
- MiniGame UI

---

# AI / Monster System

Monster behavior includes:

- chasing the player
- detecting the player
- reacting to hiding behavior

Monster interactions with hiding system are important for gameplay tension.

---

Game Systems
- AudioManager
- HidingSpot
- Monster Chase
- Interaction System
- MiniGame System

---

## Mini Games

The game includes two mini-game types:

- a wire puzzle involving ordered button input and correct wire cutting
- a control room station selection system that influences train events and ending progression

---

# Coding Style Guidelines

General principles used in the project:

- Managers handle system-wide functionality (e.g., AudioManager)
- Actors represent interactable world objects
- Data classes store reusable configuration data
- GameInstance stores global references

Unreal gameplay utilities commonly used:

- `UGameplayStatics`
- `CreateDefaultSubobject`
- Actor lifecycle functions (BeginPlay, Tick)

---

# Project Goals

Primary goal:
Create psychological tension through hiding mechanics
and chase gameplay.

Important design goals:

- player vulnerability
- decision pressure
- environmental storytelling
- simple but tense mechanics

---

# Notes for AI Assistants

When analyzing this project:

- Treat AudioManager as the central audio system.
- Interaction logic is actor-based.
- HidingSpot mechanics are core gameplay features.
- UI sounds should use PlaySound2D.
- World sounds should use PlaySoundAtLocation.

AI tools should prioritize:
- Unreal Engine best practices
- clean C++ gameplay architecture
- modular system design