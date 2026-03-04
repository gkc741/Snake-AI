#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "train.h"
#include "gui.h"



layer create_empty_layer(int nr_of_neurons, int nr_of_inputs){
    layer n;
    n.nr_of_neurons = nr_of_neurons;
    n.in_size = nr_of_inputs;

    n.bias = malloc(nr_of_neurons * sizeof(float));

    n.weights = malloc(nr_of_inputs * nr_of_neurons * sizeof(float));

    return n;
}


void free_layer(layer layer){
    free(layer.bias);
    free(layer.weights);
}

int snake_forward_pass(float* input, layer* hidden_layer, layer* output_layer){
    float hidden_out[HIDDEN_SIZE];
    float output_out[OUTPUT_SIZE];

    float temp;
    for(int j = 0; j < hidden_layer->nr_of_neurons; j++){
        temp = hidden_layer->bias[j];
        for(int i = 0; i < hidden_layer->in_size; i++){
            temp += hidden_layer->weights[j * hidden_layer->in_size + i] * input[i];
        }
        hidden_out[j] = temp > 0 ? temp : 0;
    }
    
    // Output layer
    for(int i = 0; i < output_layer->nr_of_neurons; i++){
        temp = output_layer->bias[i];
        for(int j = 0; j < output_layer->in_size; j++){
            temp += output_layer->weights[i * output_layer->in_size + j] * hidden_out[j];
        }
        output_out[i] = temp;
    }

    int direction = 0;
    float max_logit = output_out[0];
    for(int i = 0; i < OUTPUT_SIZE; i++){
        if(output_out[i] > max_logit){
            max_logit = output_out[i];
            direction = i;
        }
    }
    return direction + 1;
}

int directions[8][2] = {
    { 0, -1},  // N
    { 1, -1},  // NE
    { 1,  0},  // E
    { 1,  1},  // SE
    { 0,  1},  // S
    {-1,  1},  // SW
    {-1,  0},  // W
    {-1, -1},  // NW
};

void get_sensors(SnakeGame* game, float* sensors){

    for(int i = 0; i < 8; i++){
        int dir_x = directions[i][0];
        int dir_y = directions[i][1];

        int x = game->head.x;
        int y = game->head.y;
        float steps = 0.0f;
        float food_found = 0.0f;
        float self_found = 0.0f;

        while(x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT){
            x += dir_x;
            y += dir_y;
            steps++;

            // Check the food
            if(x == game->food.x && y == game->food.y){
                food_found = 1.0f;
            }

            // Check the body
            for(int j = 0; j < game->snake_size; j++){
                if(x == game->body[j].x && y == game->body[j].y){
                    self_found = 1.0f;
                }
            }

            // wall check is automatic with steps++
        }
        sensors[i * 3 + 0] = 1.0f / steps;  // small value is far away wall, small value is close wall
        sensors[i * 3 + 1] = food_found;
        sensors[i * 3 + 2] = self_found;
    }
}


Genome create_random_genome(){
    Genome g;
    for(int i = 0; i < GENOME_SIZE; i++){
        g.weights[i] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
    }
    g.fitness = 0.0f;
    return g;
}


void genome_to_layers(Genome* g, layer* hidden, layer* output){
    int idx = 0;

    // it goes hidden weights, bias and then output weights and bias
    for(int i = 0; i < INPUT_SIZE * HIDDEN_SIZE; i++){
        hidden->weights[i] = g->weights[idx++];
    }

    for(int i = 0; i < HIDDEN_SIZE; i++){
        hidden->bias[i] = g->weights[idx++];
    }

    for(int i = 0; i < OUTPUT_SIZE * HIDDEN_SIZE; i++){
        output->weights[i] = g->weights[idx++];
    }

    for(int i = 0; i < OUTPUT_SIZE; i++){
        output->bias[i] = g->weights[idx++];
    }
}

// void calc_fitness(SnakeGame* game, Genome* g){
//     g->fitness = game->score * game->snake_size + game->steps_alive;
// }

void calc_fitness(SnakeGame* game, Genome* g){
    if(game->score < 10){
        g->fitness = game->steps_alive * pow(2, game->score);
    } else {
        g->fitness = game->steps_alive * pow(2, 10) * (game->score - 9);
    }
}

void simulate_step(SnakeGame* game, layer* hidden, layer* output, float* sensors, int* starvation_steps, int* previous_score){
    get_sensors(game, sensors);
    game->nextDirection = snake_forward_pass(sensors, hidden, output);
    update_game_with_walls(game);

    if(game->score > *previous_score){
        *previous_score = game->score;
        *starvation_steps = 0;
    } else {
        (*starvation_steps)++;
    }

    if(*starvation_steps > 50 + game->snake_size * 20){
        game->lost = 1;
    }
}


void simulate(SnakeGame* game, Genome* g, layer* hidden, layer* ouptut, float* sensors){
    start_game(game);

    genome_to_layers(g, hidden, ouptut);

    int starvation_steps = 0;
    int previous_score = game->score;

    while(!game->lost && !game->won && game->steps_alive < 200 + game->snake_size * 100){
        simulate_step(game, hidden, ouptut, sensors, &starvation_steps, &previous_score);

    } 
    calc_fitness(game, g);
}

int comp(const void *a, const void *b) {
    Genome *g1 = (Genome *)a;
    Genome *g2 = (Genome *)b;

    if(g2->fitness > g1->fitness) return 1;
    if(g2->fitness < g1->fitness) return -1;
    return 0;
}

void sort_by_fitness(Genome* population){
    qsort(population, POP_SIZE, sizeof(Genome), comp);
}


Genome breed_child(Genome population[], int retention_percent){
    Genome offspring;
    int elite_count = POP_SIZE / retention_percent;
    if(elite_count < 2) elite_count = 2;
    int prt1_inx = rand() % elite_count;
    int prt2_inx = rand() % elite_count;
    while(prt2_inx == prt1_inx){
        prt2_inx = rand() % elite_count;
    }
    Genome parent_1 = population[prt1_inx];
    Genome parent_2 = population[prt2_inx];
    for(int i = 0; i < GENOME_SIZE; i++){
        if(rand() % 2 == 0){
            offspring.weights[i] = parent_1.weights[i];
        }
        else{
            offspring.weights[i] = parent_2.weights[i]; 
        }
    }
    offspring.fitness = 0.0f;
    return offspring;
}

void mutate(Genome* offspring, float mutation_rate, float mutation_strength){
    // This and breed_child wont work properly because i treat weigths and bias the same but its ok for now
    // TODO: fix that

    for(int i = 0; i < GENOME_SIZE; i++){
        if((rand() / (float)RAND_MAX) < mutation_rate){
            offspring->weights[i] += ((rand() / (float)RAND_MAX) * 2.0f - 1.0f) * mutation_strength;
        }
        if(offspring->weights[i] > 1.0f) offspring->weights[i] = 1.0f;
        if(offspring->weights[i] < -1.0f) offspring->weights[i] = -1.0f;
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