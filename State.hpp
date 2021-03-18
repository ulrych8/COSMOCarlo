#ifndef DEF_STATE
#define DEF_STATE


#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <cmath>
//template<class Statistique, class RandomVariable, class Measurement, class RNG>
//void MonteCarlo(Statistique & res, )
class State 
{

	private :

	std::vector<int> sequence;		//pi - elimination sequence - maxsize = nbCandidates
	std::vector<int> candidatesLeft;	//candidate not yet eliminated	
	int nbCandidates;
	int nbVoters;
	std::vector<std::vector<int>> votersPreferences;	// size >> m*n -- maybe use an array rather than a vector
	int nbPrefs;	//m*n -- maybe use an array rather than a vector
	uint hash = 0;		//use hash in play function
	std::vector<std::vector<uint>> hashTable = std::vector<std::vector<uint>>(2);	//hashTable[O] for candidates et [1] for voters


	public :

	State(int _nbCandidates, int _nbVoters, std::vector<std::vector<int>> _votersPreferences, int _nbPrefs){
		srand (time(NULL));
		nbCandidates = _nbCandidates; 
		nbVoters = _nbVoters;
		votersPreferences = _votersPreferences;
		nbPrefs = _nbPrefs;
		for (int i = 0; i < nbCandidates; ++i)
		{
			candidatesLeft.push_back(i);
			hashTable[0].push_back(rand() % (1<<(nbVoters+nbCandidates)));
		}
		for (int i = 0; i < nbVoters; ++i)
		{
			hashTable[1].push_back( rand() % (1<<(nbVoters+nbCandidates)));
		}
		//displayVec(hashTable[1]);
	}

	//int can be changed by using template if borda is not the only rule
	int score(int winner=-1, std::vector<int> votersSelected = {})
	{
		if (votersSelected.size() == 0)
		{
			votersSelected = std::vector<int>();
			for (int i = 0; i < nbVoters; ++i)
			{
				votersSelected.push_back(rand() % nbPrefs);
			}
		}
		if (winner<0){
			winner = candidatesLeft[0];
		}
		int res = 0;
		int index;
		for (int i = 0; i < nbVoters; ++i)
		{
			findElement(votersPreferences[votersSelected[i]],winner,index);
			res += nbCandidates-index-1;		// borda score
		}
		//std::cout << " res = " << res << std::endl;
		return res;
	}

	//playout only with votersIndex < nbVoters
	int focusPlayout()	//sincere playout 
	{
		if (candidatesLeft.size() < 2) return score(candidatesLeft[0]);
		std::vector<int> candidatesLeftCopy = candidatesLeft;
		//for completing the sequence
		for (int i = sequence.size() - 1; i < nbCandidates-2; ++i)	//maybe nbCandidates - 1
		{
			int votersToChose = rand() % nbVoters;

			eraseLeastPreferedSincere(votersToChose, candidatesLeftCopy);
		}
		return score(candidatesLeftCopy[0]);
	}

	//playout with voters selected among all voters available
	int playout()
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

	//true if element find, index becomes the index of the element
	bool findElement(std::vector<int> &v, int key, int &index){
		for (std::size_t i = 0; i < v.size(); ++i)
		{
			if (v[i] == key){
				index = i;
				return true;
			}
		}
		return false;
	}

	//maximal score reachable, change value of its argument
	void optimumScore(int &bestCandidate, int &max){
		int currenScore;
		for (int i = 0; i < nbCandidates; ++i)
		{
			currenScore = score(i);
			if (currenScore > max)
			{
				max = currenScore;
				bestCandidate = i;
			}
		}
	}

	//add voter to the final sequence and erase his sincere worst candidate
	void addVoterToSequence(int voter){
		sequence.push_back(voter);
		int candidateElminate = eraseLeastPreferedSincere(voter, candidatesLeft);
		//change hash
		hash ^= hashTable[0][candidateElminate] ^ hashTable[1][voter] ;
	}

	int eraseLeastPreferedSincere(int voter, std::vector<int> &candidatesLeft){
		//for going through voter preference backwards
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
		throw "shouldn't be able to reach here";
	}

	//display a given vector
	template <typename T>
	void displayVec(std::vector<T> &v) const{
		for (std::size_t i = 0; i < v.size(); ++i)
		{
			std::cout << v[i] << " ";
		}
		std::cout << std::endl;
	}

	//true if only there is only one candidate left
	bool terminal() const{
		if (candidatesLeft.size()>1) return false;
		return true;
	}

	//given a sequence eliminate round by round the least 
	//prefered candidate for each voter
	int executeSincereSequence( std::vector<int> &v ) {
		std::vector<int> candidatesLeft;
		for (std::size_t i = 0; i < v.size()+1; ++i)
		{
			candidatesLeft.push_back(i);
		}
		for (std::size_t i = 0; i < v.size(); ++i)
		{
			eraseLeastPreferedSincere(v[i], candidatesLeft);
		}
		return candidatesLeft[0];
	}

	uint getHash() const{
		return hash;
	}

	int getNbVoters() const{
		return nbVoters;
	}

	int getNbCandidates() const{
		return nbCandidates;
	}

	std::vector<int> getCandidatesLeft(){
		return candidatesLeft;
	}

	std::vector<int> getSequence() const{
		return sequence;
	}

};

#endif
