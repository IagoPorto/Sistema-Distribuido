# 🎟️ Distributed Event Ticketing System

This is a **Distributed System** designed to manage ticket sales for events. It leverages **Ricart-Agrawala's algorithm** for distributed mutual exclusion, ensuring consistency and coordination across nodes.

---

## 🌟 Features

- **Distributed Architecture**: Fully distributed ticketing system with no central point of failure.
- **Mutual Exclusion**: Utilizes the **Ricart-Agrawala algorithm with tickets** for efficient resource locking.
- **Token-Based Coordination**: Implements the **Ricart-Agrawala token-passing algorithm** for ensuring ordered access to shared resources.

---

## 🛠️ Tech Stack

- **Programming Language**: C
- **Distributed Algorithm**: Ricart-Agrawala
- **Networking**: Sockets for inter-node communication
- **Synchronization**: Message passing for coordination

---

## 📦 How It Works

1. **Ticket-Based Algorithm**:
   - Each node requests access to a critical section by broadcasting a timestamped request.
   - Nodes reply to requests based on the timestamp order, ensuring fairness.

2. **Token-Based Algorithm**:
   - A unique token circulates among nodes, granting access to the critical section.
   - Nodes pass the token after completing their operations, ensuring an orderly workflow.

---
