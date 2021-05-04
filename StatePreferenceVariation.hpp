#ifndef DEF_STATE_PREF
#define DEF_STATE_PREF


#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <cmath>

#include "State.hpp"
//template<class Statistique, class RandomVariable, class Measurement, class RNG>
//void MonteCarlo(Statistique & res, )
class StatePreferenceVariation : public State
{

	private :

	//std::vector<int> sequence;		//pi - elimination sequence - maxsize = nbCandidates
	//std::vector<int> candidatesLeft;	//candidate not yet eliminated	
	//int nbCandidates;
	//int nbVoters;
	//std::vector<std::vector<int>> votersPreferences;	// size >> m*n -- maybe use an array rather than a vector 
	//uint hash = 0;		//use hash in play function
	//std::vector<std::vector<uint>> hashTable = std::vector<std::vector<uint>>(2);	//hashTable[O] for candidates et [1] for voters


	public :

	StatePreferenceVariation(int _nbCandidates, int _nbVoters)
	{
		nbCandidates = _nbCandidates; 
		nbVoters = _nbVoters;
		std::vector<int> prefVoter0;
		for (int i = 0; i < nbCandidates; ++i) prefVoter0.push_back(i);

		//votersPreferences[0] = prefVoter0;
	}

	void allPreference()
	{
		std::vector<int> prefVoter;
		for (int i = 0; i < nbCandidates; ++i) prefVoter.push_back(i);

		do{
			displayVec(prefVoter);
		}while(next_permutation(prefVoter.begin(), prefVoter.end()));		
	}


	template<typename It>  
	bool next_permutation(It begin, It end)  
	{  
	        if (begin == end)  
	                return false;  
	  
	        It i = begin;  
	        ++i;  
	        if (i == end)  
	                return false;  
	  
	        i = end;  
	        --i;  
	  
	        while (true)  
	        {  
	                It j = i;  
	                --i;  
	  
	                if (*i < *j)  
	                {  
	                        It k = end;  
	  
	                        while (!(*i < *--k))  
	                                /* pass */;  
	  
	                        iter_swap(i, k);  
	                        reverse(j, end);  
	                        return true;  
	                }  
	  
	                if (i == begin)  
	                {  
	                        reverse(begin, end);  
	                        return false;  
	                }  
	        }  
	}  
	/*void generateSequence(int nbInterleave, std::vector<int> sequence)
	{
		//std::vector<int> sequence = {0};
		for (int i = 0; i < (nbCandidates-1 - nbInterleave) ; ++i)
		{

		}
	}*/
};

#endif
