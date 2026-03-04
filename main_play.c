#include "raylib.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gui.h"





int main(){
    srand(time(NULL));
    unsigned int seed = time(NULL);
    float timer = 0.0f;


    SnakeGame game;
    game.body = malloc(MAX_SNAKE * sizeof(Position));
    start_game(&game, &seed);

    Rectangle* possible_poses = GetBoxes();


    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake game");

    SetTargetFPS(60);


    while(!WindowShouldClose()){

        float dt = GetFrameTime();
        timer += dt;

        handle_inputs(&game, &seed);

        if (!game.lost && !game.won && timer >= (1 / game.move_speed)){
            update_game(&game);
            timer = 0.0f;
        }

        
        BeginDrawing();
        draw_game(&game);

        EndDrawing();

    }

    


    free(possible_poses);
    free(game.body);

    CloseWindow();
    return 0;
}


