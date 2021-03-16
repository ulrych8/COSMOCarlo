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
	std::vector<int> candidatesLeft;		
	int nbCandidates;
	int nbVoters;
	std::vector<std::vector<int>> votersPreferences;	//m*n -- maybe use an array rather than a vector
	uint hash = 0;		//use hash in play function
	std::vector<uint> hashTable;

	public :

	State(int _nbCandidates, int _nbVoters, std::vector<std::vector<int>> _votersPreferences){
		nbCandidates = _nbCandidates; 
		nbVoters = _nbVoters;
		votersPreferences = _votersPreferences;
		for (int i = 0; i < nbCandidates; ++i)
		{
			candidatesLeft.push_back(i);
			hashTable.push_back( 1 << i );
		}
		srand (time(NULL));
		//displayVec(hashTable);
	}

	//int can be changed by using template
	int score(int winner=-1)
	{
		if (winner<0){
			winner = candidatesLeft[0];
		}
		int res = 0;
		int index;
		for (int i = 0; i < nbVoters; ++i)
		{
			findElement(votersPreferences[i],winner,index);
			res += nbCandidates-index-1;		// borda score
		}
		return res;
	}

	//if score type changed with template change this one to
	int playout()	//sincere playout
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

	int optimumScore(){
		int max = 0;
		int currenScore;
		for (int i = 0; i < nbCandidates; ++i)
		{
			currenScore = score(i);
			if (currenScore > max) max = currenScore;
		}
		return max;
	}

	void addVoterToSequence(int voter){
		sequence.push_back(voter);
		int candidateElminate = eraseLeastPreferedSincere(voter, candidatesLeft);
		//change hash
		hash ^= hashTable[candidateElminate];
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

	template <typename T>
	void displayVec(std::vector<T> &v) const{
		for (std::size_t i = 0; i < v.size(); ++i)
		{
			std::cout << v[i] << " ";
		}
		std::cout << std::endl;
	}

	bool terminal() const{
		if (candidatesLeft.size()>1) return false;
		return true;
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
