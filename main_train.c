#include <stdlib.h>
#include <stdio.h>
#include "train.h"
#include "gui.h"
#include "time.h"



int main(){
    srand(time(NULL));

// This will take 24 (8 directions * (distance to wall), 8 * (distance to food), 8 * (distance to self))
// Then 16 hidden neurons
// 4 output for the 4 directions

    int retention_percent = 10;
    float mutation_rate = 0.02f;
    float mutation_strength = 0.1f;

    layer hidden_layer = create_empty_layer(HIDDEN_SIZE, INPUT_SIZE);
    layer output_layer = create_empty_layer(OUTPUT_SIZE, HIDDEN_SIZE);

    Genome* population = malloc(sizeof(Genome) * POP_SIZE);

    for(int i = 0; i < POP_SIZE; i++){
        population[i] = create_random_genome();
    }

    SnakeGame game;
    game.body = malloc(sizeof(Position) * MAX_SNAKE);
    float sensors[INPUT_SIZE];


    for(int gen = 0; gen < 150; gen++){
        for(int i = 0; i < POP_SIZE; i++){
            simulate(&game, &population[i], &hidden_layer, &output_layer, sensors);
        }
        sort_by_fitness(population);
        // // shakes things up my mutating the best every 5 times
        // if(gen % 5 == 0){
        //     mutate(&population[0], mutation_rate, mutation_strength);
        // }
        
        // keep top 10% so POP_SIZE / 10
        for(int i = POP_SIZE - 1; i >= POP_SIZE / retention_percent; i--){
            population[i] = breed_child(population, retention_percent);
            mutate(&population[i], mutation_rate, mutation_strength);
        }

        int immigrant_count = POP_SIZE / 20;
        for(int i = POP_SIZE - immigrant_count; i < POP_SIZE; i++){
            population[i] = create_random_genome();
        }

        printf("Best fitness of generation %d: %f\n", gen, population[0].fitness);
    }

    printf("\n=== Playing Best Snake ===\n");
    printf("Fitness: %f\n\n", population[0].fitness);

    simulate_with_gui(&game, &population[0], &hidden_layer, &output_layer, sensors);

    
    free_layer(hidden_layer);
    free_layer(output_layer);


    return 0;
}
