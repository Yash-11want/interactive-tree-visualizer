# Interactive Tree Visualizer & Contact Management System

A full-stack educational platform that visualizes the internal behavior of Binary Search Trees, AVL Trees, and Red-Black Trees through real-time animations while integrating a practical Contact Management System powered by self-balancing trees.

## Why This Project?

Most students learn tree data structures theoretically. This project bridges the gap between theory and implementation by providing a visual and interactive environment to observe:

- Tree insertion and deletion
- Search path traversal
- AVL rotations and balancing
- Red-Black Tree recoloring and restructuring
- Performance differences between tree variants

Additionally, the project demonstrates a real-world application of balanced trees through a Contact Management System.

---

## Key Highlights

### Interactive Tree Visualizer
- Binary Search Tree (BST)
- AVL Tree
- Red-Black Tree
- Real-time node animations
- Search path visualization
- Dynamic tree rendering

### Contact Management System
- Add, update, search, and delete contacts
- AVL Tree–based indexing
- Efficient O(log n) lookup performance
- Persistent storage support

### Full-Stack Architecture
- Frontend built using HTML, CSS, and JavaScript
- Backend API built with Flask
- Core data structure engines implemented in C++
- JSON-based communication between Python and C++
- SQLite-backed contact storage

---

## System Architecture

```text
User Interface
(HTML/CSS/JavaScript)
          │
          ▼
     Flask Backend
          │
          ▼
   C++ Tree Engines
(BST | AVL | Red-Black)
          │
          ▼
      JSON Output
          │
          ▼
 Interactive Visualization
```

---

## Technical Challenges Solved

### 1. Cross-Language Integration
Integrated Python Flask APIs with C++ data structure engines using subprocess communication and JSON serialization.

### 2. Dynamic Tree Visualization
Developed an automatic tree-layout algorithm to render trees of varying heights while maintaining readability.

### 3. Self-Balancing Tree Operations
Implemented AVL balancing and Red-Black Tree property maintenance while exposing internal balancing operations visually.

### 4. Real-World Application Layer
Applied AVL Trees to build a searchable contact indexing system with efficient lookup performance.

---

## Complexity Analysis

| Operation | BST | AVL | Red-Black |
|-----------|-----|-----|-----------|
| Search | O(h) | O(log n) | O(log n) |
| Insert | O(h) | O(log n) | O(log n) |
| Delete | O(h) | O(log n) | O(log n) |

---

## Tech Stack

### Languages
- C++
- Python
- JavaScript

### Frameworks & Libraries
- Flask

### Database
- SQLite

### Concepts Used
- Object-Oriented Programming
- Data Structures & Algorithms
- Tree Rotations
- Self-Balancing Trees
- REST APIs
- JSON Serialization
- Full-Stack Development

---

## Project Structure

```text
tree-visualizer/
│
├── app.py
├── requirements.txt
│
├── templates/
│   ├── index.html
│   ├── visualizer.html
│   └── contacts.html
│
├── cpp/
│   ├── bst.cpp
│   ├── avl.cpp
│   ├── rb.cpp
│   └── cm.cpp
```

---

## Impact

This project combines:

- Advanced Data Structures
- System Design Thinking
- Backend Development
- Frontend Visualization
- Cross-Language Engineering

making it both an educational tool and a demonstration of software engineering skills.

---

## Future Enhancements

- Traversal animations
- Rotation-by-rotation visualization
- User accounts
- Multi-user support
- Docker deployment
- Performance benchmarking dashboard

---

## Author

Yashwanth Allipuram  
B.Tech, Electrical Engineering  
Indian Institute of Technology Jodhpur

GitHub: https://github.com/Yash-11want
