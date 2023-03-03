#ifndef EPANET_GA_H
#define EPANET_GA_H

#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <numeric>
#include <string>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <algorithm>

#include "structs.h"
#include "struct_atakoy.h"
#include "parseinp.h"
#include "saveinp.h"

struct Individual
{
    Project project;
    double fitness;
};

class GeneticAlgorithm
{
public:
    int POPULATION_SIZE = 2800;
    int INITIAL_POPULATION_SIZE = 2800;

    int CHROMOSOME_LENGTH_PIPES = 100;
    int CHROMOSOME_LENGTH_EMITTERS = 100;
    int CHROMOSOME_LENGTH_DEMANDS = 100;
    int CHROMOSOME_LENGTH_NODE_DEMANDS = 100;
    double MUTATION_RATE = 0.7;
    double ELITISM_RATE = 0.2;
    int MAX_RUNS = 2000;
    double BEST_FITNESS = 0;
    double MIN_MUTATION_RATE = 0.1;
    double MAX_MUTATION_RATE = 1.0;
    double ADAPTATION_FACTOR = 0.1;

    Individual first_individual;
    std::vector<Individual> population;
    std::mt19937 RNG;
    std::uniform_real_distribution<double> DISTRIBUTION_ROUGHNESS;
    std::uniform_real_distribution<double> DISTRIBUTION_EMITTER;
    std::uniform_real_distribution<double> DISTRIBUTION_DEMAND;
    std::uniform_real_distribution<double> DISTRIBUTION_NODE_DEMAND;
    std::uniform_real_distribution<double> DISTRIBUTION_MUTATION = std::uniform_real_distribution<double>(0.0, 1.0);

    std::vector<Result_Atakoy> observed_atakoy_vector;

    void create_population_files();
    int get_chromosome_length()
    {
        return CHROMOSOME_LENGTH_PIPES;
    }

    int get_population_size()
    {
        return POPULATION_SIZE;
    }
    void threader();

    void adaptiveMutation();
    void set_project(Project project)
    {
        Individual individual_project;
        individual_project.project = project;
        this->first_individual = individual_project;
        this->CHROMOSOME_LENGTH_PIPES = project.pipes.size();
        this->CHROMOSOME_LENGTH_EMITTERS = project.emitters.size();
        this->CHROMOSOME_LENGTH_DEMANDS = project.demands.size();
        this->CHROMOSOME_LENGTH_NODE_DEMANDS = project.nodes.size();
    }
    void mutate();
    std::vector<Individual> parent_selection_tournament();
    std::vector<Individual> parent_selection_roulette_wheel();

    void elitism();
    Individual crossover(Individual parent1, Individual parent2);
    void breed_from_elites();

    void for_each_individual(int i);

    std::vector<Individual> get_population()
    {
        return population;
    }

    void initialize_population();
    void calculate_fitness();
    void trigger_run(std::string inputFile, std::string outputFile);
    void run();
    void log_results(std::string log_file, int run, std::vector<Individual> population, std::chrono::duration<long long, std::nano> runtime);
};

#endif