# GRADUATION PROJECT GAME 'BEHIND' – Codebase Overview

This document describes the **actual implemented structure of the project codebase**.
It complements the higher-level design document (`AI_Overall_DESIGN_Context`).

While the main context document explains the game concept and direction,  
this file explains **how the game is currently implemented in code**.

---

# Game Overview

HidingSpot is a **2D horror chase game built in Unreal Engine using C++ and PaperCharacter**.

The player explores a **subway platform environment**, interacts with objects and NPCs,
solves puzzles, avoids a monster by hiding, and ultimately reaches one of two endings
depending on subway timing and state.

Core gameplay themes:

- survival and evasion
- hiding mechanics
- puzzle solving
- timed subway events
- branching endings

---

# Player System

Player logic is implemented in:

MyPaperCharacter.h  
MyPaperCharacter.cpp

Responsibilities:

- movement
- jumping
- interaction
- hiding
- inventory usage
- special interaction for subway boarding and cutscenes

Input summary:

E  
General interaction with world objects.

W  
Special interaction used for subway boarding and cutscene triggers.

Inventory  
Allows equipping and using collected items.

---

# Interaction System

Base interaction class:

InteractableActor.cpp

Derived interaction types include:

ItemActor.cpp  
Handles collectible items.

NPCActor.cpp  
Handles NPC interactions.

HidingSpot.cpp  
Handles hiding locations where the player can avoid monster detection.

Interaction flow:

Player presses interaction key  
↓  
InteractableActor is detected  
↓  
Specific interaction logic executes

---

# Monster System

Monster behavior is implemented primarily in:

MyPaperMonster.cpp  
MonsterSpawnManager.cpp

Responsibilities:

MonsterSpawnManager

- handles monster spawning
- manages spawn timing and placement

MyPaperMonster

- monster movement (generally straight-line pursuit)
- collision detection with player
- ignoring hidden players when applicable
- triggering player death sequence

Monster interactions include:

- detecting player overlap
- chasing behavior
- death animation or failure sequence

---

# Mini Game Systems

The project currently includes two mini-game style systems, and their implementation styles are different.

## 1. Wire Puzzle

The wire puzzle appears to be primarily **Blueprint-driven**, with limited direct C++ control.

Current understanding:

- Step 1: the player presses multiple buttons in the correct order
- Step 2: the player cuts the correct wire
- Success increases the puzzle gauge and eventually completes the sequence

Relevant audio data exists in C++:

- `Mini_BeepSoundHigh`
- `Mini_LineCutting`
- `Mini_PanelGageup`
- `Mini_BtnWrong`

These are defined in `GameSFXData.h`, but the actual puzzle progression logic appears to be handled mostly in Blueprint widgets or Blueprint actors.

Implementation note:

- `WBP_PuzzleButton` already uses `Beep_high_thin`
- `Mini_LineCutting` should be triggered at the wire-cut event
- `Mini_PanelGageup` should be triggered at the gauge increase event

## 2. Control Room Station Selection System

The control room mini-game is implemented more clearly in C++ and functions as a **branching event trigger system** rather than a standalone puzzle.

Flow summary:

- `MachineActor.cpp` opens the control room machine UI
- `InteractionWidget.cpp` handles station selection UI logic
- Station buttons (`Station_A/B/C/D`) store the selected station in `UISelectedManager.h`
- The linked `DelayedTaskData.h` is passed into `SubLevelTaskManager.cpp`
- `SubLevelTaskManager.cpp` schedules train movement and platform events
- This leads to subway state changes such as `DoorsOpen` or `Passed`
- `SubwayStateActor.cpp` then drives normal ending vs hidden ending branching

System meaning:

This mini-game determines which train/event flow will be sent to the platform, and therefore strongly affects ending progression.

---

# Subway State System

The subway system controls the main game progression.

Main controller:

SubwayStateActor.cpp

Subway states:

Approaching  
The train is approaching the platform.

DoorsOpen  
Doors are open and the player can board the train.

Leaving  
The train is leaving the station.

Passed  
The train has passed the station.

---

# Ending System

Ending logic depends on subway state and player actions.

Ending controller:

EndingDirector.cpp

Sequences:

NormalEndingSequence.cpp  
Triggered when the player boards the train during the correct state.

HiddenEndingSequence.cpp  
Triggered when the train has passed the station.

Hidden ending features:

- underground mall traversal
- collapsing ceilings
- obstacle avoidance
- monster chase sequence

---

# Level Transition System

Level transitions are handled by:

LevelTransitionManager.cpp

Responsibilities:

- level streaming
- switching between gameplay spaces
- transitioning to ending sequences

---

# Event / Task System

Timed gameplay events are handled by:

SubLevelTaskManager.cpp

Responsibilities include:

- delayed tasks
- scheduled gameplay events
- subway arrival timing
- control room event triggers

---

# Global Game Data

Global game systems are stored in:

MyGameInstance.h

Responsibilities:

- storing shared references
- managing global data
- providing access to sound resources

---

# Audio System

Audio resources and playback are handled through a centralized audio system.

Data classes:

GameSFXData.h  
GameBGMData.h

Playback manager:

AudioManager.cpp

Responsibilities:

- managing sound effects
- managing background music
- centralizing audio playback

Audio should generally be routed through the AudioManager.

---

# Inventory System

Inventory is implemented in:

InventoryComponent.cpp

Responsibilities:

- item pickup
- UI inventory display
- slot selection
- equipping items
- confirming pickups
- puzzle-based item usage

Example puzzle logic:

Wallet → Card conversion

---

# Game Flow Summary

Typical gameplay progression:

Player explores subway environment  
↓  
Player interacts with items, NPCs, puzzles  
↓  
Monster may spawn and chase the player  
↓  
Player hides or evades the monster  
↓  
Subway arrival state determines ending path

DoorsOpen → Normal Ending  
Passed → Hidden Ending

---

# Notes for AI Assistants

This project uses **Unreal Engine C++ gameplay systems** with some Blueprint UI components.

Key systems to consider when modifying gameplay:

- Player logic (MyPaperCharacter)
- Interaction system (InteractableActor hierarchy)
- Monster system (MyPaperMonster / MonsterSpawnManager)
- Subway state system (SubwayStateActor)
- Ending system (EndingDirector)
- Audio system (AudioManager)
- Inventory system (InventoryComponent)

When suggesting code modifications:

- Prefer extending existing systems rather than introducing new managers.
- Keep gameplay logic within appropriate actor classes.
- Avoid large structural changes unless necessary.