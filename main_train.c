#include <stdlib.h>
#include <stdio.h>
#include "train.h"
#include "gui.h"
#include "time.h"
#include <omp.h>



int main(){
    srand(time(NULL));

// This will take 24 (8 directions * (distance to wall), 8 * (distance to food), 8 * (distance to self))
// Then 16 hidden neurons
// 4 output for the 4 directions

    int retention_percent = 10;
    int elite_count = (int)(POP_SIZE * retention_percent / 100);
    float mutation_rate = 0.15f;
    float mutation_strength = 0.25f;

    // Malloc the population and generetate them random
    Genome* population = malloc(sizeof(Genome) * POP_SIZE);

    for(int i = 0; i < POP_SIZE; i++){
        population[i] = create_random_genome();
    }


    for(int gen = 0; gen < 300; gen++){
        // Parallelize the simulation loop so every thread gets its own stuff
        #pragma omp parallel
        {
            // Thread-local random seed to avoid rand() conflicts
            // Now i have to pass the seed to simulate and start_game functions
            unsigned int seed = time(NULL) ^ omp_get_thread_num();
            
            SnakeGame thread_game;
            thread_game.body = malloc(sizeof(Position) * MAX_SNAKE);
            float thread_sensors[INPUT_SIZE];
            layer thread_hidden = create_empty_layer(HIDDEN_SIZE, INPUT_SIZE);
            layer thread_output = create_empty_layer(OUTPUT_SIZE, HIDDEN_SIZE);

            // Simulate the game
            #pragma omp for schedule(dynamic)
            for(int i = 0; i < POP_SIZE; i++){
                simulate(&thread_game, &population[i], &thread_hidden, &thread_output, thread_sensors, &seed);
            }

            free(thread_game.body);
            free_layer(thread_hidden);
            free_layer(thread_output);
        }
        
        // Sort, breed and mutate children
        sort_by_fitness(population);

        for(int i = POP_SIZE - 1; i >= elite_count; i--){
            population[i] = breed_child(population, elite_count);
            mutate(&population[i], mutation_rate, mutation_strength);
        }

        // Make the last couple immigrants so theres more diversity
        int immigrant_count = POP_SIZE / 20;
        for(int i = POP_SIZE - immigrant_count; i < POP_SIZE; i++){
            population[i] = create_random_genome();
        }

        // Print every 10 runs to see progression
        if(gen % 10 == 0){
            printf("Best fitness of generation %d: %f\n", gen, population[0].fitness);
        }
    }


    // Had to create a whole new everything because now its inside the parallel code so cant use it
    SnakeGame game;
    game.body = malloc(sizeof(Position) * MAX_SNAKE);
    float sensors[INPUT_SIZE];
    layer hidden_layer = create_empty_layer(HIDDEN_SIZE, INPUT_SIZE);
    layer output_layer = create_empty_layer(OUTPUT_SIZE, HIDDEN_SIZE);
    
    simulate_with_gui(&game, &population[0], &hidden_layer, &output_layer, sensors);
    printf("Food: %d\n", game.score);

    free(game.body);
    free(population);
    free_layer(hidden_layer);
    free_layer(output_layer);

    return 0;
}
