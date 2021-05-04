#ifndef DEF_STATE_CENTRAL
#define DEF_STATE_CENTRAL


#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <cmath>

#include "State.hpp"
//template<class Statistique, class RandomVariable, class Measurement, class RNG>
//void MonteCarlo(Statistique & res, )
class StateCentralAuthority : public State
{

	private :

	//std::vector<int> sequence;		//pi - elimination sequence - maxsize = nbCandidates
	//std::vector<int> candidatesLeft;	//candidate not yet eliminated	
	//int nbCandidates;
	//int nbVoters;
	//std::vector<std::vector<int>> votersPreferences;	// size >> m*n -- maybe use an array rather than a vector 
	int nbPrefs;	//m*n -- maybe use an array rather than a vector
	//uint hash = 0;		//use hash in play function
	//std::vector<std::vector<uint>> hashTable = std::vector<std::vector<uint>>(2);	//hashTable[O] for candidates et [1] for voters


	public :

	//Constructor to determine optimal sequence in general case
	StateCentralAuthority(int _nbCandidates, int _nbVoters, std::vector<std::vector<int>> _votersPreferences, int _nbPrefs)
	{
		std::cout << "start to build StateCentralAuthority" << std::endl;
		srand (time(NULL));
		nbCandidates = _nbCandidates; 
		nbVoters = _nbVoters;
		nbCandidatesLeft = nbCandidates;
		votersPreferences = _votersPreferences;
		nbPrefs = _nbPrefs;
		hashTable = std::vector<std::vector<uint>>(nbCandidates, std::vector<uint>(nbVoters));
		for (int i = 0; i < nbCandidates; ++i)
		{
			candidatesLeft.push_back(i);
			//hashTable[0].push_back(rand() % (1<<15));
			for (int j = 0; j < nbVoters; ++j)
			{
				hashTable[i][j] = rand() % (1<<15);
			}
		}
		/*
		hashTable = std::vector<std::vector<uint>>(2, std::vector<uint>(nbVoters));
		for (int i = 0; i < nbCandidates; ++i)
		{
			candidatesLeft.push_back(i);
			hashTable[0].push_back(rand() % (1<<15));
		}
		for (int j = 0; j < nbVoters; ++j)
		{
			hashTable[1].push_back(rand() % (1<<15));
		}*/
	}

	//int can be changed by using template if borda is not the only rule
	int score() override
	{
		int winner = candidatesLeft[0];
		return score (winner);
	}

	int score(int winner) override
	{
		std::vector<int> votersSelected = std::vector<int>();
		for (int i = 0; i < nbVoters; ++i)
		{
			votersSelected.push_back(rand() % nbPrefs);
		}
		return score(winner, votersSelected);
	}

	int score(int winner, std::vector<int> votersSelected)
	{
		int res = 0;
		for (int i = 0; i < nbVoters; ++i)
		{
			res += votersPreferences[votersSelected[i]][winner];		// borda score
		}
		return res;
	}

	//playout with voters selected among all voters available
	int playout() override
	{
		if (candidatesLeft.size() < 2) return score(candidatesLeft[0]);
		std::vector<int> candidatesLeftCopy = candidatesLeft;	
		std::vector<int> votersSelected;
		for (int i = 0; i < nbVoters; ++i)
		{
			votersSelected.push_back(rand() % nbPrefs);
		}
		//for completing the sequence
		for (int i = sequence.size() - 1; i < nbCandidates-2; ++i)	//maybe nbCandidates - 1
		{
			int votersToChose = votersSelected[rand() % nbVoters];

			eraseLeastPreferedSincere(votersToChose, candidatesLeftCopy);
		}
		return score(candidatesLeftCopy[0], votersSelected);
	}

	StateCentralAuthority getChild(int index)
	{
		StateCentralAuthority *sCopy = new StateCentralAuthority(*this);
		(*sCopy).action(index);
		return *sCopy;
	}
};

#endif
