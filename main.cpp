#include <iostream>
#include <string>
#include <algorithm>	//shuffle
#include <stdlib.h>		//random
#include <ctime>		//clock
#include <fstream>		//file

   
#include "StateCentralAuthority.hpp"
#include "StateGivenSequence.hpp"
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

void randomSequence(std::vector<int> &seq, int const& nbCandidates, int const& nbVoters)
{
	for (int i = 0; i < nbVoters; ++i)
	{
		seq.push_back(i);
	}
	//sequence size should be equal to nbCandidates -1
	for (int j = nbVoters; j < nbCandidates-1; ++j)
	{
		seq.push_back(rand() % nbVoters);
	}	
	std::random_shuffle(seq.begin(), seq.end());
}


//====================================================================
//
//						M A I N
//
//====================================================================


int main(){
	srand (time(NULL));

	std::string CONTEXT = "2";
	bool REPEAT = false;

	int const UCTrepeat = 50;

	//SAVE DATA
	/*std::string const nomFichier("/home/ulysse/Documents/Stage/Code/COSMOCarlo/durationDataMAXN.txt");
	std::ofstream monFlux(nomFichier.c_str());

	std::clock_t start;
    double duration = 0.0;

    if(monFlux)    
    {
    	int nbCandidates = 0;
		int nbVoters = 0;
		std::vector<int> seq;
        monFlux << "#IC" << std::endl;
        monFlux << "nbCandidates , nbVoters , duration" << std::endl;
		int i=0;

		while (duration < 5.0){
			i++;
			nbCandidates += 10;
			
			for (nbVoters = nbCandidates/5 +1; nbVoters < nbCandidates-1; nbVoters+=5)
			{
				std::vector<std::vector<int>> prefs(nbVoters);
				std::vector<int> sequence;

				randomSequence(sequence, nbCandidates, nbVoters);
				RandomPrefsGenerate(prefs, nbCandidates, nbVoters);
				
				start = std::clock();

				StateGivenSequence etat(nbCandidates, nbVoters, prefs, sequence);
				MCTS<StateGivenSequence> mcts;

				int finalWinner = mcts.MAXN(etat, nbCandidates-1);

				duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

        		
        		monFlux << nbCandidates << "," << nbVoters << "," << duration << std::endl;
        		std::cout << nbCandidates << "," << nbVoters << "," << duration << std::endl;

			}
		}
    }
    else
    {
        std::cout << "ERREUR: Impossible d'ouvrir le fichier." << std::endl;
    }*/


	if (CONTEXT=="2" && REPEAT)
	{
		//int const nbCandidates = 8;
		//int const nbVoters = 4;
		double const phi=0.2;
		
		int LnbCandidates[] = {8, 10, 20, 20, 40};
		int LnbVoters[] = {4, 5, 7, 15, 15};

		for (int i = 0; i < 5; ++i)
		{
			int &nbCandidates = LnbCandidates[i];
			int &nbVoters = LnbVoters[i];
			
			std::cout << "nbCandidates : " << nbCandidates << std::endl;
			std::cout << "nbVoters : " << nbVoters << std::endl;	
		
			std::vector<double> anarchyPrice;
			double maxAnarchy = 0;
			double sumAnarchy = 0;
			for (int lap = 0; lap < 1000; lap++)
			{
				if (lap%100 == 0) std::cout << ".";
				std::vector<std::vector<int>> prefs(nbVoters);
				std::vector<int> sequence;

				randomSequence(sequence, nbCandidates, nbVoters);
				//RandomPrefsGenerate(prefs, nbCandidates, nbVoters);
				MallowsModelGenerate(phi, prefs, nbCandidates, nbVoters );
				/*int const nbCandidates = 4;
				std::cout << "score in parent" << std::endl;
				int const nbVoters = 3;
				int const UCTrepeat = 50;

				std::vector<std::vector<int>> prefs = {{0,1,2,3},
													   {2,0,1,3},
													   {2,1,0,3}};
				std::vector<int> sequence = {0,1,2};*/
				StateGivenSequence etat(nbCandidates, nbVoters, prefs, sequence);
				MCTS<StateGivenSequence> mcts(nbCandidates, nbVoters, false);
				for (int j = 0; j < nbCandidates-1; ++j)
				{
					//std::cout << j << "/" << nbCandidates-2 << std::endl;
					int move = mcts.BestMoveUCT(etat, UCTrepeat, true); //true 'cause mutliplayer
					etat.action(move);
				}

				//displayPrefs(prefs,nbCandidates,nbVoters);

				std::vector<int> finalSeq = etat.getSequence();
				//std::cout << "Sequence : ";
				//etat.displayVec(finalSeq);		
				//std::cout << "Elimination Queue : ";
				std::vector<int> elimQueue = etat.getEliminationQueue();
				//etat.displayVec(elimQueue);
				int bestCandidate, max=0;
				etat.optimumScore(bestCandidate,max);
				//std::cout << "bestCandidate is " << bestCandidate << " and optimumScore = " << max << std::endl;
				int playScore =((State) etat).score();
				//std::cout << "final Candidate is " << etat.getCandidatesLeft()[0] << " score final = " << playScore << std::endl;;
				//=======================================================
				double ratio = (double) max/playScore;
				sumAnarchy += ratio;
				anarchyPrice.push_back(ratio);
				if (maxAnarchy < ratio) maxAnarchy = ratio;
				//std::cout << "°°°°°°°°°ratio = " << ratio << std::endl;
				//std::cout << "================================================" << std::endl;
			}
			std::cout << std::endl;
			auto n = anarchyPrice.size(); 
			double average = 0.0;
			average = sumAnarchy / n;
			std::cout << "final average = " << average << std::endl;
			std::cout << "max ratio = " << maxAnarchy << std::endl;
			double stdvar = 0;
			for (std::size_t k = 0; k < anarchyPrice.size(); ++k)
			{
				stdvar += (average - anarchyPrice[k])*(average - anarchyPrice[k]);
			}
			stdvar = sqrt(stdvar/n);
			std::cout << "std : " << stdvar << std::endl;
			std::cout << "========================================" << std::endl;
		}
		/*for (int nbCandidates = 10; nbCandidates < 100; nbCandidates+=10)
		{
			int loopThrough[] = {nbCandidates/4, nbCandidates/2, 3*nbCandidates/4};
			for (const int &nbVoters : loopThrough)
			//for (int nbVoters = nbCandidates/5; nbVoters < nbCandidates-1; nbVoters+=5)
			{
				//=======================================================
			}
		}*/
	}else if (CONTEXT=="2" && !REPEAT)
	{
		int cpt = 0;
		int lap = 50;
		for (int i = 0; i < lap; ++i)
		{
			std::cout << i << "/" << lap << std::endl;
			int const nbCandidates = 8;
			int const nbVoters = 4;
			std::vector<std::vector<int>> prefs(nbVoters);
			std::vector<int> sequence;

			randomSequence(sequence, nbCandidates, nbVoters);
			RandomPrefsGenerate(prefs, nbCandidates, nbVoters);
			//MallowsModelGenerate(phi, prefs, nbCandidates, nbVoters );
			//std::vector<std::vector<int>> prefs = {{0,1,2,3},
			//									   {2,0,1,3},
			//									   {2,1,0,3}};
			//std::vector<int> sequence = {0,1,2,4,3,2,3,5};
			StateGivenSequence etat(nbCandidates, nbVoters, prefs, sequence);
			MCTS<StateGivenSequence> mcts(nbCandidates, nbVoters, false);
			StateGivenSequence etat2(nbCandidates, nbVoters, prefs, sequence);
			MCTS<StateGivenSequence> mcts2(nbCandidates, nbVoters, true);
			
			std::clock_t start;
			double duration1 = 0.0;
			double duration2 = 0.0;
			int finalWinner;
			std::cout << "start mcts 1" << std::endl;
			
			start = std::clock();
			finalWinner = mcts.MAXN(etat);
			std::cout << "the final winner of MAXN is " << finalWinner << std::endl;
			duration1 = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
			std::cout << "duration == " << duration1 << std::endl;

			std::cout << "start mcts 2" << std::endl;
			start = std::clock();

			finalWinner = mcts2.MAXN(etat2);
			std::cout << "the final winner of MAXN is " << finalWinner << std::endl;
			duration2 = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
			std::cout << "duration ordered == " << duration2 << std::endl;
			std::cout << "////////////////////////////////////////////" << std::endl;
			if (duration2 < duration1) cpt++;
		}

		std::cout << "better " << cpt << "/" << lap << std::endl;

		/*for (int j = 0; j < nbCandidates-1; ++j)
		{
			//std::cout << j << "/" << nbCandidates-2 << std::endl;
			int move = mcts.BestMoveUCT(etat, 1000, true); //true 'cause mutliplayer
			etat.action(move);
		}
		std::cout << "the final winner of UCT is :";
		std::cout << etat.getCandidatesLeft()[0] << std::endl;*/


	}

	if (CONTEXT=="3")
	{
	    int const nbCandidates = 8;
		int const nbVoters = 4;

		int const nbPrefs = 100000;
		int const UCTrepeat = 100;

		std::vector<std::vector<int>> prefs(nbPrefs);

		RandomPrefsGenerate(prefs, nbCandidates, nbPrefs);


		StateCentralAuthority etat(nbCandidates, nbVoters, prefs, nbPrefs);

		MCTS<StateCentralAuthority> mcts(nbCandidates, nbVoters, false);
		std::cout << "nbCandidates : " << nbCandidates << std::endl;
		std::cout << "nbVoters : " << nbVoters << std::endl;
		for (int j = 0; j < nbCandidates-1; ++j)
		{
			std::cout << j << "/" << nbCandidates-2 << std::endl;
			int move = mcts.BestMoveUCT(etat, UCTrepeat);
			etat.action(move);
		}

		std::vector<int> finalSeq = etat.getSequence();
		etat.displayVec(finalSeq);

	}

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