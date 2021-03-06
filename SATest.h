#ifndef _SATEST_
#define _SATEST_

#include <vector>
#include "conio.h "
#include "windows.h"
#include <iostream>
#include <random>
#include <math.h>  
#include <chrono>

#define C_LIMIT 50
#define BOLTZ 1

unsigned seed_norm = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator(seed_norm);
using namespace std;

double genrand_real1(void);
void init_genrand(unsigned long s);

// --> This class represents the parameters
class cParameters {
public:
	cParameters() {};
	~cParameters() {};

	typedef vector<double>::iterator fIterator;
	typedef vector<double>::const_iterator  const_fIterator;
	typedef vector<int>::iterator iIterator;
	typedef vector<int>::const_iterator  const_iIterator;

	// --> Iterators operation
	fIterator fbegin(void) { return vFloats.begin(); };
	const_fIterator fbegin(void) const { return vFloats.begin(); };
	fIterator fend(void) { return vFloats.end(); };
	const_fIterator fend(void) const { return vFloats.end(); };
	iIterator ibegin(void) { return vInts.begin(); };
	const_iIterator ibegin(void) const { return vInts.begin(); };
	iIterator iend(void) { return vInts.end(); };
	const_iIterator iend(void) const { return vInts.end(); };


	// --> Retrive the num-th integer parameter
	int getIntParameter(int num) {
		if (num < 0 || num >(int)(vInts.size() - 1)) return 0;
		int cont = 0; iIterator it = ibegin();
		while (cont != num && it != iend()) {
			cont++; it++;
		}
		return *it;
	};

	// --> Set the num-th integer parameter
	void setIntParameter(int num, int param) {
		if (num < 0 || num >(int)(vInts.size() - 1)) return;
		int cont = 0; iIterator it = ibegin();
		while (cont != num && it != iend()) {
			cont++; it++;
		}
		*it = param;
	};

	// --> Retrive the num-th flaot parameter
	double getFloatParameter(int num) {
		if (num < 0 || num >(int)(vFloats.size() - 1)) return 0.0;
		int cont = 0; fIterator it = fbegin();
		while (cont != num && it != fend()) {
			cont++; it++;
		}
		return *it;
	};

	// --> Set the num-th Float parameter
	void setFloatParameter(int num, double param) {
		if (num < 0 || num >(int)(vFloats.size() - 1)) return;
		int cont = 0; fIterator it = fbegin();
		while (cont != num && it != fend()) {
			cont++; it++;
		}
		*it = param;
	};

	int getNumberFloats(void) const { return (int)vFloats.size(); };
	int getNumberInts(void) const { return (int)vInts.size(); };

	void clear(void) {
		vFloats.clear();
		vInts.clear();
		crystallization.clear();
		accepted.clear();
		rejected.clear();
	};

	void addFloat(double val) { vFloats.push_back(val); crystallization.push_back(0); accepted.push_back(0); rejected.push_back(0); };
	void addInt(int n) { vInts.push_back(n); };

	void setMax(vector<double> max, int imax) { this->max = max; this->imax = imax; };
	vector<double> getMax(void) const { return this->max; };
	void setMin(vector<double> min, int imin) { this->min = min; this->imin = imin; };
	vector<double> getMin(void) const { return this->min; };

	void pushMax(double val){
		max.push_back(val);
	};

	void pushMin(double val){
		min.push_back(val);
	};

	double getMin(int index) const {
		return min[0];
	};

	double getMax(int index) const {
		return max[0];
	};

	double getStep(int index) const {
		return max[0] - min[0];
	};

	int getLast(void) const { return this->last; };
	void setLast(int last) { this->last = last; };
	int getFirst(void) const { return this->first; };
	void setFirst(int first) { this->first = first; };

	void incrementCrystallization(int index) {
		if (index + 1 > 0)
			if (crystallization[index] < 5000000)
			crystallization[index] ++;

		rejected[index] ++;

	};
	void resetCrystallization(int index) {
		if (index + 1 > 0) {
			if (crystallization[index] > 0) {
				crystallization[index] = 0 ;
			}
			if (crystallization[index] < 0)
				crystallization[index] = 0;
		}
		accepted[index] ++;
	};

	void decreaseCrystallization(int index, int pass) {
		if (index + 1 > 0) {
			if (crystallization[index] > 0) {
				crystallization[index] -=pass;
			}
			if (crystallization[index] < 0)
				crystallization[index] = 0;
		}
		accepted[index] ++;
	};

	int getCrystallization(int index) { return crystallization[index]; };

	int getaccepted(int index) { return accepted[index]; };

	int getrejected(int index) { return rejected[index]; };

	void verifycrystalization() {
		int i = 1000;
		while (i < vFloats.size()) {
			if (accepted[i] > rejected[i]) {
				crystallization[i] /=2;
			}
			accepted[i] = 0;
			rejected[i] = 0;
			i++;
		}
	}

	double shuffle(int index) {
		double val = getMax(index);
		int count = 0;
		if (crystallization[index] < C_LIMIT) {
			while (val + vFloats[index] < getMin(index) || val + vFloats[index] > getMax(index) || count == 0) {
				val = 0;
				for (int k = 0; k < crystallization[index] + 1; k++)
					val += genrand_real1();
				val /= (double)(crystallization[index] + 1);
				val -= 0.5;
				val *= getStep(index);
				count = 1;
			}
		}
		else{
			std::normal_distribution<double> distribution(0.0, 1 / (double)(1 * exp(crystallization[index] + 1  - C_LIMIT)));
			while (val + vFloats[index] < getMin(index) || val + vFloats[index] > getMax(index) || count == 0) {
				val = distribution(generator)*getStep(index);
				count = 1;
			}
		}

		vFloats[index] += val;
		return val;
	};



private:

	// --> Vector of Floats
	vector<double> vFloats;

	// --> Vector of Integers
	vector<int> vInts;

	// --> Parameter range
	vector<double> min, max;

	// --> Pointer to first and last points
	int first, last, imin, imax;

	// --> Cristallization factor for this parameter
	vector<int> crystallization;

	// --> accepted and rejected by parameter
	vector<int> accepted;
	vector<int> rejected;
};

class cInterpretParameters {
public:
	cInterpretParameters() {
		parameters = (cParameters *)0;
	};
	~cInterpretParameters() {};

	// --> Configure the parameters
	void setParameters(cParameters *parameters) { this->parameters = parameters; };
	cParameters *getParameters(void) { return this->parameters; };

	
private:
	// --> Pointer to parameters
	cParameters *parameters;
};

class cEditParameters {
public:
	cEditParameters() {
		parameters = (cParameters *)0;
		pEditFloat = -1;
	};
	~cEditParameters() {};

	// --> Configure the parameters
	void setParameters(cParameters *parameters) { this->parameters = parameters; };
	cParameters *getParameters(void) { return this->parameters; };


	// --> Edit next float parameter
	void goNextFloatParameter(void) {
		if (parameters == (cParameters *)0) {
			printf("There is no associated parameters.\n");
			return;
		}

		if (pEditFloat < parameters->getNumberFloats() - 1)
			pEditFloat++;
	};

	// --> Edit previous float parameter
	void goPreviousFloatParameter(void) { if (pEditFloat > 0) pEditFloat--; };

	// --> Modify current float parameter
	void modifyFloatParameter(double step) {
		if (parameters == (cParameters *)0) {
			printf("There is no associated parameters.\n");
			return;
		}

		if (pEditFloat < 0) {
			printf("There is no current float parameter set.\n");
			return;
		}

		double param = parameters->getFloatParameter(pEditFloat);
		parameters->setFloatParameter(pEditFloat, param + step);
	};

	int getCurrentFloatParameter(void) const { return this->pEditFloat; };

private:
	// --> Pointer to parameters
	cParameters *parameters;

	// --> Pointer to the floating parameter that is edited
	int pEditFloat;
};

class cStatistics {
public:
	cStatistics() { first = true; };
	~cStatistics() {};

	void init(void) {
		first = true;
		FILE *fp = fopen(filename, "w");
		fprintf(fp,
			"pTa, tMinE, tMaxE, tMedia, tVariance, tSpecifHeat, tVarCost, tAvgSquare, tAvgCost, sum, pAlfa, pAccepted, pNotAccepted, N_iter\n");
		fclose(fp);
	};

	// --> Retrieve new value for alfa
	double getAlfa(void) const { return this->pAlfa; };

	// --> Set the storage for statistics
	void setFilename(char *filename) { strcpy(this->filename, filename); };

	// --> Add a new energy value
	void pushEnergy(double value) { energy.push_back(value); };

	// --> clear energy vactor
	void clearEnergy() { energy.clear(); };

	boolean FPClass(double x)
	{
		int i = _fpclass(x);
		boolean s = false;
		switch (i)
		{
		case _FPCLASS_SNAN: s = true;					 break;
		case _FPCLASS_QNAN: s = true;                    break;
		}
		return s;
	}

	// --> Calculate the statistics
	void print(double pTa, int pAccepted, int pNotAccepted, int N_iter) {

		vector<double>::iterator itf = energy.begin();
		double sum = 0, tMinE = *itf, tMaxE = *itf;
		for (itf = energy.begin(); itf != energy.end(); itf++) {
			if (tMinE > *itf) tMinE = *itf;
			if (tMaxE < *itf) tMaxE = *itf;
			sum += exp(-(*itf) /( BOLTZ*(pTa)));
		}

		vector<double> PiT;
		for (itf = energy.begin(); itf != energy.end(); itf++)
			PiT.push_back(1.0/energy.size());

		double tAvgCost = 0, tAvgSquare = 0;
		vector<double>::iterator itg = energy.begin();
		for (itf = PiT.begin(); itf != PiT.end(); itf++, itg++) {
			tAvgCost += (*itf) * (*itg);
			tAvgSquare += (*itf) * (*itg) * (*itg);
		}

		double tMedia = 0.0; int tCount = 0;
		for (itf = PiT.begin(); itf != PiT.end(); itf++, tCount++) 
			tMedia += *itf;
		tMedia /= (double)tCount;

		double std_amost = 0.0;
		double avg_amost = 0.0;
		for (itg = energy.begin(); itg != energy.end(); itg++)
			avg_amost += *itg;
		tMedia /= (double)tCount;
		for (itg = energy.begin(); itg != energy.end(); itg++)
			std_amost += (*itg- avg_amost)*(*itg - avg_amost);
		std_amost /= (double)(tCount);
		std_amost = sqrt(std_amost);


		double tVariance = 0.0;
		for (itf = PiT.begin(); itf != PiT.end(); itf++)
			tVariance += (*itf - tMedia) * (*itf - tMedia);
		tVariance /= (double)tCount;

		tVarCost = tAvgSquare - tAvgCost * tAvgCost;
		if (first) {
			pLastVarCost = tVarCost;
			pLastVariance = tVariance;
			pLastT = pTa;
			first = false;
		}

		double tVarC = (1.0 - 0.95) * tVarCost + 0.95 * pLastVarCost * pTa / pLastT;
		double tVar = (1.0 - 0.95) * tVariance + 0.95 * pLastVariance * pTa / pLastT;
		double tSpecifHeat = tVarCost / (pTa * pTa);

		pLastT = pTa;
		pLastVariance = tVariance;
		pLastVarCost = tVarCost;
		pAlfa = exp(-(0.05 * pTa) / sqrt(tVarCost));
		if (FPClass(pAlfa) == true) pAlfa = 0.99;
		if (pAlfa < 0.8) pAlfa = 0.8;
		FILE *fp = fopen(filename, "a");

		fprintf(fp,
			"%e; %e; %e; %e; %e; %e; %e; %e; %e; %e; %e; %d; %d %d %e\n",
			pTa, tMinE, tMaxE, tMedia, tVariance, tSpecifHeat, tVarCost, tAvgSquare, tAvgCost, sum, pAlfa, pAccepted, pNotAccepted, N_iter, std_amost);
		fclose(fp);
	};

	double get_variance(void) { return this->tVarCost; };

private:
	vector<double> energy;

	// --> File to store statistics
	char filename[100];

	// --> Last variational cost
	double pLastVarCost;

	// --> Last variance
	double pLastVariance;

	// --> Last temperature
	double pLastT;

	// --> New value for alfa
	double pAlfa;

	// --> Flag indicating first time
	bool first;

	// --> energy variance
	double tVarCost;
};

void readParameterFile(char *filename, cParameters &params, int &iteractionNumber, int &acceptedEquilibrium, double &initT, double &Final_Temp, int &Max_iter);

class cSimulatedAnnealing{
public:
	cSimulatedAnnealing() {};
	~cSimulatedAnnealing() {};

	void init(char *rfilename, char *res, char *wfilename) {

		int iteractionNumber;
		int acceptedEquilibrium;
		double initT;
		// --> Initiate random number generator
		long seed = GetTickCount();
		init_genrand(seed);
		// --> Reserve space for parameters
		pListParameters.clear();

		// --> Set storage for statistics
		statistics.setFilename(wfilename);

		// --> Configure parameters
		interpreter.setParameters(&pListParameters);

		// --> Read parameter file
		readParameterFile(rfilename, pListParameters, iteractionNumber, acceptedEquilibrium, initT, Final_Temp, Max_iter);

		pTa = initT;
		pAccepted = pNotAccepted = 0;
		pAlfa = 0.95;

		firstShow = true;

		resFilename = res;

		this->pIteractionNumber = iteractionNumber;
		this->pAcceptedEquilibrium = acceptedEquilibrium;

	};

	void setValue(int index, double value) { pListParameters.setFloatParameter(index, value); };

	double getValue(int index) { return pListParameters.getFloatParameter(index); };
	double getEnergyCandidate(void) const { return this->pEnergyCandidate; };


	void setCalculateEnergyFunction(double(*calculateEnergy)(cParameters &)) {
		this->calculateEnergy = calculateEnergy;
	};

	void acceptedCandidate(long index, double cEneryCandidate) {
		pListParameters = pListParametersCandidates;
		interpreter.setParameters(&pListParameters);
		pEnergyCandidate = cEneryCandidate;
		pAccepted++;
		statistics.pushEnergy(cEneryCandidate);
		pListParameters.decreaseCrystallization(index, 1);
		//pListParameters.resetCrystallization(index);
	};

	void run(void) {
		int count = 0;
		double step = 0;
		bestCont = 0;
		N_iter = 0;

		// --> Determine the initial Energy
		interpreter.setParameters(&pListParameters);
		pEnergyCandidate = pEnergyCandidateBest = (*calculateEnergy)(pListParameters);
		pListParametersBest = pListParameters;

		// --> Initiate the statistics calculation
		statistics.init();

		// --> While the stop criteria is not reached
		while (stopCriteria()) {

			// --> Initialize counters
			pAccepted = pNotAccepted = 0;

			// --> While the equilibrium is not reached
			while (equilibriumNotReached()) {
		
				// --> Copy the actual candidate
				pListParametersCandidates = pListParameters;

				// --> Calculate the new shuffling
				long index = (int)(genrand_real1() * ((double)pListParameters.getNumberFloats()));
				step = pListParametersCandidates.shuffle(index);
				interpreter.setParameters(&pListParametersCandidates);

				// --> Calculate the new Energy
				double cEnergyCandidate = (*calculateEnergy)(pListParametersCandidates);

				// --> New energy is lower?
				if (cEnergyCandidate < pEnergyCandidate){  //delta"e" = cEnergyCandidate-pEnergyCandidate (nova energia - a anterior)
					// --> Then accept the candidate
					acceptedCandidate(index, cEnergyCandidate);
				}
				else {
					// --> The probability is lower than the energy probability?
					if (exp((pEnergyCandidate - cEnergyCandidate) / (BOLTZ*(pTa))) > genrand_real1()){
						// --> Then accept the candidate
						acceptedCandidate(index, cEnergyCandidate);
					}
					else {
						// --> The candidate was not accepted
						pListParameters.incrementCrystallization(index);
						pNotAccepted++;
					}
				}

				checkKeyBoard();
				// --> Check for best ever candidate
				if ((pEnergyCandidateBest > cEnergyCandidate) || firstShow) {
					if (pEnergyCandidateBest > cEnergyCandidate) {
						pListParametersBest = pListParameters;
						pEnergyCandidateBest = cEnergyCandidate;
						bestCont++;
					}
					pListParameters = pListParametersBest;
					interpreter.setParameters(&pListParameters);
					pEnergyCandidate = (*calculateEnergy)(pListParameters);
					firstShow = false;
				}
				N_iter++;
			}
		printf("*** %e, %e, %d, %d, %d\n", pEnergyCandidateBest, pTa, pAccepted, pAccepted + pNotAccepted, N_iter);
		statistics.print(pTa, pAccepted, pNotAccepted, N_iter);

		// --> Retrieve new value for alfa
		pAlfa = statistics.getAlfa();

		// --> Go to next temperature
		if (pTa > pow(10,-1000)){
			pTa = nextTemperature(); };

		// --> purge energy vector
		statistics.clearEnergy();

		std_temp  = sqrt(statistics.get_variance());
		//pListParameters.verifycrystalization();
	}
	printf("End = %6.12f, %d, %d\n", pTa, pAccepted, pAccepted + pNotAccepted);
	};

	void write(char *filename) {
		interpreter.setParameters(&pListParametersBest);
		FILE *fw = fopen(filename, "w");
		fprintf(fw, "*** Exit File \n");
		fprintf(fw, "\n\n");
		fprintf(fw, "***  Variable Values Crystallization NUmber of Acceptance and Rejections \n");
		vector<double>::iterator itt = pListParametersBest.fbegin();
		int i = 0;
		while (itt != pListParametersBest.fend()) {
			fprintf(fw, "%e %d %d %d \n", *itt, pListParametersBest.getCrystallization(i), pListParametersBest.getaccepted(i), pListParametersBest.getrejected(i));
			itt++;
			i++;
		}
		fprintf(fw, "---\n");
		fprintf(fw, "\n\n");
		fprintf(fw, "*** Cost\n");
		fprintf(fw, "%e\n", pEnergyCandidateBest);
		fprintf(fw, "\n\n");
		fprintf(fw, "*** Number of iterations\n");
		fprintf(fw, "%d\n", N_iter);
		fclose(fw);
	};

	void write_best(char *filename) {
		interpreter.setParameters(&pListParametersBest);
		FILE *fw = fopen(filename, "a");
		fprintf(fw, "%e %d\n", pEnergyCandidateBest, N_iter);
		fclose(fw);
	};

	// --> Criteria for stop
	bool stopCriteria(void) {
		//return ((double)pAccepted) / ((double)(pAccepted + pNotAccepted)) > 0.01 || firstShow;
		return (pTa > Final_Temp || firstShow) && N_iter < Max_iter;
	};

	// --> Determine the next temperature
	//double nextTemperature(void) { return pTa * 0.99; };
	double nextTemperature(void) { return pTa * pAlfa;};

	// --> Criterium for equilibrium
	bool equilibriumNotReached(void) {
		return pAccepted + pNotAccepted < pIteractionNumber && pAccepted < pAcceptedEquilibrium && N_iter < Max_iter;
	};

	// --> Total number of accepted solutions
	long pAccepted;

	// --> Total number of not accepted solutions
	long pNotAccepted;

	// --> Number of accepted to reach equilibrium
	long pAcceptedEquilibrium;

	// --> Total number of iteractions to reach equilibrium
	long pIteractionNumber;

	// --> Total number of iteractions
	long N_iter;

	// --> Cooling factor
	double pAlfa;

	// --> Actual temperature
	double pTa;

	// --> Final temperature and iterations
	double Final_Temp;
	int Max_iter;

	// --> Energy for the actual candidate
	double pEnergyCandidate;

	// --> List of parameters
	cParameters pListParameters;
	cParameters pListParametersCandidates;
	cParameters pListParametersBest;


	// --> List of parameters for best energy
	double pEnergyCandidateBest;

	// --> Store the statistics
	cStatistics statistics;

	// --> Pointer for energy calculation
	double(*calculateEnergy)(cParameters &);

	cInterpretParameters interpreter;

	void checkKeyBoard(void);

	bool firstShow;

	char *resFilename;

	long bestCont;

	double std_temp;

	int std_mag;

};

void readParameterFile(char *filename, cParameters &params, int &iteractionNumber, int &acceptedEquilibrium, double &initT, double &Final_Temp, int &Max_iter)
{
	FILE *fr = fopen(filename, "r");
	if (fr == (FILE *)0) {
		printf("There is no file %s\n", filename);
		return;
	}

	long seed1 = GetTickCount();
	init_genrand(seed1);

	params.clear();
	int id = 0;
	char line[500];
	fgets(line, 400, fr);
	fgets(line, 400, fr);
	fgets(line, 400, fr);
	fgets(line, 400, fr);
// --> Sets the lower limit
	while (true) {
		fgets(line, 400, fr);
		if (line[0] == '-' && line[1] == '-')
			break;
		double val = atof(line);
		params.pushMin(val);
	}
	fgets(line, 400, fr);
	fgets(line, 400, fr);
// --> Sets the upper limit
	while (true) {
		fgets(line, 400, fr);
		if (line[0] == '-' && line[1] == '-')
			break;
		double val = atof(line);
		params.pushMax(val);
	}
	fgets(line, 400, fr);
	fgets(line, 400, fr);
// --> Sets the number of variables and randomly initiate the variable initial values
	fgets(line, 400, fr);
	int numbvar = atoi(line);
	int aux = 0;
	while (aux < numbvar){
		double aux_val = params.getMin(aux) + (params.getMax(aux) - params.getMin(aux))*genrand_real1();
		params.addFloat(aux_val);
		aux++;
	}
// --> Sets the number of iterations and acceptances by temperature
	fgets(line, 400, fr);
	fgets(line, 400, fr);
	fgets(line, 400, fr);
	iteractionNumber = atoi(line);
	fgets(line, 400, fr);
	acceptedEquilibrium = atoi(line);

// --> Sets the initial and final tempertature
	fgets(line, 400, fr);
	fgets(line, 400, fr);
	fgets(line, 400, fr);
	initT = atof(line);
	fgets(line, 400, fr);
	Final_Temp = atof(line);

// --> Sets the maximum number of iterations
	fgets(line, 400, fr);
	fgets(line, 400, fr);
	fgets(line, 400, fr);
	Max_iter = atoi(line);
	fclose(fr);
}

#endif	/* _SATEST_ */