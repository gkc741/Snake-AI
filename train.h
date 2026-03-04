#ifndef NN_H
#define NN_H

#include <stdint.h>
#include "gui.h"

#define INPUT_SIZE 24
#define HIDDEN_SIZE 32
#define OUTPUT_SIZE 4 

#define GENOME_SIZE (INPUT_SIZE * HIDDEN_SIZE) + HIDDEN_SIZE + (HIDDEN_SIZE * OUTPUT_SIZE) + OUTPUT_SIZE
#define POP_SIZE 500


typedef struct{
    float weights[GENOME_SIZE];  // all the weights and biases
    float fitness;  // how good it was
}Genome;

typedef struct layer{
    float* weights; // Long flattended list of weights for every neruon in the layer
    float* bias; // the bias
    int nr_of_neurons;  // the size of the out list basically how many neurons
    int in_size; // the size of the input list
}layer;



layer create_empty_layer(int nr_of_neurons, int nr_of_inputs);
void free_layer(layer layer);

// Snake
/*
Calculates the direction the snake should take based on the inputs and the NN
*/
int snake_forward_pass(float* input, layer* hidden_layer, layer* output_layer);

// Training and genetic algorithm functions
void get_sensors(SnakeGame* game, float* sensors);
Genome create_random_genome();
void genome_to_layers(Genome* g, layer* hidden, layer* output);
void calc_fitness(SnakeGame* game, Genome* g);
void simulate_step(SnakeGame* game, layer* hidden, layer* output, float* sensors, int* starvation_steps, int* previous_score);
void simulate(SnakeGame* game, Genome* g, layer* hidden, layer* output, float* sensors);


int comp(const void *a, const void *b);
void sort_by_fitness(Genome* population);
Genome breed_child(Genome population[], int retention_percent);
void mutate(Genome* offspring, float mutation_rate, float mutation_strength);
void simulate_with_gui(SnakeGame* game, Genome* g, layer* hidden, layer* output, float* sensors);




#endif
