#  CLECS
## Overview
CLECS (Cute Little ECS) implements an Entity Component System on top of SDL libraries to create a small yet complete framework for developing desktop applications. The solution comes with an [Arkanoid](#rogueanoid) project - a sample game implementation based on an all-time classic.

## Key Concepts
CLECS implements an Archetype-based Entity Component System. It features Commands that are the only way to perform structural changes to Archetypes. Moreover, it allows for querying in both Read and Write mode.

### Modules
CLECS projects are built with Modules. They provide a place to register Components, Systems, and StartupSystems which build the application. CLECS itself is also built on top of a Module architecture and provides Audio and Render modules.

### Components
Components can be attached to entities via Commands. Their values can be retrieved or changed using Queries. They are behaviourless buckets of data that Systems operate on.

### Entities
Entities are unique identifiers. They are not aware of what components are attached to them. They serve as an ID that binds them together. Just like components, they do not have behavior of their own.

### Systems
Systems are stateless and unaware of one another. They are put into Phases which then produce SystemStages. Phases define at which point of the GameLoop Systems should be called. After each stage, structural commands are flushed and executed. Eventually, systems themselves are synchronous and not yet able to work in parallel, so don't worry about read/write conflicts.

#### StartupSystems
StartupSystems differ from regular Systems. They are called only once within application lifetime, at the very beginning after World initialization. One should spawn initial entities and their components in there. As some initialization operations may be complex, there are two types of StartupSystems: Regular and Late. After Regulars are executed, there is a command flush, and then Late ones are called.

## Core Classes

### Column
Storage of specified Component type. They are the lowest-level container where actual values for Components are held. A set of Columns define an Archetype.

### Archetype
Archetype is a unique table for Entities with an exact set of Components. It consists of unique Column sets. No two Archetypes may have exactly the same set of Components; however, one can be a subset of another.

### ArchetypeStorage
Collection of Archetypes. Assigns them unique keys based on their Column sets. It translates Commands sent from Systems to Archetype structural changes. It also provides data for Queries via ArchetypeHandles.

### Query
Query is a templated class that allows Systems to manipulate Component values. They feature Read and Write access. They also implement an ExcludeList templated argument, which enables operating on a subset of Archetypes. Queries will return every matching Component set attached to a single Entity, whether it's only a subset or a full set of them.

### ArchetypeHandle
Means of communication between Queries and Archetypes. Allows for proper iteration over Components.

### CommandRunner
Stores Commands for structural Archetype changes created by Systems. At the end of each SystemStage, they are executed and the next stage begins. Commands encourage batch operations in favour of making changes individually, as those are the most expensive operations in CLECS.

### ModuleBase
Modules are building blocks for CLECS. Module is to be thought of as a package of Components and Systems that are to be included in the game. So one registers Modules, which then register Components and Systems.

### GameRunner
GameRunner manages the lifecycle of a CLECS game. It handles SDL initialization and the main game loop.

### Game
Game is the main entry point for CLECS games. You must inherit from this class to define the entry point of the game.  

### World
World is the heart of CLECS architecture. It owns the SDL and CLECS resources and manages their lifecycle. It coordinates Systems and is reponsible for flushing Commands. It translates SDL events to the InputState.

### InputState
Responsible for input polling. Updated each frame by the World, which translates SDL events to it. Does not forward any events by itself.

## External Libraries
List of external libraries which CLECS utilizes. Note that they have dependencies of their own, which are not listed here:
1. [SDL3](https://wiki.libsdl.org/SDL3/FrontPage)
2. [SDL3_image](https://wiki.libsdl.org/SDL3_image/FrontPage)
3. [SDL3_ttf](https://wiki.libsdl.org/SDL3_ttf/FrontPage)
4. [nlohmann/json](https://github.com/nlohmann/json)

## Rogueanoid
Rogueanoid is, you guessed it, Arkanoid with roguelike elements. In addition to classic brick-breaking, the player can choose from numerous upgrades that alter their gameplay every run. It follows CLECS principles of development, and therefore ECS too. No system knows about any other, and doesn't contain state. Also, components are PODs without any behavior of their own.

Each stage and modifier is defined in a JSON file, so you can play with values to your heart's content. Well, not entirely, as there is no data validation. Yet, feel more than welcome to try!