#include "epanet-ga.h"

void GeneticAlgorithm::initializePopulation()
{
    DISTRIBUTION = std::uniform_real_distribution<double>(50.0, 150.0);
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        population.push_back(first_individual);
        for (int j = 0; j < CHROMOSOME_LENGTH; j++)
        {
            population[i].pipes[j].roughness = (DISTRIBUTION(RNG));
        }
    }
}

double GeneticAlgorithm::calculateFitness()
{
    double fitness = 0;
    std::vector<Result_Atakoy> result_atakoy_vector;
    Parser::parseResultTxt("ak-Sonuc.txt", result_atakoy_vector);
    return fitness;
}
void GeneticAlgorithm::threader()
{
    std::vector<std::thread> threads;
    printf("Populating treads...\n");
    for (int i = 0; i < POPULATION_SIZE; ++i)
    {
        threads.emplace_back(std::thread(&GeneticAlgorithm::forEachIndividual, this, i));
        printf("Thread %d started.\n", i);
    }
    printf("Total threads: %d\n", threads.size());
    for (auto &th : threads)
        th.join();
}

void GeneticAlgorithm::mutate()
{
    std::normal_distribution<double> mutationDistribution(0, MUTATION_RATE);
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        for (int j = 0; j < CHROMOSOME_LENGTH; j++)
        {
            double randomNum = DISTRIBUTION(RNG);
            if (randomNum < MUTATION_RATE)
            {
                std::uniform_real_distribution<double> roughnessDistribution(50, 150);
                double newRoughness = roughnessDistribution(RNG);
                population[i].pipes[j].roughness = newRoughness;
            }
        }
    }
}

void GeneticAlgorithm::forEachIndividual(int i)
{
    char fileName[100] = "";
    char result[100] = "";
    sprintf(fileName, "resources\\ga_inp_%d.inp", i);
    sprintf(result, "resources\\result_%d.txt", i);
    Saver::saveInputFile(fileName, population[i]);
    trigger_run(fileName, result);
}

void GeneticAlgorithm::run()
{
    initializePopulation();
    int num_runs = 0;
    while (num_runs < MAX_RUNS)
    {
        threader();
        calculateFitness();
        mutate();
        printf("Ran for %d/%d runs\n", num_runs+1, MAX_RUNS);
        num_runs++;
    }
}

void GeneticAlgorithm::trigger_run(std::string inputFile, std::string resultFile)
{
    std::string command = "resources\\run-epanet3.exe " + inputFile + " resources\\report.rpt " + resultFile;
    system(command.c_str());
}