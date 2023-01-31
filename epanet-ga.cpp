#include "epanet-ga.h"

bool isNearlyEqual(double a, double b)
{
    int factor = 0.000001;

    double min_a = a - (a - std::nextafter(a, std::numeric_limits<double>::lowest())) * factor;
    double max_a = a + (std::nextafter(a, std::numeric_limits<double>::max()) - a) * factor;
    return min_a <= b && max_a >= b;
}

void GeneticAlgorithm::initialize_population()
{
    DISTRIBUTION_ROUGHNESS = std::uniform_real_distribution<double>(10.0, 150.0);
    DISTRIBUTION_EMITTER = std::uniform_real_distribution<double>(0.00001, 0.0015);
    DISTRIBUTION_DEMAND = std::uniform_real_distribution<double>(0.00001, 0.1);
    DISTRIBUTION_NODE_DEMAND = std::uniform_real_distribution<double>(0.0001, 1);
    Parser::parseResultTxt("res.txt", observed_atakoy_vector);

    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        population.push_back(first_individual);
        for (int j = 0; j < CHROMOSOME_LENGTH_PIPES; j++)
        {
            population[i].project.pipes[j].roughness = (DISTRIBUTION_ROUGHNESS(RNG));
        }
        for (int j = 0; j < CHROMOSOME_LENGTH_EMITTERS; j++)
        {
            population[i].project.emitters[j].coefficient = (DISTRIBUTION_EMITTER(RNG));
        }
        for (int j = 0; j < CHROMOSOME_LENGTH_DEMANDS; j++)
        {
            double current = population[i].project.demands[j].demand;
            if (!isNearlyEqual(current, 0))
            {
                DISTRIBUTION_DEMAND = std::uniform_real_distribution<double>(current - (current / 2), current + (current / 2));
                population[i].project.demands[j].demand = (DISTRIBUTION_DEMAND(RNG));
            }
        }
        for (int j = 0; j < CHROMOSOME_LENGTH_NODE_DEMANDS; j++)
        {
            double current = population[i].project.nodes[j].demand;
            if (!isNearlyEqual(current, 0) && population[i].project.nodes[j].type == NodeType::JUNCTION)
            {
                DISTRIBUTION_NODE_DEMAND = std::uniform_real_distribution<double>(current - (current / 2), current + (current / 2));
                population[i].project.nodes[j].demand = (DISTRIBUTION_NODE_DEMAND(RNG));
            }
        }
    }
    //population.at(POPULATION_SIZE - 1) = first_individual;
}

void GeneticAlgorithm::calculate_fitness()
{
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        double error = 0;
        char resultFile[100] = "";
        sprintf_s(resultFile, "resources\\result_%d.txt", i);
        std::vector<Result_Atakoy> result_atakoy_vector;
        try
        {
            Parser::parseResultTxt(resultFile, result_atakoy_vector);
        }
        catch (...)
        {
            printf("%d (%s) is corrupted. Removing from the population.\n", i, resultFile);
            population.erase(population.begin() + i);
            POPULATION_SIZE--;
            i--;
            continue;
        }
        if (result_atakoy_vector.size() < 24)
        {
            printf("%d (%s) is corrupted. Removing from the population.\n", i, resultFile);
            population.erase(population.begin() + i);
            POPULATION_SIZE--;
            i--;
            continue;
        }
        for (int j = 0; j < result_atakoy_vector.size(); j++)
        {
            error += abs(result_atakoy_vector[j].Inc_FR_ - observed_atakoy_vector[j].Inc_FR_);
        }
        double fitness = 1 / (error / result_atakoy_vector.size());
        population[i].fitness = fitness;
        printf("[ -- %d -- ] - CALCULATED FITNESS: %f\n", i, fitness);
    }
}

void GeneticAlgorithm::threader()
{
    std::vector<std::thread> threads;
    printf("Populating treads...\n");
    for (int i = 0; i < POPULATION_SIZE; ++i)
    {
        threads.emplace_back(std::thread(&GeneticAlgorithm::for_each_individual, this, i));
        printf("Thread %d started.\n", i);
    }
    printf("Total threads: %d\n", threads.size());
    for (auto &th : threads)
        th.join();
}

void GeneticAlgorithm::mutate()
{
    for (int i = 0; i < population.size(); i++)
    {
        for (int j = 0; j < CHROMOSOME_LENGTH_PIPES; j++)
        {
            double randomNum = DISTRIBUTION_MUTATION(RNG);
            if (randomNum < MUTATION_RATE)
            {
                double current = population[i].project.pipes[j].roughness;
                DISTRIBUTION_ROUGHNESS = std::uniform_real_distribution<double>(current - (current * 0.25), current + (current * 0.25));
                double newRoughness = DISTRIBUTION_ROUGHNESS(RNG);
                population[i].project.pipes[j].roughness = newRoughness;
            }
        }
        for (int j = 0; j < CHROMOSOME_LENGTH_EMITTERS; j++)
        {
            double randomNum = DISTRIBUTION_MUTATION(RNG);
            if (randomNum < MUTATION_RATE)
            {
                double current = population[i].project.emitters[j].coefficient;
                DISTRIBUTION_EMITTER = std::uniform_real_distribution<double>(current - (current * 0.25), current + (current * 0.25));
                double newEmitterCoeff = DISTRIBUTION_EMITTER(RNG);
                population[i].project.emitters[j].coefficient = newEmitterCoeff;
            }
        }
        for (int j = 0; j < CHROMOSOME_LENGTH_DEMANDS; j++)
        {
            double randomNum = DISTRIBUTION_MUTATION(RNG);
            if (randomNum < MUTATION_RATE && !isNearlyEqual(population[i].project.demands[j].demand, 0.0))
            {
                double current = population[i].project.demands[j].demand;
                DISTRIBUTION_DEMAND = std::uniform_real_distribution<double>(current - (current * 0.25), current + (current * 0.25));
                population[i].project.demands[j].demand = DISTRIBUTION_DEMAND(RNG);
            }
        }
        for (int j = 0; j < CHROMOSOME_LENGTH_NODE_DEMANDS; j++)
        {
            double randomNum = DISTRIBUTION_MUTATION(RNG);
            if (randomNum < MUTATION_RATE && !isNearlyEqual(population[i].project.nodes[j].demand, 0.0) && population[i].project.nodes[j].type == NodeType::JUNCTION)
            {
                double current = population[i].project.nodes[j].demand;
                DISTRIBUTION_NODE_DEMAND = std::uniform_real_distribution<double>(current - (current * 0.25), current + (current * 0.25));
                population[i].project.nodes[j].demand = DISTRIBUTION_NODE_DEMAND(RNG);
            }
        }
    }
}

void GeneticAlgorithm::for_each_individual(int i)
{
    char fileName[100] = "";
    char result[100] = "";
    sprintf_s(fileName, "resources\\ga_inp_%d.inp", i);
    sprintf_s(result, "resources\\result_%d.txt", i);
    Saver::saveInputFile(fileName, population[i].project);
    trigger_run(fileName, result);
}

std::vector<Individual> GeneticAlgorithm::parent_selection_roulette_wheel()
{
    std::vector<Individual> parents;
    double total_fitness = 0;
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        total_fitness += population[i].fitness;
    }

    std::vector<double> cumulative_fitness;
    cumulative_fitness.push_back(population[0].fitness / total_fitness);
    for (int i = 1; i < POPULATION_SIZE; i++)
    {
        cumulative_fitness.push_back(cumulative_fitness[i - 1] + population[i].fitness / total_fitness);
    }

    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        double random_value = (double)rand() / RAND_MAX;
        for (int j = 0; j < POPULATION_SIZE; j++)
        {
            if (random_value <= cumulative_fitness[j])
            {
                Individual parent = population[j];
                parents.push_back(parent);
                break;
            }
        }
    }

    return parents;
}

std::vector<Individual> GeneticAlgorithm::parent_selection_tournament()
{
    std::vector<Individual> parents;

    for (int i = 0; i < POPULATION_SIZE / 2; i++)
    {
        int idx1 = rand() % POPULATION_SIZE;
        int idx2 = rand() % POPULATION_SIZE;

        while (idx1 == idx2)
        {
            idx2 = rand() % POPULATION_SIZE;
        }

        Individual parent = population[idx1].fitness > population[idx2].fitness ? population[idx1] : population[idx2];

        parents.push_back(parent);
    }

    return parents;
}

Individual GeneticAlgorithm::crossover(Individual parent1, Individual parent2)
{
    std::uniform_int_distribution<int> distribution_pipes(0, CHROMOSOME_LENGTH_PIPES - 1);
    std::uniform_int_distribution<int> distribution_emitters(0, CHROMOSOME_LENGTH_EMITTERS - 1);
    std::uniform_int_distribution<int> distribution_demands(0, CHROMOSOME_LENGTH_DEMANDS - 1);
    std::uniform_int_distribution<int> distribution_node_demands(0, CHROMOSOME_LENGTH_NODE_DEMANDS - 1);
    int crossoverPoint = distribution_pipes(RNG);
    int crossoverPoint2 = distribution_emitters(RNG);
    int crossoverPoint3 = distribution_demands(RNG);
    int crossoverPoint4 = distribution_node_demands(RNG);
    Individual child = parent1;
    std::vector<Individual> children;
    for (int j = 0; j < crossoverPoint; j++)
    {
        child.project.pipes[j] = parent1.project.pipes[j];
    }
    for (int j = crossoverPoint; j < CHROMOSOME_LENGTH_PIPES; j++)
    {
        child.project.pipes[j] = parent2.project.pipes[j];
    }
    for (int j = 0; j < crossoverPoint2; j++)
    {
        child.project.emitters[j] = parent1.project.emitters[j];
    }
    for (int j = crossoverPoint2; j < CHROMOSOME_LENGTH_EMITTERS; j++)
    {
        child.project.emitters[j] = parent2.project.emitters[j];
    }
    for (int j = 0; j < crossoverPoint3; j++)
    {
        child.project.demands[j] = parent1.project.demands[j];
    }
    for (int j = crossoverPoint3; j < CHROMOSOME_LENGTH_DEMANDS; j++)
    {
        child.project.demands[j] = parent2.project.demands[j];
    }
    for (int j = 0; j < crossoverPoint4; j++)
    {
        child.project.nodes[j] = parent1.project.nodes[j];
    }
    for (int j = crossoverPoint4; j < CHROMOSOME_LENGTH_NODE_DEMANDS; j++)
    {
        child.project.nodes[j] = parent2.project.nodes[j];
    }
    return child;
}

void GeneticAlgorithm::run()
{
    initialize_population();
    int num_runs = 0;
    while (num_runs < MAX_RUNS)
    {
        auto start = std::chrono::high_resolution_clock::now();

        threader();
        calculate_fitness();
        printf("Ran for %d/%d runs\n", num_runs + 1, MAX_RUNS);

        auto end = std::chrono::high_resolution_clock::now();

        log_results("results.txt", num_runs, population, end - start);
        // system("pause");
        std::vector<Individual> parents = parent_selection_tournament();
        printf("Parents: %d\n", parents.size());
        elitism();
        // breed_from_elites();
        // mutate();
        POPULATION_SIZE = population.size();
        num_runs++;
    }
}

void GeneticAlgorithm::trigger_run(std::string inputFile, std::string resultFile)
{
    std::string command = "resources\\run-epanet3.exe " + inputFile + " resources\\report.rpt " + resultFile;
    system(command.c_str());
}

void GeneticAlgorithm::elitism()
{
    std::sort(population.begin(), population.end(), [](const Individual &p1, const Individual &p2)
              { return p1.fitness > p2.fitness; });
    BEST_FITNESS = population[0].fitness;

    std::vector<Individual> newPopulation;
    double eliteRatio = 0.05;
    int eliteCount = (int)round(INITIAL_POPULATION_SIZE * eliteRatio);
    std::vector<Individual> parents;
    std::vector<Individual> elites;
    for (int i = 0; i < eliteCount; i++)
    {
        elites.push_back(population.at(i));
    }

    parents = parent_selection_tournament();

    while (newPopulation.size() < (0.60 * INITIAL_POPULATION_SIZE - eliteCount * 2))
    {
        int idx1 = rand() % parents.size();
        int idx2 = rand() % parents.size();
        Individual parent1 = parents[idx1];
        Individual parent2 = parents[idx2];

        Individual offspring = crossover(parent1, parent2);

        newPopulation.push_back(offspring);
    }

    while (newPopulation.size() < (INITIAL_POPULATION_SIZE - eliteCount * 2))
    {
        int idx1 = rand() % elites.size();
        int idx2 = rand() % elites.size();
        Individual parent1 = elites[idx1];
        Individual parent2 = elites[idx2];

        Individual offspring = crossover(parent1, parent2);

        newPopulation.push_back(offspring);
    }
    for (int i = 0; i < eliteCount; i++)
    {
        newPopulation.push_back(population.at(i));
    }
    population = newPopulation;
    adaptiveMutation();
    for (int i = 0; i < eliteCount; i++)
    {
        population.push_back(elites.at(i));
    }
}

double calculate_stddev(const std::vector<Individual> &population)
{
    double sum = 0;
    double mean;
    double variance = 0;
    double stdDev;
    for (int i = 0; i < population.size(); i++)
    {
        sum += population[i].fitness;
    }
    mean = sum / population.size();
    for (int i = 0; i < population.size(); i++)
    {
        variance += pow(population[i].fitness - mean, 2);
    }
    variance = variance / population.size();
    stdDev = sqrt(variance);
    return stdDev;
}

void GeneticAlgorithm::adaptiveMutation()
{
    double stdDev = calculate_stddev(population);
    double new_mutation_rate = MUTATION_RATE;
    MUTATION_RATE += (stdDev * ADAPTATION_FACTOR);
    MUTATION_RATE = std::max(MIN_MUTATION_RATE, std::min(MUTATION_RATE, MAX_MUTATION_RATE));
    mutate();
}

void GeneticAlgorithm::log_results(std::string log_file, int run, std::vector<Individual> population, std::chrono::duration<long long, std::nano> runtime)
{
    std::ofstream outputFile(log_file, std::ios::app);
    outputFile << "[RUN " << run << "]" << std::endl;
    for (int i = 0; i < population.size(); i++)
    {
        outputFile << std::left << std::setw(20) << i
                   << std::left << std::setw(20) << std::fixed << std::setprecision(10) << population[i].fitness
                   << std::endl;
    }
    std::sort(population.begin(), population.end(), [](const Individual &p1, const Individual &p2)
              { return p1.fitness > p2.fitness; });
    outputFile << "The best individual had "
               << std::setprecision(10) << population[0].fitness << " fitness."
               << std::endl;
    outputFile << "This run took "
               << std::setprecision(10) << std::chrono::duration_cast<std::chrono::microseconds>(runtime).count() << " microseconds.";
    outputFile << "("
               << std::setprecision(10) << std::chrono::duration_cast<std::chrono::seconds>(runtime).count() << " seconds)"
               << std::endl;
    outputFile << "Mutation rate was: "
               << std::setprecision(10) << MUTATION_RATE
               << std::endl;

    outputFile << std::endl;
    outputFile << std::endl;
    outputFile << std::endl;
}