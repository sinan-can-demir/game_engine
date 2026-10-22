# Game Engine — Requirements, Scope & Milestones

## Context

A ground-up game engine written in **pure C (C11)** for the purpose of learning low-level programming:
OS primitives, manual memory management, hardware communication, and software engineering discipline.
The project starts from an **empty directory**. No existing code to port.

Key design decisions locked before any code:
- Language: **C11** (no C++, no OOP overhead, no exceptions, no RTTI)
- Renderer: **swappable via interface** (start with software rasterizer, plug in OpenGL later)
- Focus: **OS/hardware layer first** — memory, syscalls, timer, platform abstraction — then build upward
- Principle: **increment by subsystem**, each milestone produces a runnable executable

---

## Actors

| Actor | Role | Interacts With |
|---|---|---|
| **Engine Developer** | Builds and maintains engine subsystems (you, the learner) | All subsystems |
| **Game Developer** | Uses the engine API to build a game | Public engine API headers |
| **Player** | Runs a game built on the engine | Game executable only |
| **Debug User** | Inspects live engine state during development (often same as Engine Dev) | Debug overlay, console, logger |

---

## Use Cases

| ID | Actor | Use Case | Description |
|---|---|---|---|
| UC-01 | Engine Dev | Initialize platform | Boot the engine: allocate memory arenas, open timer, query CPU info |
| UC-02 | Engine Dev | Shutdown platform | Tear down all subsystems in reverse dependency order, free memory |
| UC-03 | Game Dev | Open a window | Create an OS window with specified dimensions and title |
| UC-04 | Game Dev | Poll input | Read keyboard, mouse state into a canonical `InputState` each tick |
| UC-05 | Game Dev | Run game loop | Execute fixed-timestep simulate → render cycle at target tick rate |
| UC-06 | Game Dev | Load asset | Request a texture/sound/data file by path; engine returns a handle |
| UC-07 | Game Dev | Create entity | Allocate a unique entity ID from the ECS |
| UC-08 | Game Dev | Attach component | Bind typed data (Position, Sprite, Velocity…) to an entity ID |
| UC-09 | Game Dev | Query entities | Iterate all entities that possess a given set of components |
| UC-10 | Game Dev | Render frame | Submit draw calls to the renderer interface; backend executes them |
| UC-11 | Game Dev | Spatial query | Ask "which entities are within radius R of point P?" |
| UC-12 | Game Dev | Play audio | Trigger a sound asset on a channel with volume/pan |
| UC-13 | Debug User | Log message | Emit a leveled log entry (DEBUG/INFO/WARN/ERROR) with timestamp |
| UC-14 | Debug User | Inspect entity | View all components attached to a live entity in an overlay |
| UC-15 | Engine Dev | Swap renderer | Replace the renderer backend (software → OpenGL) without touching game code |
| UC-16 | Engine Dev | Profile tick | Measure time spent per subsystem per tick; display in overlay |

---

## Functional Requirements

| ID | Requirement | Priority | Use Case |
|---|---|---|---|
| FR-01 | Engine shall provide a **platform abstraction layer** (PAL) that wraps all OS calls (memory, file, time, thread) | Must | UC-01, UC-02 |
| FR-02 | PAL shall expose a **high-resolution monotonic timer** (nanosecond precision) via `plat_time_ns()` | Must | UC-05 |
| FR-03 | Engine shall implement a **fixed-timestep game loop** with a configurable tick rate (default 60 Hz) and a Gaffer-style accumulator | Must | UC-05 |
| FR-04 | Engine shall provide an **arena allocator** for per-frame scratch memory, reset each tick | Must | UC-01 |
| FR-05 | Engine shall provide a **pool allocator** for fixed-size object slots (entity records, component arrays) | Must | UC-01 |
| FR-06 | Engine shall define a **renderer interface** (`ren_clear`, `ren_draw_sprite`, `ren_present`, `ren_swap`) as a function-pointer vtable | Must | UC-10, UC-15 |
| FR-07 | Engine shall ship a **software renderer backend** (CPU rasterizer writing to a pixel buffer) as the first concrete implementation of the renderer interface | Should | UC-10 |
| FR-08 | Engine shall implement an **ECS** with: entity ID pool, per-component sparse-set storage, and system iteration over entity subsets | Must | UC-07, UC-08, UC-09 |
| FR-09 | Engine shall implement a **resource/asset manager** that loads files, caches them by path, and hands out opaque handles (not raw pointers) | Must | UC-06 |
| FR-10 | Engine shall implement an **input system** that polls the OS each frame and exposes a snapshot `InputState` struct (no direct OS calls from game code) | Must | UC-04 |
| FR-11 | Engine shall implement a **spatial hash** to answer "entities within radius R" queries in sub-linear time | Should | UC-11 |
| FR-12 | Engine shall implement an **event/message bus** — a typed queue drained once per tick; systems communicate via events, not direct calls | Should | UC-05 |
| FR-13 | Engine shall implement an **audio system stub** (load WAV, play on channel, stop) backed initially by a no-op backend | Should | UC-12 |
| FR-14 | Engine shall provide a **structured logger** with DEBUG/INFO/WARN/ERROR levels, timestamps, and file:line tags | Must | UC-13 |
| FR-15 | Engine shall provide an **on-screen debug overlay** showing FPS, tick time, entity count, subsystem timings | Should | UC-14, UC-16 |
| FR-16 | Replacing the renderer backend shall require **changing only a single initialization call** (`ren_backend_software_init` → `ren_backend_opengl_init`) | Must | UC-15 |

---

## Non-Functional Requirements

| ID | Requirement |
|---|---|
| NFR-01 | Written in **C11** (gcc/clang, `-std=c11`). No C++ files. |
| NFR-02 | **No runtime dynamic allocation** after engine init except through the engine's own allocators (no stray `malloc` in subsystems). |
| NFR-03 | Each subsystem lives in its **own .h/.c pair**; cross-subsystem includes go through the PAL or a forward-declared handle — no subsystem includes another subsystem's internals. |
| NFR-04 | Every subsystem can be **compiled and unit-tested in isolation** (a `tests/` executable per subsystem with no windowing or GPU required). |
| NFR-05 | Target platforms: **Linux** primary (Fedora), Windows secondary. All OS-specific code confined to PAL files (`platform_linux.c`, `platform_win32.c`). |
| NFR-06 | Build system: **plain Makefile** (no CMake, no build abstraction at the start — understanding the linker is part of the learning goal). |
| NFR-07 | No external dependencies in the core engine. Only the PAL and renderer backends are allowed to link external libraries (SDL2 for windowing, stb_image for texture load). |
| NFR-08 | All public API functions are prefixed by subsystem (`plat_`, `mem_`, `ren_`, `ecs_`, `res_`, `inp_`, `log_`). |

---

## Subsystem Dependency Graph

Arrows mean "depends on / must exist first":

```
┌─────────────────────────────────────────────────────┐
│               PLATFORM LAYER (plat_)                │  ← Layer 0: wraps all OS calls
│  timer · file I/O · memory pages · thread prims     │
└──────────┬──────────────────────────┬───────────────┘
           │                          │
    ┌──────▼──────┐           ┌───────▼────────┐
    │  MATH (m_)  │           │ MEMORY (mem_)  │  ← Layer 1: arena, pool
    │ vec·mat·quat│           │ arena · pool   │
    └──────┬──────┘           └───────┬────────┘
           │                          │
           └──────────┬───────────────┘
                      │
           ┌──────────▼──────────────────────────────────┐
           │           GAME LOOP (loop_)                  │  ← Layer 2: heartbeat
           │  fixed timestep · accumulator · tick counter │
           └──────────┬──────────────────────────────────┘
                      │
       ┌──────────────┼──────────────────┐
       │              │                  │
┌──────▼──────┐ ┌─────▼──────┐  ┌───────▼───────┐
│  INPUT      │ │  LOGGER    │  │  ECS (ecs_)   │  ← Layer 3
│  (inp_)     │ │  (log_)    │  │  id·sparse·sys│
└─────────────┘ └────────────┘  └───────┬───────┘
                                        │
       ┌──────────────┬─────────────────┤
       │              │                 │
┌──────▼──────┐ ┌─────▼──────┐  ┌──────▼──────┐
│  ASSET MGR  │ │  EVENT BUS │  │  SPATIAL    │  ← Layer 4
│  (res_)     │ │  (evt_)    │  │  HASH       │
└──────┬──────┘ └────────────┘  └──────┬──────┘
       │                               │
┌──────▼──────────────────┐   ┌────────▼──────┐
│  RENDERER INTERFACE     │   │  PHYSICS /    │  ← Layer 5
│  (ren_) vtable          │   │  COLLISION    │
│  ├── software backend   │   └──────┬────────┘
│  └── opengl backend     │          │
└─────────────────────────┘   ┌──────▼──────┐
                               │  AI / PATH  │  ← Layer 6
┌─────────────────────────┐   │  FINDING    │
│  AUDIO (aud_)           │   └─────────────┘
│  stub → real backend    │
└─────────────────────────┘
┌─────────────────────────┐
│  DEBUG OVERLAY (dbg_)   │  ← spans all layers (reads, never writes)
└─────────────────────────┘
```

---

## Milestone Table

| # | Milestone | Deliverable (runnable test) | Key subsystems built | Est. effort |
|---|---|---|---|---|
| **M0** | Platform + Memory foundation | `test_plat`: prints timer ticks, allocates from arena, frees pool slot | `plat_`, `mem_` | 3–5 days |
| **M1** | Game loop heartbeat | `test_loop`: runs 300 ticks at 60 Hz, prints tick count and wall time, exits | `loop_`, `log_` | 2–3 days |
| **M2** | Window + Input | Window opens, keyboard/mouse state printed to console each tick, ESC exits | `inp_`, PAL window | 2–3 days |
| **M3** | Renderer interface + software backend | Solid-color window renders at 60 fps via `ren_` vtable backed by CPU pixel buffer | `ren_`, software backend | 1–2 weeks |
| **M4** | Math library | `test_math`: Vec2/Vec3/Mat4/Quat ops pass, camera project/unproject correct | `m_` | 3–4 days |
| **M5** | ECS core | `test_ecs`: 10 000 entities with Position+Velocity, system iterates all, updates in <1 ms | `ecs_` sparse-set | 4–6 days |
| **M6** | Asset manager | Texture loaded once, shared via handle across 1000 sprites, reload-safe | `res_` | 2–3 days |
| **M7** | Event bus | Input events and custom game events flow through `evt_` queue; no direct cross-subsystem calls | `evt_` | 1–2 days |
| **M8** | Spatial hash | `test_spatial`: radius query over 10 000 entities, correct neighbors returned, O(k) | spatial hash | 3–4 days |
| **M9** | Physics / collision | AABB overlap detection, push-out resolution, entity stops at wall | physics | 1–2 weeks |
| **M10** | A\* pathfinding | Entity navigates grid from A to B avoiding obstacles | `ai_` A\* | 1–2 weeks |
| **M11** | Audio stub → real | Sound plays on event; backend swappable (stub → miniaudio) | `aud_` | 2–3 days |
| **M12** | Debug overlay | FPS, tick time, entity count, per-subsystem microseconds shown in-window | `dbg_` | 2–3 days |

**Baseline = M0 + M1**: a runnable loop with a verified platform layer. Every other subsystem builds on this baseline. Nothing else starts until M0 and M1 pass their tests.

---

## Repository Layout (initial skeleton)

```
game_engine/
├── Makefile
├── include/            ← public API headers (plat.h, mem.h, ren.h, ecs.h …)
├── src/
│   ├── platform/
│   │   ├── platform_linux.c
│   │   └── platform_win32.c
│   ├── mem/
│   ├── loop/
│   ├── math/
│   ├── ecs/
│   ├── renderer/
│   │   ├── ren.c          ← interface + dispatch
│   │   ├── ren_software.c ← software backend
│   │   └── ren_opengl.c   ← (deferred)
│   └── …
├── tests/
│   ├── test_plat.c
│   ├── test_mem.c
│   ├── test_ecs.c
│   └── …
└── demos/
    └── demo_heartbeat.c   ← M1 deliverable
```

---

## Architecture Decisions

| Decision | Choice | Rationale |
|---|---|---|
| Language | C11 | Maximum hardware visibility; no runtime behind our back |
| Build system | Makefile | Forces understanding of compilation units, linking, flags |
| Component storage | Sparse-set (not `unordered_map`) | Cache-coherent iteration; one dense array per component type |
| Renderer coupling | vtable of function pointers | C-idiomatic polymorphism; backend swap = reassign struct fields |
| Memory | Arena + pool (no stray malloc) | Deterministic layout; teaches cache line awareness |
| Asset identity | Opaque handle (`uint32_t` index + generation) | Handles survive asset reload; pointers don't |
| Subsystem coupling | Event bus only | Systems never include each other; only `evt.h` |

---

## Verification Plan

Each milestone is considered done when:
1. Its isolated test executable (`tests/test_X`) compiles with `-Wall -Wextra -std=c11` and runs clean.
2. The deliverable described in the Milestone Table is observable (console output or on-screen result).
3. `valgrind --leak-check=full` on the test reports zero leaks (enforced from M0 onward).
4. The Makefile can build from scratch with a single `make`.

Renderer swap test (FR-16 / UC-15): swap `ren_backend_software_init` → future `ren_backend_opengl_init` in `main.c`, recompile, confirm same visual output — no other file changes permitted.
