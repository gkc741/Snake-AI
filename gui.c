#include "raylib.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gui.h"
#include "train.h"





Rectangle grid = {0, 0, SCREEN_WIDTH, CANVAS_SIZE * GRID_SIZE};
Rectangle bottom = {0, GRID_HEIGHT * GRID_SIZE, SCREEN_WIDTH, BOTTOM_HEIGHT};

Rectangle* GetBoxes(){
    Rectangle* pos = malloc(sizeof(*pos) * GRID_HEIGHT * GRID_WIDTH);

    int posX;
    int posY;
    int width;
    int heigth;
    for(int i = 0; i < GRID_HEIGHT; i++){
        for (int j = 0; j < GRID_WIDTH; j++){
            posX = j * GRID_SIZE;
            posY = i * GRID_SIZE;
            width = GRID_SIZE;
            heigth = GRID_SIZE;
            Rectangle temp = {posX, posY, width, heigth};
            pos[i * GRID_WIDTH + j] = temp;
        }
    }
    return pos;
}


void start_game(SnakeGame* game){
    memset(game->body, -1, MAX_SNAKE * sizeof(Position));
    game->head = (Position){5 + rand() % 20, 5 + rand() % 10};
    game->direction = 1 + rand() % 4;
    game->nextDirection = game->direction;
    game->snake_size = 3;
    game->lost = 0;
    game->won = 0;
    game->score = 0;
    game->steps_alive = 0;
    game->move_speed = 5;
    game->food = (Position){rand() % GRID_WIDTH, rand() % GRID_HEIGHT};
}

// if(size_of_list < length)
// append and size++
// else
// append og remove last element

bool add_head(int x, int y, int* snakeSize, Position* snake_list){
    for(int i = (*snakeSize); i > 0; i--){
        snake_list[i] = snake_list[i-1];  // moves every item one step to the right
        if(snake_list[i].x == x && snake_list[i].y == y){
            return 1;
        }
    }
    snake_list[0] = (Position){x, y};
    return 0;
}



void update_game(SnakeGame* game){
    game->direction = game->nextDirection;
    game->steps_alive++;
    
    switch(game->direction){
        case 1:
            game->lost = add_head(game->head.x, game->head.y, &game->snake_size, game->body);
            if(!game->lost){
                if(game->head.y <= 0) game->head.y = GRID_HEIGHT - 1;
                else game->head.y--;
            }
            break;

        case 2:
            game->lost = add_head(game->head.x, game->head.y, &game->snake_size, game->body);
            if(!game->lost) game->head.x = (game->head.x + 1) % GRID_WIDTH;
            break;

        case 3:
            game->lost = add_head(game->head.x, game->head.y, &game->snake_size, game->body);
            if(!game->lost) game->head.y = (game->head.y + 1) % GRID_HEIGHT;
            break;

        case 4:
            game->lost = add_head(game->head.x, game->head.y, &game->snake_size, game->body);
            if(!game->lost){
                if(game->head.x <= 0) game->head.x = GRID_WIDTH - 1;
                else game->head.x--;
            }
            break;
    }
    // FOOD
    bool valid_food = 0;

    if(game->head.x == game->food.x && game->head.y == game->food.y){
        game->snake_size++;
        game->move_speed *= 1.04f;
        game->score++;
        if(game->move_speed > MAX_SPEED){
            game->move_speed = MAX_SPEED;
        }
        valid_food = 0;
        while(!valid_food){
            game->food = (Position){rand() % GRID_WIDTH, rand() % GRID_HEIGHT};
            valid_food = 1;
            for(int i = 0; i < game->snake_size; i++){
                if(game->body[i].x == game->food.x && game->body[i].y == game->food.y){
                    valid_food = 0;
                    break;
                }
            }
        }
    }
    if(game->snake_size == MAX_SNAKE){
            game->won = 1;
    }
}



void update_game_with_walls(SnakeGame* game){
    game->direction = game->nextDirection;
    game->steps_alive++;
    
    switch(game->direction){
        case 1:  // UP
            game->lost = add_head(game->head.x, game->head.y, &game->snake_size, game->body);
            if(!game->lost){
                game->head.y--;
                if(game->head.y < 0) game->lost = 1;  // Hit top wall
            }
            break;

        case 2:  // RIGHT
            game->lost = add_head(game->head.x, game->head.y, &game->snake_size, game->body);
            if(!game->lost){
                game->head.x++;
                if(game->head.x >= GRID_WIDTH) game->lost = 1;  // Hit right wall
            }
            break;

        case 3:  // DOWN
            game->lost = add_head(game->head.x, game->head.y, &game->snake_size, game->body);
            if(!game->lost){
                game->head.y++;
                if(game->head.y >= GRID_HEIGHT) game->lost = 1;  // Hit bottom wall
            }
            break;

        case 4:  // LEFT
            game->lost = add_head(game->head.x, game->head.y, &game->snake_size, game->body);
            if(!game->lost){
                game->head.x--;
                if(game->head.x < 0) game->lost = 1;  // Hit left wall
            }
            break;
    }
    
    // FOOD
    bool valid_food = 0;

    if(game->head.x == game->food.x && game->head.y == game->food.y){
        game->snake_size++;
        game->move_speed *= 1.04f;
        game->score++;
        if(game->move_speed > MAX_SPEED){
            game->move_speed = MAX_SPEED;
        }
        valid_food = 0;
        while(!valid_food){
            game->food = (Position){rand() % GRID_WIDTH, rand() % GRID_HEIGHT};
            valid_food = 1;
            for(int i = 0; i < game->snake_size; i++){
                if(game->body[i].x == game->food.x && game->body[i].y == game->food.y){
                    valid_food = 0;
                    break;
                }
            }
        }
    }
    if(game->snake_size == MAX_SNAKE){
            game->won = 1;
    }
}





void DrawCenteredText(char* text, int size, Color color){
    int text_width = MeasureText(text, size);

    int x = SCREEN_WIDTH/2 - text_width/2;
    int y = (GRID_HEIGHT * GRID_SIZE)/2 - size/2;

    DrawText(text, x, y, size, color);
}


void draw_game(SnakeGame* game){
    ClearBackground(BLACK);
    
    DrawRectangleRec(grid, LIME);
    DrawRectangleRec(bottom, BROWN);

    
    // Draw the snake
    DrawRectangle(game->head.x * GRID_SIZE, game->head.y * GRID_SIZE, GRID_SIZE, GRID_SIZE, YELLOW);
    for(int i = 0; i < game->snake_size; i++){
        int posX = game->body[i].x * GRID_SIZE;
        int posY = game->body[i].y * GRID_SIZE;
        DrawRectangle(posX, posY, GRID_SIZE, GRID_SIZE, ORANGE);
    }

    // Food
    DrawRectangle(game->food.x * GRID_SIZE, game->food.y * GRID_SIZE, GRID_SIZE, GRID_SIZE, GREEN);

    // Grid lines
    for(int i = 0; i <= GRID_WIDTH; i++){
        DrawLine(i * GRID_SIZE, 0, i * GRID_SIZE, CANVAS_SIZE, (Color){255, 255, 255, 70});
    }
    for(int i = 0; i <= GRID_HEIGHT; i++){
        DrawLine(0, i * GRID_SIZE, SCREEN_WIDTH, i * GRID_SIZE, (Color){255, 255, 255, 70});
    }

    // Bottom
    DrawText("Welcome to Snake!\nMove with arrow keys | Press R to restart", bottom.x + 10, bottom.y + 10, 30, WHITE);
    DrawText(TextFormat("Fruits Collected: %d | Speed: %.2f", game->score, game->move_speed), bottom.x + 10, bottom.y + 80, 25, WHITE);

    // Win/Lose
    if(game->won){ 
        DrawCenteredText("YOU WIN", 120, RED);
    }
    if(game->lost){
        DrawCenteredText("YOU LOOSE!", 120, RED);
    }
}

void handle_inputs(SnakeGame* game){
    // UP
    if(IsKeyPressed(KEY_UP) && game->direction != 3){
        game->nextDirection = 1;
    }
    // RIGHT
    if(IsKeyPressed(KEY_RIGHT) && game->direction != 4){
        game->nextDirection = 2;
    }
    // DOWN
    if(IsKeyPressed(KEY_DOWN) && game->direction != 1){
        game->nextDirection = 3;
    }
    // LEFT
    if(IsKeyPressed(KEY_LEFT) && game->direction != 2){
        game->nextDirection = 4;
    }

    // Reset
    if(IsKeyPressed(KEY_R)){
        start_game(game);
    }
}


void simulate_with_gui(SnakeGame* game, Genome* g, layer* hidden, layer* output, float* sensors){
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Best Snake");
    SetTargetFPS(30);

    start_game(game);
    genome_to_layers(g, hidden, output);

    int starvation_steps = 0;
    int previous_score = game->score;

     while(!WindowShouldClose() && !game->lost && !game->won && game->steps_alive < 200 + game->snake_size * 100){
        simulate_step(game, hidden, output, sensors, &starvation_steps, &previous_score);

        BeginDrawing();
        ClearBackground(BLACK);
        draw_game(game);
        EndDrawing();
    }
    
    calc_fitness(game, g);
    CloseWindow();
}