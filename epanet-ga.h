#ifndef EPANET_GA_H
#define EPANET_GA_H

#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <thread>
#include "structs.h"
#include "struct_atakoy.h"
#include "parseinp.h"
#include "saveinp.h"

class GeneticAlgorithm
{
public:
    int POPULATION_SIZE = 20;
    int CHROMOSOME_LENGTH = 100;
    double MUTATION_RATE = 0.01;
    int MAX_RUNS = 1;
    std::vector<double> FITNESS;

    Project first_individual;
    std::vector<Project> population;
    std::mt19937 RNG;
    std::uniform_real_distribution<double> DISTRIBUTION;

    std::vector <Result_Atakoy> observed_atakoy_vector;

    void set_chromosome_length(int i)
    {
        this->CHROMOSOME_LENGTH = i;
    }

    int get_chromosome_length() {
        return CHROMOSOME_LENGTH;
    }

    int get_population_size() {
        return POPULATION_SIZE;
    }
    void threader();

    void set_project(Project project) {
        this->first_individual = project;
        set_chromosome_length(project.pipes.size());
    }
    void mutate();
    std::vector<Project> parent_selection_tournament();
    void crossover();

    void forEachIndividual(int i);

    std::vector<Project> get_population() {
        return population;
    }

    void initializePopulation();
    void calculateFitness();
    void trigger_run(std::string inputFile, std::string outputFile);
    void run();
};

#endif