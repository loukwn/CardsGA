#pragma once

#include <vector>

using std::vector;

enum OutputChoice { OUTPUT_CONSOLE, OUTPUT_CSV, OUTPUT_BOTH };

struct Genotype
{
	vector<int> Genes; 		 // a series of 0s and 1s where if the ith digit is 0 that means that the card with the number i+1 is at the first stack, otherwise at the second
	double fitness;          // the fitness of the genotype
	int sum, product;        // the sum of the values in the first stack and the product of the values in the second
	double pSel, pCum;       // The probability of selection and the cumulative one for this certain genotype
	bool willMate;           // if this genotype will mate or not 
	

	// init a new genotype
	Genotype() {}
	Genotype(int numOfCards) : fitness(0), pSel(0), pCum(0), willMate(false) { Genes = vector<int>(numOfCards); }
};

class CardGenAlgo {
  /* 
   * The class/interface for the genetic algorithm that solves our problem	 
   */

private:
	// execution properties
	int mPopsize;
	double mPXOver, mPMutation;
	int mMaxGenerations;
	OutputChoice mOutputChoice;
	int mTargetSum;
	int mTargetProd;
	int mTargetCards;
	int mOutputFreq;

	// algorithm vars
	vector<Genotype> mPopulation, mInitialPopulation;
	Genotype bestGenotype;
	int bestGenotypeIndex;
	int mCurrentGen, mCurrentExp;
	double totalFitness;
	double totalFitnessSquare;
	bool solutionFound;
	

	// population initialization
	void initialize();

	// core functions
	bool evaluate();
	void select();
	void crossover();
	void mutate();

	// aux functions
	void checkForInputErrors();
	void initVars();
	inline double randZeroToOne();
	inline double getEuclideanDistance(int sum, int product);
	void setBestGenotype(int);
	void mateGenotypes(int, int, int);
	void displayDataAndReport(bool);

public:
	// Normal constructor (Target sum: 36, Target Product: 360, Cards: 1-10)
	CardGenAlgo(int popSize, double pXOver, double pMutation, int maxGenerations, OutputChoice outputChoice, int outputFreq);
	// Custom constructor (target sum/product as well as cards, decided by user)
	CardGenAlgo(int sum, int prod, int totalCards, int popSize, double pXOver, double pMutation, int maxGenerations, OutputChoice outputChoice, int outputFreq);


	// interact with outside world
	int advanceNGenerations(int n);
	int advanceToFinalGeneration();
	void restartSimulation(bool samePopulation);
	void reportGeneration();
};