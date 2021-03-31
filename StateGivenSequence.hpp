#ifndef DEF_STATE_SEQUENCE
#define DEF_STATE_SEQUENCE


#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <cmath>

#include "State.hpp"
//template<class Statistique, class RandomVariable, class Measurement, class RNG>
//void MonteCarlo(Statistique & res, )
class StateGivenSequence : public State
{

	private :

	/*std::vector<int> sequence;		//pi - elimination sequence - maxsize = nbCandidates
	std::vector<int> candidatesLeft;	//candidate not yet eliminated	
	int nbCandidates;
	int nbVoters;
	std::vector<std::vector<int>> votersPreferences;	// size >> m*n -- maybe use an array rather than a vector
	int nbPrefs;	//m*n -- maybe use an array rather than a vector
	uint hash = 0;		//use hash in play function
	std::vector<std::vector<uint>> hashTable;  	//hashTable[O] for candidates et [1] for voters
	*/
	int indexCurrentCandidateToVote = 0;
	std::vector<int> eliminationQueue;

	public :

	//CONTEXT 2
	//Given a Sequence find the choice of each voter
	StateGivenSequence(int _nbCandidates, int _nbVoters, std::vector<std::vector<int>> _votersPreferences, std::vector<int> _sequence)
	{
		srand (time(NULL));
		nbCandidates = _nbCandidates; 
		nbVoters = _nbVoters;
		votersPreferences = _votersPreferences;
		sequence = _sequence;
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
	}

	int playout() override		//override to return winner and not score
	{
		if (candidatesLeft.size() < 2) return candidatesLeft[0];
		std::vector<int> candidatesLeftCopy = candidatesLeft;	
		
		//for completing the sequence
		for (int i = indexCurrentCandidateToVote; i < nbCandidates-2; ++i)	//maybe nbCandidates - 1
		{
			State::eraseLeastPreferedSincere(sequence[i], candidatesLeftCopy);
		}
		return candidatesLeftCopy[0];
	}

	int result() override
	{
		return candidatesLeft[0];
	}

	void eliminateCandidate(int candidateToElminate){
		int candidateElminated = candidatesLeft[candidateToElminate];

		eliminationQueue.push_back(candidateElminated);
		//std::cout << "candidatesLeft size : " << candidatesLeft.size() << " | must eliminate : " << candidateToElminate << std::endl;
		candidatesLeft.erase(candidatesLeft.begin()+candidateToElminate);

		int voter = getCurrentVoter();

		hash ^= hashTable[candidateElminated][voter];

		indexCurrentCandidateToVote ++;
	}

	int eraseLeastPreferedSincere() override{
		int voter = getCurrentVoter();
		for (int j = nbCandidates-1; j >=0; --j)
		{
			int index;
			//checking if voter's preference can be discarded
			if (findElement(candidatesLeft, votersPreferences[voter][j], index))
			{
				int candidateToElminate = candidatesLeft[index];
				candidatesLeft.erase(candidatesLeft.begin()+index);
				return candidateToElminate;
			}
		}
		throw "Shouldn't be able to reach here";
	}

	void action( int choice ) override
	{
		eliminateCandidate(choice);
	}

	int score(int candidate) override
	//this score return the evaluation of the candidate by the current voter - 1
	//-1 is because we already took action and changed current player
	{
		int currentVoter = getCurrentVoter();
		int index;
		findElement(votersPreferences[currentVoter],candidate,index);
		return nbCandidates - index -1;
	}

	int getCurrentVoter(/*int recoil=0*/) const
	{
		return sequence[indexCurrentCandidateToVote /*- recoil*/];
	}

	int getNbChild() const override
	{
		return candidatesLeft.size();
	}

	std::vector<int> getEliminationQueue()
	{
		return eliminationQueue;
	}

	StateGivenSequence getChild(int index) 
	{
		StateGivenSequence *sCopy = new StateGivenSequence(*this);
		(*sCopy).action(index);
		return *sCopy;
	}
};

#endif
