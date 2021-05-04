#include <iostream>
#include <string>
#include <algorithm>	//shuffle
#include <stdlib.h>		//random
#include <ctime>		//clock
#include <fstream>		//file
#include <iomanip>		//pretty print

   
#include "StatePreferenceVariation.hpp"
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

int factorial( int n)
{
	if (n==1) return 1;
	return (n*factorial(n-1));
}


//====================================================================
//
//						M A I N
//
//====================================================================

struct EXP{
	std::string const heuristic;
	int cpt;
	double mean;
};


int main(){
	srand (time(NULL));

	std::string CONTEXT = "TEST";
	//"2 - candidates and voters change size"
	//"SAVE"
	//"2 - heuristic variration"
	//"3" 

	int const UCTrepeat = 50;


	if (CONTEXT=="TEST"){
		int nbCandidates = 8;
		int nbVoters = 2;
		std::vector<std::vector<int>> seqPossible = {
								{0, 1, 1, 0, 1, 0, 0},
								{0, 0, 1, 1, 1, 0, 0},
								{0, 1, 0, 1, 0, 0, 1},
								{0, 1, 1, 0, 0, 1, 0},
								{0, 0, 1, 1, 0, 0, 1},
								{0, 0, 1, 1, 1, 1, 0},
								{0, 0, 1, 0, 0, 0, 1},
								{0, 1, 0, 0, 0, 1, 1},
								{0, 0, 0, 1, 1, 1, 1},
								{0, 0, 0, 0, 1, 1, 1},
								//{0, 0, 0, 0, 0, 1, 1},
								//{0, 0, 0, 0, 0, 0, 1},
								{0, 1, 0, 1, 1, 0, 0},
								{0, 1, 1, 1, 0, 0, 1},
								{0, 0, 1, 0, 0, 1, 0}, 
								{0, 0, 1, 0, 1, 0, 0},
								{0, 1, 0, 1, 0, 1, 0},
								{0, 0, 1, 1, 0, 1, 0},
								{0, 0, 1, 1, 0, 1, 1},
								{0, 1, 1, 0, 1, 0, 0},
								{0, 1, 0, 0, 1, 0, 1},
		};

		for (size_t i = 0; i < seqPossible.size(); ++i)
		{
			std::vector<int> seq=seqPossible[i];
			double meanRatio = 0.0;
			std::cout << "start .... " << (i+1) << "/" << seqPossible.size() << std::endl;
			
			std::clock_t start = std::clock();;
		    double duration = 0.0;

			StatePreferenceVariation prefVar(nbCandidates, nbVoters);

			std::vector<std::vector<int>> prefs;
			std::vector<std::vector<int>> worstPrefs;
			std::vector<std::vector<int>> bestPrefs;
			std::vector<int> worstElimQueue;
			std::vector<int> bestElimQueue;
			int worstWinnerStrat;
			int bestWinnerStrat;
			int worstWinnerSincere;
			int bestWinnerSincere;
			std::vector<int> v;
			for (int i = 0; i < nbCandidates; ++i) v.push_back(i);
			prefs.push_back(v);
			prefs.push_back(v);
			//prefs[1] = v;
			int socialWelfare = 1000;
			double worstStrategicDivBySincere = 100;
			double bestStrategicDivBySincere = 0;
			//displayPrefs(prefs, nbCandidates, nbVoters);
			int combinaisons = factorial(8);
			int lap = 1;
			do{
				StateGivenSequence state(nbCandidates,nbVoters,prefs,seq);
				MCTS<StateGivenSequence> mcts(nbCandidates, nbVoters, "simple order + simple pruning + cut -1", prefs);
				tupleResult res = mcts.MAXN(state,std::vector<int>(nbCandidates,-1));
				// MAXN 
				// if (prefs[0][res.winner] + prefs[1][res.winner] < socialWelfare)
				// {
				// 	socialWelfare = prefs[0][res.winner] + prefs[1][res.winner];
				// 	worstWinner = res.winner;
				// 	worstPrefs = prefs;
				// }
				// MIN
				// if (std::min(prefs[0][res.winner], prefs[1][res.winner]) < socialWelfare)
				// {
				// 	socialWelfare = std::min(prefs[0][res.winner], prefs[1][res.winner]);
				// 	worstWinner = res.winner;
				// 	worstPrefs = prefs;
				// }
				//Playout
				// int winner = state.playout();
				// if (prefs[0][winner] + prefs[1][winner] < socialWelfare)
				// {
				// 	socialWelfare = prefs[0][winner] + prefs[1][winner] ;
				// 	worstWinner = winner;
				// 	worstPrefs = prefs;
				// }
				StateGivenSequence state2(nbCandidates,nbVoters,prefs,seq);
				int winner = state2.playout();

				double rapport = (prefs[0][res.winner] + prefs[1][res.winner])/(double)(prefs[0][winner] + prefs[1][winner]);
				if (rapport < worstStrategicDivBySincere)
				{
					worstStrategicDivBySincere = rapport;
				 	worstWinnerSincere = winner;
				 	worstWinnerStrat = res.winner;
				 	worstPrefs = prefs;
				 	//worstElimQueue = 
				}
				if (rapport > bestStrategicDivBySincere)
				{
					bestStrategicDivBySincere = rapport;
				 	bestWinnerSincere = winner;
				 	bestWinnerStrat = res.winner;
				 	bestPrefs = prefs;
				}
				meanRatio += rapport;
				std::cout << '\r' <<  std::setw(5) << std::setfill('0') << lap << "/" << combinaisons << std::flush;
				lap++;
			}while(prefVar.next_permutation(prefs[1].begin(), prefs[1].end()));
			std::cout << std::endl;
			
			duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
	        std::cout << "duration : " << duration << std::endl;
			
			std::cout << "sequence = ";
			State::displayVec(seq);
			//std::cout << "worst socialWelfare = " << socialWelfare << std::endl;
			meanRatio /= combinaisons;
			std::cout << "mean ratio = " << meanRatio << std::endl;
			std::cout << "---------------------------------------" << std::endl;
			std::cout << "best ratio = " << bestStrategicDivBySincere << std::endl;
			displayPrefs(bestPrefs, nbCandidates, nbVoters);
			std::cout << "winner strategic is candidate "<< bestWinnerStrat << " ---> score = " << bestPrefs[0][bestWinnerStrat] + bestPrefs[1][bestWinnerStrat] << std::endl;
			std::cout << "winner sincere is candidate "<< bestWinnerSincere << " ---> score = " << bestPrefs[0][bestWinnerSincere] + bestPrefs[1][bestWinnerSincere] << std::endl;
			std::cout << "---------------------------------------" << std::endl;
			std::cout << "worst ratio = " << worstStrategicDivBySincere << std::endl;
			displayPrefs(worstPrefs, nbCandidates, nbVoters);
			std::cout << "winner strategic is candidate "<< worstWinnerStrat << " ---> score = " << worstPrefs[0][worstWinnerStrat] + worstPrefs[1][worstWinnerStrat] << std::endl;
			std::cout << "winner sincere is candidate "<< worstWinnerSincere << " ---> score = " << worstPrefs[0][worstWinnerSincere] + worstPrefs[1][worstWinnerSincere] << std::endl;
			std::cout << "=======================================" << std::endl;
		
		}
	}
	//SAVE DATA
	else if (CONTEXT=="SAVE")
	{
		std::string const nomFichier("/home/ulysse/Documents/Stage/Code/COSMOCarlo/durationData.txt");
		std::ofstream monFlux(nomFichier.c_str());

		std::clock_t start;
	    double duration = 0.0;
	    double mean_duration = 0.0;

	    if(monFlux)    
	    {
	    	int nbCandidates = 0;
			int nbVoters = 0;
			std::vector<int> seq;
	        monFlux << "#IC - 'simple pruning + simple order' " << std::endl;
	        monFlux << "nbCandidates , nbVoters , duration" << std::endl;

			for (nbVoters = 2; nbVoters < 20; ++nbVoters)
			{
				mean_duration = 0.0;
				nbCandidates = 3/2*nbVoters;
				while (mean_duration < 50.0){
					++nbCandidates;
					std::cout << "===========================================" << std::endl;
					mean_duration = 0.0;
					std::cout << nbCandidates << "," << nbVoters << ": ";
					for (int k = 0; k < 10; ++k)
					{
						std::cout << (k+1) << "/10 : "; 
						std::vector<std::vector<int>> prefs(nbVoters);
						std::vector<int> sequence;

						randomSequence(sequence, nbCandidates, nbVoters);
						RandomPrefsGenerate(prefs, nbCandidates, nbVoters);
						
						start = std::clock();
					
						StateGivenSequence state(nbCandidates,nbVoters,prefs,sequence);
						MCTS<StateGivenSequence> mcts(nbCandidates, nbVoters, "simple pruning + simple order");

						tupleResult res = mcts.MAXN(state,std::vector<int>(nbCandidates,-1));

						duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
	        			std::cout << duration << ", " << std::endl;
						mean_duration += duration;
		        		
					}
					mean_duration/=10.0;
	        		std::cout << nbCandidates << "," << nbVoters << "," << mean_duration << std::endl;
	        		monFlux << nbCandidates << "," << nbVoters << "," << mean_duration << std::endl;
				}
			}
	    } else
	    {
	        std::cout << "ERREUR: Impossible d'ouvrir le fichier." << std::endl;
	    }
	}


	if (CONTEXT == "2 - candidates and voters change size")
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
				MCTS<StateGivenSequence> mcts(nbCandidates, nbVoters);
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
	}else if (CONTEXT=="2 - heuristic variration")
	{
		std::vector<std::string> listHeuristics = {"basic", 	//don't do nothing
									//"vanilla_basic no cut",
									"simple order", 
									"simple order -1", 
									"simple pruning", 
									"cut",
									//"killer",
									//"history",
									//"complex pruning", 
									//"ordered complex pruning" ,
									//"simple order + simple pruning", 
									//"simple order + cut", 
									//"simple order + simple pruning + cut", 
									//"simple pruning + killer", 
									"simple order + simple pruning + cut",
									"simple order + simple pruning + cut -1",
									//"simple order + simple pruning + killer",
									//"simple order + simple pruning + killer + simple cut",
									//"simple pruning + cut + history",
									//"ordered complex pruning + simple order + simple pruning + cut" ,
								};
		/*List of heuristics :
		""   ,   "simpleOrder",  "killer"  ,   "history"   ,   "killer+simpleOrder";
		"order only once"    ,  "alphaBeta pruning"
		*/
		
		std::vector<EXP> listExp; 
		for (size_t i = 0; i < listHeuristics.size(); ++i)
		{
			EXP newExp = {listHeuristics[i], 0 , 0.0}; 
			//newExp.heuristic = listHeuristics[i];
			//newExp.cpt = 0;
			//newExp.mean = 0.0;
			listExp.push_back(newExp);
		}
		int const nbCandidates = 10;
		int const nbVoters = 5;
		
		int const lap = 10;

		for (int i = 0; i < lap; ++i)
		{
			std::cout << (i+1) << "/" << lap;
			std::cout << "   nbCandidates : "<<nbCandidates<<" nbVoters : "<< nbVoters << std::endl;
			std::vector<std::vector<int>> prefs(nbVoters);
			std::vector<int> sequence;
			randomSequence(sequence, nbCandidates, nbVoters);
			RandomPrefsGenerate(prefs, nbCandidates, nbVoters);
			
			displayPrefs(prefs, nbCandidates, nbVoters);
			std::cout << "-------" << std::endl;
			//MallowsModelGenerate(phi, prefs, nbCandidates, nbVoters );
			//std::vector<std::vector<int>> prefs = {{0,1,2,3},
			//									   {2,0,1,3},
			//									   {2,1,0,3}};
			//std::vector<int> sequence = {0,1,2,4,3,2,3,5};
			std::clock_t start;
			int compareWinner=0;

			for (size_t j = 0; j < listExp.size(); ++j)
			{
				EXP & currentExp = listExp[j];
				StateGivenSequence state(nbCandidates,nbVoters,prefs,sequence);
				MCTS<StateGivenSequence> mcts(nbCandidates, nbVoters, currentExp.heuristic, prefs);
				
				if (j==0){
					state.displayVec(sequence);
					std::cout << "-------" << std::endl;
				}

				double duration = 0.0;
				
				if (currentExp.heuristic == "order only once")
					state.orderCandidatesLeftForVoter(sequence.back());

				std::cout << "start mcts " << currentExp.heuristic << std::endl;
				int finalWinner = -1;
				std::vector<int> candidEliminatedSeq;
				start = std::clock();

				tupleResult res = mcts.MAXN(state,std::vector<int>(nbCandidates,-1));
				finalWinner = res.winner;
				state.displayVec(res.elSeq);

				duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
				currentExp.cpt += mcts.cpteur;
				std::cout << "duration : " << duration << std::endl;
				//mcts.seeFinal();
				std::cout << "-------------------------------------------------> finalWinner = " << finalWinner << std::endl;
				currentExp.mean += duration;
				std::cout << std::endl;
				if (j>0 && compareWinner!=finalWinner){
					throw "there !";
				}
				compareWinner = finalWinner;
			}

			std::cout << "nbCandidates : "<<nbCandidates<<", nbVoters : "<< nbVoters << "\n" << std::endl;

			for (size_t j = 0; j < listExp.size(); ++j)
			{
				EXP currentExp = listExp[j];
				std::cout << "mean " << currentExp.heuristic << " = " << currentExp.mean/(double)(i+1) << std::endl;
				std::cout << "cpt maxn call = " << currentExp.cpt <<"\n"<< std::endl;
			}

			std::cout << std::endl;
			
			//std::cout << ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: " << std::endl;
			std::cout << "==================================================================== " << std::endl;

			std::cout << std::endl;
		}
	}

	else if (CONTEXT=="3")
	{
	    int const nbCandidates = 8;
		int const nbVoters = 2;

		int const nbPrefs = 100000;
		int const UCTrepeat = 15;

		std::vector<std::vector<int>> prefs(nbPrefs);

		RandomPrefsGenerate(prefs, nbCandidates, nbPrefs);


		StateCentralAuthority etat(nbCandidates, nbVoters, prefs, nbPrefs);

		MCTS<StateCentralAuthority> mcts(nbCandidates, nbVoters);
		std::cout << "nbCandidates : " << nbCandidates << std::endl;
		std::cout << "nbVoters : " << nbVoters << std::endl;
		//we always start with voter 0	
		etat.action(0);
		for (int j = 1; j < nbCandidates-1; ++j)
		{
			std::cout << (j+1) << "/" << nbCandidates-1 << std::endl;
			int move = mcts.BestMoveUCT(etat, UCTrepeat);
			std::cout << " - " << std::endl;
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