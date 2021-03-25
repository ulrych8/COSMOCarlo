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
	std::vector<std::vector<uint>> hashTable = std::vector<std::vector<uint>>(2);	//hashTable[O] for candidates et [1] for voters
	*/
	int indexCurrentCandidateToVote = 0;
	std::vector<int> eliminationQueue;

	public :

	//CONTEXT 2
	//Given a Sequence find the choice of each voter
	StateGivenSequence(int _nbCandidates, int _nbVoters, std::vector<std::vector<int>> _votersPreferences, std::vector<int> _sequence)
	{
		std::cout << "start to build StateGivenSequence" << std::endl;
		srand (time(NULL));
		nbCandidates = _nbCandidates; 
		nbVoters = _nbVoters;
		votersPreferences = _votersPreferences;
		sequence = _sequence;
		for (int i = 0; i < nbCandidates; ++i)
		{
			candidatesLeft.push_back(i);
			hashTable[0].push_back(rand() % (1<<15));
		}
		for (int i = 0; i < nbVoters; ++i)
		{
			hashTable[1].push_back( rand() % (1<<15));
		}
	}

	int playout() override		//override to return winner and not score
	{
		if (candidatesLeft.size() < 2) return candidatesLeft[0];
		std::vector<int> candidatesLeftCopy = candidatesLeft;	
		
		//for completing the sequence
		for (int i = indexCurrentCandidateToVote; i < nbCandidates-2; ++i)	//maybe nbCandidates - 1
		{
			eraseLeastPreferedSincere(sequence[i], candidatesLeftCopy);
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
		candidatesLeft.erase(candidatesLeft.begin()+candidateToElminate);
		indexCurrentCandidateToVote ++;
		int voter = sequence[indexCurrentCandidateToVote];
		hash ^= hashTable[0][candidateElminated] * hashTable[1][voter];
	}

	void action( int choice ) override
	{
		eliminateCandidate(choice);
	}

	int score(int candidate) override
	//this score return the evaluation of the candidate by the current voter
	{
		int currentVoter = sequence[indexCurrentCandidateToVote];
		int index;
		findElement(votersPreferences[currentVoter],candidate,index);
		return nbCandidates - index -1;
	}

	int getNbChild() const override
	{
		return candidatesLeft.size()-1;
	}

	std::vector<int> getEliminationQueue()
	{
		return eliminationQueue;
	}
};

#endif
