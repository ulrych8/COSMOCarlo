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
	int lastEliminatedCandidate=-1;
	std::vector<int> eliminationQueue;

	public :

	//CONTEXT 2
	//Given a Sequence find the choice of each voter
	StateGivenSequence( int _nbCandidates, 
						int _nbVoters, 
						std::vector<std::vector<int>> _votersPreferences, 
						std::vector<int> _sequence)
	{
		//srand (time(NULL));
		nbCandidates = _nbCandidates; 
		nbVoters = _nbVoters;
		nbCandidatesLeft = _nbCandidates;
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

	int playout(bool addEliminationQueue)		//override to return winner and not score
	{
		if (nbCandidatesLeft < 2)
		{
			//State::displayVec(candidatesLeft);
			return candidatesLeft[0];
		}
		std::vector<int> candidatesLeftCopy = candidatesLeft;	
		
		//for completing the sequence
		for (int i = indexCurrentCandidateToVote; i < nbCandidates-1; ++i)	//maybe nbCandidates - 1
		{
			int candidateElminated = State::eraseLeastPreferedSincere(sequence[i], candidatesLeftCopy);
			if (addEliminationQueue)
			{
				eliminationQueue.push_back(candidateElminated);
				//std::cout << "blaf " << candidateElminated << " euh" << std::endl;
			}
		}
		return candidatesLeftCopy[0];
	}

	int playout() override		//override to return winner and not score
	{
		if (nbCandidatesLeft < 2) return candidatesLeft[0];
		std::vector<int> candidatesLeftCopy = candidatesLeft;	
		
		//for completing the sequence
		for (int i = indexCurrentCandidateToVote; i < nbCandidates-1; ++i)	//maybe nbCandidates - 1
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
		//eliminate candidate of index candidateToEliminate in candidatesLeft
		int candidateElminated = candidatesLeft[candidateToElminate];
		lastEliminatedCandidate = candidateElminated;

		eliminationQueue.push_back(candidateElminated);
		//std::cout << "candidatesLeft size : " << nbCandidatesLeft << " | must eliminate : " << candidateToElminate << std::endl;
		candidatesLeft.erase(candidatesLeft.begin()+candidateToElminate);

		int voter = getCurrentVoter();

		hash ^= hashTable[candidateElminated][voter];

		indexCurrentCandidateToVote ++;
		nbCandidatesLeft--;
	}

	int eraseLeastPreferedSincere() override{
		int voter = getCurrentVoter();
		int min = 10000000;
		int leastPreferedCandidate = -1;
		int indexLeastPreferedCandidate = -1;
		for (int i = 0; i < nbCandidatesLeft; ++i)
		{
			int & currentCandidate = candidatesLeft[i];
			if (votersPreferences[voter][currentCandidate] < min)
			{
				min = votersPreferences[voter][currentCandidate];
				leastPreferedCandidate = currentCandidate;
				indexLeastPreferedCandidate = i;
			}
		}
		candidatesLeft.erase(candidatesLeft.begin()+indexLeastPreferedCandidate);
		nbCandidatesLeft--;
		return leastPreferedCandidate;
	}

	void action( int choice) override
	{
		eliminateCandidate(choice);
	}

	int score(int candidate) override
	{
		int currentVoter = getCurrentVoter();
		return votersPreferences[currentVoter][candidate];
	}

	int scoreMaxForVoter(int voter)
	{
		int max = -1;
		for (int i = 0; i < nbCandidatesLeft; ++i)
		{
			if (votersPreferences[voter][candidatesLeft[i]] > max)
			{
				max = votersPreferences[voter][candidatesLeft[i]];
			}
		}
		return max;
	} 

	int scoreMinForVoter(int voter)
	{
		int min = 10000000;
		for (int i = 0; i < nbCandidatesLeft; ++i)
		{
			if (votersPreferences[voter][candidatesLeft[i]] < min)
			{
				min = votersPreferences[voter][candidatesLeft[i]];
			}
		}
		return min;
	} 

	int scoreMaxForCurrentVoter()
	{
		int currentVoter = getCurrentVoter();
		return scoreMaxForVoter(currentVoter);
	}

	//score max reachable with candidates left for voter of index 'indexVoter' in th seqeunce
	int scoreMaxVoterSeqIndex(int indexVoter)
	{
		int voter = sequence[indexVoter];
		return scoreMaxForVoter(voter);
	}


	void orderCandidatesLeft()
	{
		int currentVoter = getCurrentVoter();
		orderCandidatesLeftForVoter(currentVoter);
	}

	//previously orderCandidatesLeftOnlyOnce
	void orderCandidatesLeftForVoter(int voter)
	{
		std::vector<int> candidatesLeftOrdered;
		std::vector<int> scoresVoter = candidateToScoreForVoter(candidatesLeft, voter);
		int index;
		for (int i = 0; i < nbCandidatesLeft; ++i)
		{
			findWorstElement(scoresVoter, index);
			candidatesLeftOrdered.push_back(candidatesLeft[index]);
			candidatesLeft.erase(candidatesLeft.begin()+index);
			scoresVoter.erase(scoresVoter.begin()+index);
		}
		candidatesLeft = candidatesLeftOrdered;

	}

	void orderCandidatesLeftWithHistory(std::vector<std::vector<uint>> histTable)
	{
		int currentVoter = getCurrentVoter(); 
		std::map<int,uint> m;
		for (int i=0; i < nbCandidates; i++) m[i] = histTable[currentVoter][i];
		std::vector<int> candidatesLeftOrdered;
		for (int i = 0; i < nbCandidatesLeft; ++i)
		{
			beginAgain:
				std::map<int, uint>::iterator it;
				int bestCandidate = findBestElement(m,it);
				m.erase(it);
				
				int index;
				if (findElement(candidatesLeft, bestCandidate, index))
				{
					candidatesLeftOrdered.push_back(bestCandidate);
				}else
				{
					goto beginAgain;
				}
		}
		candidatesLeft = candidatesLeftOrdered;
	}

	void orderCandidatesLeftWithKiller(int killValue)//, int& cpteur, int& cpteurUtile)
	{
		int index;
		if (findElement(candidatesLeft, killValue, index))
		{
			candidatesLeft.erase(candidatesLeft.begin()+index);
			candidatesLeft.insert(candidatesLeft.begin(),killValue);
		}

	}

	std::vector<int> candidateToScoreForVoter(std::vector<int> candidates, int voter)
	{
		std::vector<int> scoresVoter;
		for (size_t i = 0; i < candidates.size(); ++i)
		{
			scoresVoter.push_back(votersPreferences[voter][candidates[i]]);
		}
		return scoresVoter;
	}

	int getCurrentVoter() const
	{
		return sequence[indexCurrentCandidateToVote];
	}

	int getNbChild() const override
	{
		return nbCandidatesLeft;
	}

	std::vector<int> getEliminationQueue()
	{
		return eliminationQueue;
	}

	StateGivenSequence getChild(int index) 
	{
		StateGivenSequence sCopy = * this;//new StateGivenSequence(*this);
		sCopy.action(index);
		return sCopy;
	}

	int getLastEliminatedCandidate() const
	{
		return lastEliminatedCandidate;
	}
};

#endif
