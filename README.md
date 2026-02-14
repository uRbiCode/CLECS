# CLECS
## Overview
CLECS (Cute Little ECS) implements an Entity Component System on top of SDL libraries to create a small, yet a complete synchronous framework for developing desktop applications. The solution comes with an [Arkanoid](#rogueanoid) project - a sample game implementation based on an all-time classic.

## Key Concepts
CLECS implements Entity Component System based on [EnTT](https://github.com/skypjack/entt) approach to the subject.

### Components
For components, it utilizes sparse sets for highly performative operations and implements a Group pattern for template-based querying. Components are stored in contiguous memory blocks to encourage a cache-friendly approach. Components are behaviorless, they are simply buckets of data that Systems operate on.

### Entities
Entities are unique identifiers with versioning. They are not aware of what components are attached to them. They serve as an ID that binds them together. Just like components, they do not have behavior of their own.

### Systems
Systems are stateless and unaware of one another. They are able to communicate with each other using synchronized Events.

## Core Classes

### EntityAdmin
EntityAdmin handles entity lifecycle and component storage. It manages Ids of entities with versioning to prevent stale references. It also stores ComponentPools for efficient access and iteration.

### ComponentPool
Templated component pool utilizing sparse set.

### System
It represents a piece of logic that operates on entities that have specific components attached to them. Systems are to be stateless, and they should not store any data themselves. Instead, they should operate on the data stored in components.

### GameRunner
GameRunner manages the lifecycle of a CLECS game. It handles SDL initialization and the main game loop.

### Game
Game is the main entry point for CLECS games. You must inherit from this class and use `CLECS_DEFINE_GAME_ENTRY` macro to define the entry point of the game.	

### World
World is the heart of CLECS architecture. It owns the SDL and CLECS resources and manages their lifecycle. It coordinates systems and provides access to entity management to them. It translates SDL events to the InputState.

### EventBus
Responsible for managing event subscriptions and notifications in CLECS. Systems may subscribe to events by type, and the EventBus will notify them when events of that type are emitted. Notifications are synchronous and happen immediately. This means that an event sent in response to another will be processed first.

### InputState
Responsible for input polling. Updated each frame by the World, which translates SDL events to it. Does not forward any events by itself.

## External Libraries
List of external libraries which CLECS utilizes. Note, that they have dependencies of their own, which are not listed here:
1. [SDL3](https://wiki.libsdl.org/SDL3/FrontPage)
2. [SDL3_image](https://wiki.libsdl.org/SDL3_image/FrontPage)
3. [SDL3_ttf](https://wiki.libsdl.org/SDL3_ttf/FrontPage)
4. [nlohmann/json](https://github.com/nlohmann/json)

## Rogueanoid
Rogueanoid is, you guessed it, Arkanoid with roguelike elements. In addition to classic brick-breaking, player can choose from numerous upgrades that alter their gameplay every run. It follows CLECS principles of development, and therefore ECS too. No system knows about any other, and doesn't contain state. Also, components are PODs without any behavior of their own.

Each stage and modifier is defined in a JSON file, so you can play with values to your heart's content. Well, not entirely, as there is no data validation. Yet, feel more than welcome to try!