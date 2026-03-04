# Snake Game with Neuroevolution AI

A Snake game implemented in C using raylib with an AI agent trained through neuroevolution.

## Overview

This project combines two main components:

- **Game**: A classic Snake game rendered with raylib
- **AI Agent**: Trained using neuroevolution (genetic algorithms with fixed-topology neural network)

The AI learns to play Snake by evolving neural network weights across generations. It uses 8-directional sensors to perceive the game state and makes decisions through a small feedforward network.

## Project Structure

```
Snake/
├── train.c / train.h       # Neuroevolution algorithm and neural network
├── gui.c / gui.h            # Raylib game rendering and logic
├── main_train.c             # Training loop for the AI agent
├── main_play.c              # Play with trained agent or human control
├── Makefile                 # Build configuration
└── README.md                # This file
```

## How It Works

### Neuroevolution Algorithm

1. **Initialization**: Create a population of random neural networks
2. **Fitness Evaluation**: Each network plays Snake and receives a fitness score
3. **Selection**: Best performers are selected for breeding
4. **Crossover & Mutation**: Create offspring from parent networks
5. **Repeat**: New generation replaces old population

### Neural Network

- **Input**: 24 sensors (8 directions × 3 values per direction)
  - Distance to wall
  - Food detection
  - Body collision detection
- **Hidden Layer**: 32 neurons with ReLU activation
- **Output**: 4 neurons (up, right, down, left)

### Fitness Function

Rewards both eating food and survival time:

$$\text{fitness} = \begin{cases}
t \cdot 2^s & \text{if } s < 10 \\
t \cdot 2^{10} \cdot (s - 9) & \text{if } s \geq 10
\end{cases}$$

where:
- $t$ = steps alive
- $s$ = score

## Results

The agent learns to:

- Navigate towards food
- Avoid walls and its own body
- Play increasingly longer games

## Technologies

- **Language**: C
- **Graphics**: Raylib
- **ML**: Genetic Algorithm, Neuroevolution

## Future Improvements

- Implement NEAT (topological evolution)
- Add speciation for better diversity
- Improve fitness function for corner navigation
- Save/load trained genomes
