#include <iostream>
#include <string>
#include <algorithm>	//shuffle
#include <stdlib.h>		//random

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


	int const nbCandidates = 9;
	int const nbVoters = 3;
	double const phi = 0.2;
	
	/*std::vector<std::vector<int>> prefs { {1, 3, 2, 4, 0},
										 {4, 3, 2, 1, 0},
										 {2, 0, 3, 1, 4} };*/

	std::vector<std::vector<int>> prefs(nbVoters);
	
	std::cout << "start mallows" << std::endl;

	MallowsModelGenerate(phi, prefs, nbCandidates, nbVoters );
	std::cout << "show" << std::endl;
	displayPrefs(prefs, nbCandidates, nbVoters);

	std::cout << " start mcts " << std::endl;

	State etat(nbCandidates, nbVoters, prefs);

	std::cout << "State created" << std::endl;

	MCTS mcts;

	for (int i = 0; i < nbCandidates-1; ++i)
	{
		//std::cout << "start bestuct" << std::endl;
		int move = mcts.BestMoveUCT(etat, 10);
		//std::cout << "selon uct le bset move to play is : " << move << std::endl;
		etat.addVoterToSequence(move);
	}
	std::cout << "The final sequence is ";
	std::vector<int> res;
	res = etat.getSequence();
	etat.displayVec(res);

	//int sbdugagnant = etat.playout();

	//std::cout << "point du gagnant : " << sbdugagnant << std::endl;
	return 0;
}