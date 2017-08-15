// the demo file
#include <cstdint>
#include <limits>
#include <chrono>

#include "CardGenAlgo.h"

#include <iostream>
#include <string>
#include <sstream>


typedef std::chrono::high_resolution_clock::time_point TimeVar;                        
#define duration(a) std::chrono::duration_cast<std::chrono::milliseconds>(a).count()   
#define timeNow() std::chrono::high_resolution_clock::now()                            


using namespace std;

int sel1, sel2, sel3, sel4;
int sum, prod, cards;
int popsize, maxgens, dispFreq;
int numberOfExperiments;
double pxover, pmut;


void clearSCR() {
#if defined(__GCC__) || defined(__GNUC__)
	std::cout << "\x1B[2J\x1B[H";   // ANSI escape codes to clear screen (linux)
#elif defined(_MSC_VER)
	system("cls");      // windows
#endif

}

inline void waitUserInput() {
	cout << "\n> Press [enter] to continue.. ";     
	int temp;
	temp = cin.get();                  
	cout << "\n\n";                            
}

inline int getNumber(int a = 0, int b = std::numeric_limits<std::int32_t>::max(), bool isInline = true) {

	int sel;
	string strInput = "";
	bool firstTime = true, loopAgain;

	while (true)
	{
		loopAgain = false;
		if (isInline && !firstTime)
			cout << "\n";
		if (!isInline)
			cout << "\n> ";
		getline(cin, strInput);
		if (a!=0)
			if (strInput[0] == '0' || strInput[0] == '.')
				continue;

		int len = (int)strInput.length();
		for (int i = 0; i < len; i++) {
			if (strInput[i] < '0' || strInput[i] > '9')
				loopAgain = true;
		}
		if (loopAgain) continue;

		stringstream myStream(strInput);
		if ((myStream >> sel)) {
			if (sel >= a && sel <= b)
				break;
		}
		cin.clear();
	}

	return sel;
}

inline double getDouble() {

	double sel;
	string strInput = "";
	bool firstTime = true, loopAgain;

	while (true)
	{
		loopAgain = false;
		if (!firstTime)
			cout << "\n";
		getline(cin, strInput);

		int len = (int)strInput.length();
		for (int i = 0; i < len; i++) {
			if ((strInput[i] < '0' || strInput[i] > '9') && strInput[i]!='.')
				loopAgain = true;
		}
		if (loopAgain) continue;

		stringstream myStream(strInput);
		if ((myStream >> sel)) {
			break;
		}
		cin.clear();
	}

	return sel;
}

void reportChoices() {
	stringstream text;
	text << "\nYou have chosen ";
	if (sel1 == 1) text << "the default problem type(36,360,1-10).";
	else {
		text << "the custom problem type with sum target " << sum << ", product target " << prod << " and cards range 1-"<<cards<<".";
	}

	text << "\nPopulation size: " << popsize << "\nProbability of crossover: " << pxover << "\nProbability of mutation: " << pmut << "\nMaximum number of generations: " << maxgens;
	text << "\nOutput will appear on ";
	switch (sel2) {
	case 0:
		text << "console ";
		break;
	case 1:
		text << "file ";
		break;
	default:
		text << "console and file ";
		break;
	}
	text << "every " << dispFreq << " generation(s) (or when solution is found).";
	if (sel3==1) {
		text << "\nSimulation pace will be controlled by user.\n";
	}
	else {
		text << "\nSimulation will run for " << numberOfExperiments << " experiments, while the initial population in each of them will be ";
		if (sel4 == 1)
			text << "the same. \n";
		else
			text << "different. \n";
	}

	cout << text.str();

}

void manualRun(CardGenAlgo cga) {
	int cgen=0, cexp=1;
	int sel;

	do {
		cout << "> Current Exp: " << cexp << ", Current Gen: " << cgen << endl;
		cout << "> Actions: \n";
		cout << "1) Display the data of this generation on the screen\n";
		cout << "2) Advance N generations\n";
		cout << "3) Advance to last generation\n";
		cout << "4) Restart simulation\n";
		cout << "5) Go to start\n";

		sel = getNumber(1, 5, false);

		switch (sel) {
		case 1:
			cga.reportGeneration();
			break;
		case 2:
			cout << "-How many? ";
			cgen = cga.advanceNGenerations(getNumber(1));
			break;
		case 3:
			cgen = cga.advanceToFinalGeneration();
			break;
		case 4:
			cout << "-Same initial population?\n1)Yes 2)No\n";
			if (getNumber(1, 2, false) == 1)
				cga.restartSimulation(true);
			else
				cga.restartSimulation(false);
			cgen = 0;
			cexp++;
			break;
		default:
			sel = 5;
		}

	} while (sel != 5);
}

void automatedRun(CardGenAlgo cga) {
	int cexp=1;
	double dur = 0;
	
	cout << "> About to start the experiments. ";
	waitUserInput();

	cout << "> Execution began!\n";
	
	TimeVar now;

	do {
		cout << "> Curr Exp: " << cexp << endl;
		now = timeNow();
		cga.advanceToFinalGeneration();
		dur += duration(timeNow() - now);
		if (cexp == numberOfExperiments)
			break;
		cga.restartSimulation(sel4 == 1 ? true : false);
		cexp++;
	} while (true);

	cout << "> Execution ended!\n";
	cout << "> Median time of experiment: " << dur/numberOfExperiments << " ms\n";
	cout << "> Program will return to main screen now.\n";
	waitUserInput();
}

int main() {

	bool readyToStart;

	do {
		readyToStart = false;
		do {

			cout << "*--------------------------------------------*\n";
			cout << "|     Card Stack Genetic algorithm Demo      |\n";
			cout << "*--------------------------------------------*\n\n";

			cout << "-Choose type of problem:\n";
			cout << "1) Default (Sum=36, Product=360, Cards: 1-10)\n";
			cout << "2) Specify your own data\n";
			cout << "3) Exit\n";

			sel1 = getNumber(1, 3, false);
			if (sel1 == 3) return 0;
			else if (sel1 == 2) {
				cout << "> Enter sum target: ";
				sum = getNumber(0);

				cout << "> Enter prod target: ";
				prod = getNumber(0);

				cout << "> Enter card range: ";
				cards = getNumber(2);
				cout << endl;
			}

			cout << "-Enter the rest of the parameters:\n";
			cout << "> Enter population size: ";
			popsize = getNumber();
			cout << "> Enter propability of crossover: ";
			pxover = getDouble();
			cout << "> Enter probability of mutation: ";
			pmut = getDouble();
			cout << "> Enter maximum number of generations: ";
			maxgens = getNumber();
			cout << "> Enter your output preference: \n";
			cout << "1) Console only\n";
			cout << "2) File only (output.csv)\n";
			cout << "3) Both\n";

			sel2 = getNumber(1, 3, false) - 1;
			cout << "> Enter how often (in generations) you want the output to appear: ";
			dispFreq = getNumber();
			cout << endl;

			cout << "-Choose the execution plan:\n";
			cout << "1) Start and control the flow manually\n";
			cout << "2) Start a batch of experiments\n";

			sel3 = getNumber(1, 2, false);

			if (sel3 == 2) {
				cout << "-How many experiments? ";
				numberOfExperiments = getNumber(1);
				cout << "-Same population every time? \n";
				cout << "1) Yes 2) No\n";
				sel4 = getNumber(1, 2, false);
			}
			reportChoices();

			cout << "\n-Does this look OK?\n1) Yes 2) No\n";
			if (getNumber(1, 2, false) == 1) readyToStart = true;

			clearSCR();

		} while (!readyToStart);

		auto output = static_cast<OutputChoice>(sel2);

		if (sel1 == 1) {
			CardGenAlgo cga = CardGenAlgo(popsize, pxover, pmut, maxgens, output, dispFreq);

			if (sel3 == 1)
				manualRun(cga);
			else
				automatedRun(cga);
		}
		else {
			CardGenAlgo cga = CardGenAlgo(sum, prod, cards, popsize, pxover, pmut, maxgens, output, dispFreq);

			if (sel3 == 1)
				manualRun(cga);
			else
				automatedRun(cga);
		}

		clearSCR();
	} while (true);
	return 0;
}