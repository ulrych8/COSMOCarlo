#include <iostream>
#include <string>
#include <algorithm>	//shuffle
#include <stdlib.h>		//random
#include <ctime>		//clock
#include <fstream>		//file

#include "State.hpp"
#include "MCTS.hpp"

void MallowsModelGenerate(double phi, std::vector<std::vector<int>> &prefs, int const& nbCandidates, int const& nbVoters){
	std::vector<int> init;
	for (int i = 0; i < nbCandidates; ++i){
		init.push_back(i);
	}
	std::random_shuffle(init.begin(), init.end());
	prefs[0] = init;

	for (int i = 1; i < nbVoters; ++i)
	{
		std::vector<int> ballot = init;
		for (int A = 0; A < nbCandidates-1; ++A)
		{
			for (int B = A+1; B < nbCandidates; ++B)
			{
				if (((double) rand() / (RAND_MAX)) < phi )
				{
					iter_swap(ballot.begin() + A, ballot.begin() + B);
					break;
				}
			}
		}
		prefs[i] = ballot;
	}
}

void RandomPrefsGenerate(std::vector<std::vector<int>> &prefs, int const& nbCandidates, int const& nbVoters)
{
	std::vector<int> init;
	for (int i = 0; i < nbCandidates; ++i){
		init.push_back(i);
	}
	for (int i = 0; i < nbVoters; ++i)
	{
		std::random_shuffle(init.begin(), init.end());
		prefs[i] = std::vector<int>(init);
	}
}

void displayPrefs(std::vector<std::vector<int>> &prefs, int const& nbCandidates, int const& nbVoters) {
	for (int i = 0; i < nbVoters; ++i)
	{
		for (int j = 0; j < nbCandidates; ++j)
		{
			std::cout << prefs[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

int main(){
	srand (time(NULL));


    int const nbCandidates = 8;
	int const nbVoters = 4;

	int const nbPrefs = 100000;
	int const UCTrepeat = 50;

	std::vector<std::vector<int>> prefs(nbPrefs);

	RandomPrefsGenerate(prefs, nbCandidates, nbPrefs);


	State etat(nbCandidates, nbVoters, prefs, nbPrefs);

	MCTS mcts;
	std::cout << "nbCandidates : " << nbCandidates << std::endl;
	std::cout << "nbVoters : " << nbVoters << std::endl;
	for (int j = 0; j < nbCandidates-1; ++j)
	{
		std::cout << j << "/" << nbCandidates-2 << std::endl;
		int move = mcts.BestMoveUCT(etat, UCTrepeat);
		etat.addVoterToSequence(move);
	}

	std::vector<int> finalSeq = etat.getSequence();
	etat.displayVec(finalSeq);

	return 0;
}
	




	/*std::string const nomFichier("/home/ulysse/Documents/Stage/Code/COSMOCarlo/durationData.txt");
	std::ofstream monFlux(nomFichier.c_str());


	std::clock_t start;
    double duration = 0.0;

	int nbCandidates = 0;
	int nbVoters = 0;
	double phi = 0.2;

	int UCTprof = 10;

	if(monFlux)    
    {
        monFlux << "#phi = " <<  phi << std::endl;
        monFlux << "nbCandidates , nbVoters , duration , UCTcall" << std::endl;
		int i=0;

		while (duration < 5.0){
			i++;
			nbCandidates += 10;
			
			for (nbVoters = nbCandidates/5 +1; nbVoters < nbCandidates-1; nbVoters+=5)
			{
				std::vector<std::vector<int>> prefs(nbVoters);
				
				MallowsModelGenerate(phi, prefs, nbCandidates, nbVoters );
				//std::cout << "Prefs voters :" << std::endl;
				//displayPrefs(prefs, nbCandidates, nbVoters);

				
				loop : 
					State etat(nbCandidates, nbVoters, prefs);
					
					int max=0, bestCandidate;
					etat.optimumScore(bestCandidate, max);
					start = std::clock();


					//std::cout << "max borda score is : " << max << " for candidate " << bestCandidate << std::endl ;

					MCTS mcts;
					//std::cout << "start BestMoveUCT" << std::endl;
					for (int j = 0; j < nbCandidates-1; ++j)
					{
						int move = mcts.BestMoveUCT(etat, UCTprof);
						etat.addVoterToSequence(move);
					}
					duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
					if (etat.score(etat.getCandidatesLeft()[0]) != max )
					{
						std::cout << "max is " << max << " and value found was " << etat.score(etat.getCandidatesLeft()[0]) << std::endl;
						std::cout << "\n\n\n REAALLLYYY NEGGAEEEE \n" << std::endl;
						UCTprof += 2;
						goto loop;
					}
        		
        		monFlux << nbCandidates << "," << nbVoters << "," << duration << "," << UCTprof << std::endl;
        		std::cout << nbCandidates << "," << nbVoters << "," << duration << "," << UCTprof << std::endl;

			}
		}
    }
    else
    {
        std::cout << "ERREUR: Impossible d'ouvrir le fichier." << std::endl;
    }


    std::cout << std::endl << "           OVER" << std::endl; */