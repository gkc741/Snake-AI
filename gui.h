#ifndef GUI_H
#define GUI_H

#include "raylib.h"

#define GRID_SIZE 30  // pixels
#define GRID_WIDTH 30  // nr. of grids
#define GRID_HEIGHT 20
#define CANVAS_SIZE (GRID_HEIGHT * GRID_WIDTH)

#define BOTTOM_HEIGHT 200
#define SCREEN_WIDTH GRID_SIZE * GRID_WIDTH
#define SCREEN_HEIGHT GRID_SIZE * GRID_HEIGHT + BOTTOM_HEIGHT

#define MAX_SNAKE GRID_HEIGHT * GRID_WIDTH -1
#define MAX_SPEED 10

typedef struct{
    int x;
    int y;
}Position;

typedef struct {
    Position head;
    Position* body;
    int snake_size;
    Position food;
    int direction;
    int nextDirection;
    bool lost;
    bool won;
    int score;
    int steps_alive;  // needed for fitness later
    float move_speed;
} SnakeGame;




Rectangle* GetBoxes();

void start_game(SnakeGame* game);
bool add_head(int x, int y, int* snakeSize, Position* snake_list);
void update_game(SnakeGame* game);
void update_game_with_walls(SnakeGame* game);

void DrawCenteredText(char* text, int size, Color color);


void draw_game(SnakeGame* game);
void handle_inputs(SnakeGame* game);


#endif
