#include "CardGenAlgo.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <stdexcept>
#include <cmath>

using std::cout;
using std::endl;


// Normal Constructor
CardGenAlgo::CardGenAlgo(int popSize, double pXOver, double pMutation, int maxGenerations, OutputChoice outputChoice, int outputFreq) :
	mPopsize(popSize), mPXOver(pXOver), mPMutation(pMutation), mMaxGenerations(maxGenerations), mOutputChoice(outputChoice), mTargetSum(36), mTargetProd(360), mTargetCards(10), mOutputFreq(outputFreq)
{
	try {
		mCurrentExp = 1;
		checkForInputErrors();
		initVars();
		initialize();
		
		if (outputChoice != OUTPUT_CSV)
			cout << "> Successfully initialized\n\n";

		if (outputChoice != OUTPUT_CONSOLE) {
			// append to file
			std::ofstream outfile("output.csv", std::ofstream::out | std::ofstream::trunc);
			outfile << "Exp Gen TotFitness AvgFitness StdDev BestGenoSum BestGenoProd BestGenoFitnessn\n";
			outfile.close();
		}
	}
	catch (const std::invalid_argument& e) {
		cout << e.what();
		throw new std::invalid_argument("Error initializing object");
	}
}

// Custom Constructor
CardGenAlgo::CardGenAlgo(int sum, int prod, int totalCards, int popSize, double pXOver, double pMutation, int maxGenerations, OutputChoice outputChoice, int outputFreq) :
	mPopsize(popSize), mPXOver(pXOver), mPMutation(pMutation), mMaxGenerations(maxGenerations), mOutputChoice(outputChoice), mTargetSum(sum), mTargetProd(prod), mTargetCards(totalCards), mOutputFreq(outputFreq)
{
	try {
		mCurrentExp = 1;
		checkForInputErrors();
		initVars();
		initialize();
		
		if (outputChoice != OUTPUT_CSV)
			cout << "> Successfully initialized\n\n";

		if (outputChoice != OUTPUT_CONSOLE) {
			// append to file
			std::ofstream outfile("output.csv", std::ofstream::out | std::ofstream::trunc);
			outfile << "Exp Gen TotFitness AvgFitness StdDev BestGenoSum BestGenoProd BestGenoFitnessn\n";
			outfile.close();
		}

	}
	catch (const std::invalid_argument& e) {
		cout << e.what();
		throw new std::invalid_argument("Error initializing object");
	}
}

void CardGenAlgo::checkForInputErrors() {
	if (mPopsize<2)
		throw std::invalid_argument("Population size should be at least 2");

	if ((mPXOver <= 0.0 && mPXOver >= 1.0) || (mPMutation <= 0.0 && mPMutation >= 1.0))
		throw std::invalid_argument("Probabilities of mutation and crossover should both be in the range (0,1)");

	if (mMaxGenerations<1)
		throw std::invalid_argument("Max Generations should be at least 1");

	if (mTargetSum<1 || mTargetProd<1 || mTargetCards<1)
		throw std::invalid_argument("Target sum, product and cards should be at least 1");
}

void CardGenAlgo::initVars() {
	bestGenotype = Genotype(mTargetCards);
	bestGenotypeIndex = 0;
	mCurrentGen = 0;
	totalFitness = 0;
	totalFitnessSquare = 0;
	solutionFound = false;
}

// initialize normal function
void CardGenAlgo::initialize() {

	mPopulation = vector<Genotype>();
	mInitialPopulation = vector<Genotype>();

	int i, j;

	srand(time(NULL));

	for (i = 0; i < mPopsize; ++i) {
		Genotype genotype = Genotype(mTargetCards);

		// generate the random genes
		for (j = 0; j < mTargetCards; ++j)
			genotype.Genes[j] = (int)rand() % 2;

		mPopulation.push_back(genotype);
	}

	mInitialPopulation = mPopulation;
}

int CardGenAlgo::advanceToFinalGeneration() {

	bool gotIn = false;

	if (solutionFound) {
		cout << "> The best genotype has the best fitness possible. There is no need to continue.\n\n";
		return mCurrentGen;
	}

	for (int i = mCurrentGen; i < mMaxGenerations; ++i) {
		gotIn = true;
		mCurrentGen++;
		if (!evaluate()) {
			select();
			crossover();
			mutate();
		}
		else {
			solutionFound = true;
			break;
		}
		displayDataAndReport(false);
	}

	if (!gotIn) {
		cout << "You are already at the last generation. Try restarting!\n" << endl;
		return mCurrentGen;
	}
	displayDataAndReport(true);

	return mCurrentGen;
}

int CardGenAlgo::advanceNGenerations(int n) {
	
	if (n < 1) {
		cout << "You need to advance one or more generations.." << endl;
		return mCurrentGen;
	}

	if (solutionFound) {
		cout << "> The best genotype has the best fitness possible. There is no need to continue.\n\n";
		return mCurrentGen;
	}

	int targetGen = mCurrentGen + n;
	bool gotIn = false;

	if (targetGen >= mMaxGenerations)
		targetGen = mMaxGenerations;

	for (int i = mCurrentGen; i < targetGen; ++i) {
		gotIn = true;
		mCurrentGen++;
		if (!evaluate()) {
			select();
			crossover();
			mutate();
		}
		else {
			solutionFound = true;
			displayDataAndReport(true);
			break;
		}
		displayDataAndReport(false);
	}

	if (!gotIn) cout << "You are already at the last generation. Try restarting!\n" << endl;
	return mCurrentGen;
}

void CardGenAlgo::restartSimulation(bool samePopulation) {

	mCurrentExp++;
	initVars();

	if (samePopulation) {
		mPopulation = mInitialPopulation;
		cout << "> Reinitialized with the initial population.\n\n" << endl;
	} else {
		initialize();
		cout << "> Reinitialized with different population.\n\n" << endl;
	}
}

void CardGenAlgo::reportGeneration() {

	double avg, stddev, square_sum;

	avg = totalFitness / (double)mPopsize;
	square_sum = totalFitness*totalFitness / (double)mPopsize;
	stddev = sqrt((1.0 / (double)(mPopsize - 1))*(totalFitnessSquare - square_sum));

	cout << "> GEN: " << mCurrentGen << endl;
	cout << "- Total fitness: " << totalFitness << endl;
	cout << "- Avg Fitness: " << avg << endl;
	cout << "- Standard Deviation: " << stddev << endl;
	cout << "- Best Genotype: " << endl << "-- ";
	for (int j = 0; j < mTargetCards; j++)
		cout << bestGenotype.Genes[j] << " ";
	cout << endl << "-- Sum: " << bestGenotype.sum << endl;
	cout << "-- Product: " << bestGenotype.product << endl;
	cout << "-- Fitness: " << bestGenotype.fitness << endl << endl;
}

void CardGenAlgo::displayDataAndReport(bool ended) {

	double avg, stddev, square_sum;

	avg = totalFitness / (double) mPopsize;
	square_sum = totalFitness*totalFitness / (double) mPopsize;
	stddev = sqrt((1.0 / (double)(mPopsize - 1))*(totalFitnessSquare - square_sum));

	if (ended) {
		if (mOutputChoice == OUTPUT_BOTH || mOutputChoice == OUTPUT_CONSOLE) {
			cout << "------------------------------------" << endl;
			cout << "> FINAL GEN: " << mCurrentGen << endl;
			cout << "- Total fitness: " << totalFitness << endl;
			cout << "- Avg Fitness: " << avg << endl;
			cout << "- Standard Deviation: " << stddev << endl;
			cout << "- Best Genotype: " << endl << "-- ";
			for (int j = 0; j < mTargetCards; j++)
				cout << bestGenotype.Genes[j] << " ";
			cout << endl << "-- Sum: " << bestGenotype.sum << endl;
			cout << "-- Product: " << bestGenotype.product << endl;
			cout << "-- Fitness: " << bestGenotype.fitness << endl;
			cout << "------------------------------------" << endl << endl;
		}

		// append to file
		if (mOutputChoice == OUTPUT_BOTH || mOutputChoice == OUTPUT_CSV) {
			std::ofstream outfile("output.csv", std::ios_base::app);
			outfile << mCurrentExp << " " << mCurrentGen << " " << totalFitness << " " << avg << " " << stddev << " " << bestGenotype.sum << " " << bestGenotype.product << " " << bestGenotype.fitness << endl;
			outfile.close();
		}
	}
	else {
		if (mCurrentGen%mOutputFreq == 0) {
			// output to console
			if (mOutputChoice == OUTPUT_BOTH || mOutputChoice == OUTPUT_CONSOLE) {
			
				cout << "> GEN: " << mCurrentGen << endl;
				cout << "- Total fitness: " << totalFitness << endl;
				cout << "- Avg Fitness: " << avg << endl;
				cout << "- Standard Deviation: " << stddev << endl;
				cout << "- Best Genotype: " << endl << "-- ";
				for (int j = 0; j < mTargetCards; j++)
					cout << bestGenotype.Genes[j] << " ";
				cout << endl << "-- Sum: " << bestGenotype.sum << endl;
				cout << "-- Product: " << bestGenotype.product << endl;
				cout << "-- Fitness: " << bestGenotype.fitness << endl << endl;
			}

			// append to file
			if (mOutputChoice == OUTPUT_BOTH || mOutputChoice == OUTPUT_CSV) {
				std::ofstream outfile("output.csv", std::ios_base::app);
				outfile << mCurrentExp << " " << mCurrentGen << " " << totalFitness << " " << avg << " " << stddev << " " << bestGenotype.sum << " " << bestGenotype.product << " " << bestGenotype.fitness << endl;
				outfile.close();
			}
		}
	}
}


// evaluate the fitness of each genome
bool CardGenAlgo::evaluate() {
	if (mPopsize > 0) {

		int sum, product;
		bool thereIsCardInProduct = false;

		// for every genotype
		for (int i = 0; i < mPopsize; ++i) {
			sum = 0;
			product = 1;
			thereIsCardInProduct = false;

			// for every gene
			for (int j = 0; j < mTargetCards; ++j) {
				if (mPopulation[i].Genes[j] == 0)    // if the card in the first stack we add
					sum += j + 1;
				else {								 // if it is in the second one we multiply
					thereIsCardInProduct = true;
					product *= j + 1;
				}
			}

			if (!thereIsCardInProduct) product = 0;

			// TODO: to delete
			mPopulation[i].sum = sum;
			mPopulation[i].product = product;

			mPopulation[i].fitness = getEuclideanDistance(sum, product);

			if (mPopulation[i].fitness == 0) {
				setBestGenotype(i);
				return true;
			}
			mPopulation[i].fitness = 1 / (mPopulation[i].fitness);

			// we save the best genotype
			if (mPopulation[i].fitness > bestGenotype.fitness)
				setBestGenotype(i);
		}
	}
	return false;
}

// calculate the probabilities of each genotype
void CardGenAlgo::select() {

	vector<Genotype> newPopulation = vector<Genotype>();
	double roulette;
	int j;

	totalFitness = 0;
	totalFitnessSquare = 0;

	// first we calculate the total fitness
	for (int i = 0; i < mPopsize; ++i) {
		totalFitness += mPopulation[i].fitness;
		totalFitnessSquare += pow(mPopulation[i].fitness, 2);
	}

	// then we set the selection/cumulative probabilities for every genotype
	for (int i = 0; i < mPopsize; ++i) {
		mPopulation[i].pSel = mPopulation[i].fitness / totalFitness;

		if (i!=0) {
			mPopulation[i].pCum = mPopulation[i - 1].pCum + mPopulation[i].pSel;
		} else {
			mPopulation[i].pCum = mPopulation[i].pSel;
		}
	}

	// then we select based on the cumulative probability
	for (int i = 0; i < mPopsize; ++i) {
		roulette = randZeroToOne();

		if (mPopulation[0].pCum >= roulette) {
			newPopulation.push_back(mPopulation[0]);
			continue;
		}

		if (mPopulation[mPopsize - 1].pCum <= roulette) {
			newPopulation.push_back(mPopulation[bestGenotypeIndex]);
			continue;
		}

		j = 0;
		do {
			if (mPopulation[j].pCum <= roulette && mPopulation[j + 1].pCum >= roulette) {
				// we found a survivor
				newPopulation.push_back(mPopulation[j + 1]);
				break;
			}
			j++;
		} while (j < mPopsize-1 );
	}

	// finally we set the new population
	mPopulation = newPopulation;
	
}

// perform mating of genotypes
void CardGenAlgo::crossover() {

	int lovers = 0;
	int newLoverIndex, candidate = 0, firstLover = 0, secondLover = 0;

	// first we find based on our probability which genotypes will mate
	for (int i = 0; i < mPopsize; ++i) {
		if (randZeroToOne() < mPXOver) {
			mPopulation[i].willMate = true;
			lovers++;
		}
	}

	// then we make sure we have an even amount of lovers
	if ((lovers % 2) != 0) {
		do {
			newLoverIndex = (int)rand() % mPopsize;
		} while (mPopulation[newLoverIndex].willMate);

		mPopulation[newLoverIndex].willMate = true;
		lovers++;
	}

	// perform mating
	for (int i = 0; i<(lovers / 2); ++i) {

		// first lover
		while (!mPopulation[candidate].willMate) candidate++;
		firstLover = candidate;
		candidate++;
		
		// second lover
		while (!mPopulation[candidate].willMate) candidate++;
		secondLover = candidate;
		candidate++;

		// they will not mate again in this generation
		mPopulation[firstLover].willMate = false;
		mPopulation[secondLover].willMate = false;

		// crossover point
		int xoverPoint = ((int)rand() % 9) + 1;

		mateGenotypes(firstLover, secondLover, xoverPoint);
	}
}


// perform mutation based on the probability of mutation
void CardGenAlgo::mutate() {
	int i, j;

	for (i = 0; i < mPopsize; ++i) {
		for (j = 0; j < mTargetCards; ++j) {
			if (randZeroToOne() < mPMutation) {
				// this gene will be mutated

				int gene = mPopulation[i].Genes[j];
				if (gene == 0)
					mPopulation[i].Genes[j] = 1;
				else
					mPopulation[i].Genes[j] = 0;
			}
		}
	}
}


// generate a random double in [0,1)
inline double CardGenAlgo::randZeroToOne() { return rand() / (RAND_MAX + 1.); }

inline double CardGenAlgo::getEuclideanDistance(int sum, int product) {
	double distance = 0;

	distance += pow(mTargetSum - sum, 2);
	distance += pow(mTargetProd - product, 2);
	distance = pow(distance, 0.5);

	return distance;
}

void CardGenAlgo::setBestGenotype(int index) {
	bestGenotype.Genes = mPopulation[index].Genes;
	bestGenotype.fitness = mPopulation[index].fitness;
	bestGenotype.pSel = mPopulation[index].pSel;
	bestGenotype.pCum = mPopulation[index].pCum;
	bestGenotype.willMate = mPopulation[index].willMate;
	bestGenotype.sum = mPopulation[index].sum;
	bestGenotype.product = mPopulation[index].product;

	bestGenotypeIndex = index;
}

void CardGenAlgo::mateGenotypes(int first, int second, int xPoint) {
	
	Genotype temp = mPopulation[first];

	for (int i = xPoint; i < mTargetCards; ++i)
		temp.Genes[i] = mPopulation[second].Genes[i];
	for (int i = xPoint; i < mTargetCards; ++i)
		mPopulation[second].Genes[i] = mPopulation[first].Genes[i];

	mPopulation[first] = temp;
}
