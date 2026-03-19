# Adaptive Deadlock Management System

A deadlock detection and prevention simulator built in C, with a browser-based visual interface. Implements Banker's Algorithm, Resource Allocation Graph (RAG), DFS cycle detection, dynamic Max update for runtime processes, and periodic deadlock sweeps — all in an adaptive system that automatically switches strategy based on resource instance counts.

---

## What it does

The system simulates how an OS manages resources across multiple processes and handles deadlock scenarios. It is **adaptive** — it reads the resource configuration at startup and decides which algorithm to use:

- **Single-Instance Mode** — all resources have exactly 1 copy → RAG + DFS is primary. A cycle in the graph means deadlock is confirmed.
- **Multi-Instance Mode** — any resource has more than 1 copy → Banker's Algorithm is primary. A RAG cycle is a warning only, not a guarantee.

---

## Features

| # | Feature | Description |
|---|---------|-------------|
| 1 | System Initialization | Input processes, resources, total instances, allocation matrix, max matrix. Need and Available auto-calculated. Mode auto-detected. |
| 2 | Display System State | Print Allocation, Max, Need, Available in tabular format |
| 3 | Banker's Algorithm | Safe state check. Prints safe sequence if exists, unsafe state if not |
| 4 | Resource Allocation Graph | Build adjacency matrix, print graph and edge list |
| 5 | Deadlock Detection | DFS cycle detection on RAG. Output adapts based on single vs multi-instance mode |
| 6 | Resource Request Simulation | Validate request, temporarily allocate, run Banker's, confirm or rollback |
| 6b | Dynamic Max Update | If request exceeds declared Need, attempt runtime Max update and re-validate |
| 7 | Random Simulation | N rounds of random requests, each validated against Banker's Algorithm |
| 7b | Periodic Sweep | Every 3 confirmed allocations, auto-run RAG + DFS + Banker's to catch cumulative drift |

---

## Project USP

Unlike traditional deadlock simulators, this system:

- Dynamically switches between detection and avoidance strategies
- Supports runtime Max updates (real-world process behavior)
- Includes periodic safety sweeps to detect cumulative risk
- Provides both algorithmic correctness (Banker’s) and structural insight (RAG)

This makes it closer to real OS behavior than textbook implementations.

---

## Screenshots

### System Initialization
<img width="700" alt="System Initialization" src="https://github.com/user-attachments/assets/70acbe49-2929-4670-801f-7d0f07bba466" />

### System State
<img width="700" alt="System State" src="https://github.com/user-attachments/assets/26378396-5a4d-4fba-9bdb-b05b9543a663" />

### Banker's Algorithm - Safe Check
<img width="700" alt="Banker's Algorithm" src="https://github.com/user-attachments/assets/45718d16-0ef0-4c25-8aa0-a5bc868ea22d" />

### RAG
<img width="700" alt="RAG" src="https://github.com/user-attachments/assets/a3379bb0-a251-4dc9-be4a-1e66b935a90a" />

### Simulation
<img width="700" alt="Simulation" src="https://github.com/user-attachments/assets/0bd1b89f-25d0-45e4-b772-07d491068a94" />

---

## Project Structure

```
adaptive-deadlock-management-system/
│
├── backend/                          C backend engine
│   ├── types.h                       Shared structs, constants, SystemState definition
│   ├── main.c                        Global state definition, menu loop
│   ├── init.h / init.c               System initialization, Need calculation, mode detection
│   ├── display.h / display.c         Matrix display in tabular format
│   ├── banker.h / banker.c           Banker's Algorithm — is_safe(), run_banker()
│   ├── rag.h / rag.c                 RAG construction and printing
│   ├── deadlock.h / deadlock.c       DFS cycle detection
│   ├── request.h / request.c         Resource request + dynamic Max update
│   └── simulation.h / simulation.c   Random simulation + periodic sweep
│
├── frontend/                         Browser-based visual interface
│   ├── index.html                    UI structure
│   ├── style.css                     Green-on-black terminal styling
│   └── app.js                        All algorithms reimplemented in JavaScript
│
└── README.md
```
---

## C Backend — How to Run

**Compile:**
```bash
gcc -o deadlock_sim main.c init.c display.c banker.c rag.c deadlock.c request.c simulation.c
```

**Run:**
```bash
./deadlock_sim
```

**Menu:**
```
1. Initialise System
2. Display System State
3. Run Banker's Algorithm
4. Show Resource Allocation Graph
5. Detect Deadlock (DFS Cycle Check)
6. Request Resource
7. Run Random Simulation
8. Exit
```

---

## Frontend UI — How to Open

No server needed. Just open `index.html` directly in any browser.

```
File Explorer → right click index.html → Open with Chrome
```

Or drag `index.html` into a browser window. The address bar will show `file:///...` — that is correct.

**Features in the UI:**
- Boot sequence animation on load
- Tab-based interface mapping to all 7 features
- Block visualizer — filled blocks for allocated resources, dim blocks for needed resources
- Animated safe sequence — process nodes light up in execution order
- RAG drawn on canvas — circles for processes, diamonds for resources, directed arrows for edges
- Green-on-black terminal output with colour-coded results (green = safe, red = denied, yellow = warning)
- Status bar showing current mode, allocation count, and sweep count

---

## Sample Input for Testing

```
Processes  : 3
Resources  : 3
Total      : R0=10  R1=5  R2=7

Allocation:        Max:
     R0 R1 R2          R0 R1 R2
P0 [  0  1  0 ]   P0 [  7  5  3 ]
P1 [  2  0  0 ]   P1 [  3  2  2 ]
P2 [  3  0  2 ]   P2 [  9  0  2 ]

Auto-calculated:
Need:              Available:
     R0 R1 R2          R0 R1 R2
P0 [  7  4  3 ]   [  5  4  5 ]
P1 [  1  2  2 ]
P2 [  6  0  0 ]

Expected safe sequence: P1 → P2 → P0
```

---

## Algorithms

**Banker's Algorithm (Safety Check)**
Simulates finishing all processes using a copy of the available vector. Greedy within each pass — picks up all runnable processes in a single scan before restarting. Returns safe sequence if all processes can finish, unsafe otherwise.

**DFS Cycle Detection**
Uses `visited[]` and `rec_stack[]` arrays. A back edge — reaching a node whose `rec_stack` entry is still 1 — indicates a cycle. DFS launches from every unvisited node to handle disconnected graphs.

**Dynamic Max Update**
When a process requests beyond its declared Need, the system saves the original Max and Need, updates them to accommodate the request, temporarily allocates, and runs Banker's. Confirms if safe, rolls back entirely if unsafe.

**Periodic Sweep**
Fires every `SWEEP_INTERVAL` (default: 3) confirmed allocations. Rebuilds RAG, runs DFS, and in multi-instance mode also runs Banker's. Catches cumulative unsafe drift that per-request validation alone might miss.

---

## Tech Stack

| Layer | Technology |
|-------|-----------|
| Backend engine | C (C99) |
| Frontend structure | HTML5 |
| Frontend styling | CSS3 with CSS variables |
| Frontend logic | Vanilla JavaScript (ES6) |
| Fonts | VT323, Share Tech Mono (Google Fonts) |

---

## Concepts Demonstrated

- Deadlock conditions (Coffman's four conditions)
- Deadlock avoidance vs detection vs prevention
- Banker's Algorithm and safe state theory
- Resource Allocation Graph construction and traversal
- DFS with recursion stack for cycle detection
- Rollback and transactional state management
- Adaptive algorithm selection based on system configuration
- Dynamic process handling with runtime constraint updates

---

## TL;DR

> *The system dynamically adapts between deadlock detection and prevention strategies. It uses Banker's Algorithm for accurate safety checking in multi-instance systems, extends it with runtime Max updates for dynamic processes, and uses RAG with DFS both for visualization in single-instance scenarios and as a periodic detection sweep to catch cumulative drift that individual safe allocations might cause.*