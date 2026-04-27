# Medieval Combat Game

A third-person, Souls-inspired medieval combat prototype built in **Unreal Engine 5** with **C++**.

![Engine](https://img.shields.io/badge/Unreal_Engine-5.4-313131?logo=unrealengine)
![Language](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=cplusplus&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?logo=windows)
![Status](https://img.shields.io/badge/Status-In%20Development-yellow)

## Overview

A personal portfolio project exploring third-person sword combat, AI behavior, and modular
gameplay systems. The codebase is organized so the same core components (health, stamina,
combat interfaces) drive a Souls-like *Combat* variant alongside lighter *Platforming* and
*Side-Scrolling* variants — letting one C++ framework power three distinct gameplay modes.

> Built from scratch in C++ — no Blueprint logic for combat, AI, or core systems.

## Highlights

**Player**
- Attack combos with anim-notify driven trace damage
- Dodge with invulnerability frames (Elden Ring–style medium roll)
- Stamina cost, regen, and lockouts on attack / dodge / block
- Hit reaction + death animations with ragdoll fallback

**Enemy AI**
- Chase / attack / strafe pacing through `EnemyAIController`
- Hit reaction stagger that cancels in-flight attacks
- Death animation playback with optional ragdoll fallback
- Configurable from data — engineers can ship new enemies without touching code

**Combat framework**
- Modular `UHealthComponent` and `UStaminaComponent` usable by any actor
- `ICombatDamageable` / `ICombatAttacker` interfaces for clean cross-actor messaging
- `AnimNotify_DoAttackTrace`, `AnimNotify_CheckCombo`, `AnimNotify_CheckChargedAttack`

**Inventory & interaction**
- Pickup detection with highlight pulse on focused items
- Equip / unequip pipeline with held-item mesh attachment

**Variants**
- *Combat*: damageable actors, life-bar UI, lava floor hazard, checkpoint volumes
- *Side-Scrolling*: jump pads, moving / soft platforms, NPC AI, on-screen UI
- *Platforming*: dash mechanic with anim-notify cleanup

## Tech

- **Unreal Engine 5.4+**
- **C++** (gameplay code), **C#** (build scripts)
- Animation Blueprints, Anim Notifies, AI Controllers, Behavior / State Trees
- UMG for HUD and life bars

## Project layout

```text
testgame/
├── Config/                    Project .ini configuration
├── Source/testgame/           Gameplay C++ modules
│   ├── Combat/                Health and stamina components
│   ├── Enemy/                 Enemy character + AI controller
│   ├── Inventory/             Pickup, interaction, inventory
│   ├── Variant_Combat/        Souls-like combat: AI, gameplay actors, notifies, UI
│   ├── Variant_Platforming/   Platforming character, dash notify
│   └── Variant_SideScrolling/ Side-scroller character, AI, gameplay actors, UI
└── testgame.uproject
```

The `Content/` folder (assets, animations, third-party marketplace packs) is intentionally
excluded from version control to keep the repository lean and recruiter-friendly.

## Build

1. Right-click `testgame.uproject` → **Generate Visual Studio project files**
2. Open the generated `.sln` in **Visual Studio 2022** (C++ game-development workload)
3. Build the **Development Editor | Win64** target
4. Launch the editor by double-clicking `testgame.uproject`

## Requirements

- Unreal Engine **5.4** or newer
- Visual Studio **2022** with the C++ game-development workload
- Windows 10 / 11

## Author

**Kevin Rusev** — gameplay programming portfolio.
