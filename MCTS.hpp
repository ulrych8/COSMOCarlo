#ifndef DEF_MCTS
#define DEF_MCTS


//#include <utility> //pair
#include <cmath>
#include <vector>
#include <iostream>
//#include <random>
//#include <algorithm> //min
//#include <ctime>
#include <map>

#include "State.hpp"


//template<class Statistique, class RandomVariable, class Measurement, class RNG>
//void MonteCarlo(Statistique & res, )
template <class StateContext> //CTX for CONTEXT
class MCTS 
{

	private :

	std::map<int,std::vector<std::vector<int>>> infoTable;
	std::vector<std::vector<uint>> historyTable;
	bool chilOrdered;

	public :
	MCTS(int nbCandidates, int nbVoters, bool _chilOrdered)
	{
		//chilOrdered = false;//_chilOrdered;
		for (int i = 0; i < nbVoters; ++i)
		{
			historyTable.push_back( std::vector<uint>(nbCandidates,0) );
		}
	}

	//Take a state and the optimumScore for one player (nbCandidate-1)
	//Return the winner
	int MAXN(StateContext s, int depth = 0)
	{
		//std::cout << "|" << std::endl;
		if (s.getCandidatesLeft().size() == 2)
		{
			s.eraseLeastPreferedSincere();
			return s.getCandidatesLeft()[0];
		}

		int currentVoter = s.getCurrentVoter();
		
		//improve order of children if it may
		int maxScoreAction = 0;
		int maxWinner;
		//int indexMaxScoreAction;

		int lastEliminatedCandidate;
		int maxScore = s.scoreMaxForCurrentVoter();
		for (int i = 0; i < s.getNbChild(); ++i)
		{
			StateContext newState = s.getChild(i);
			
			if (chilOrdered)	
			//{
				//->order list of children == History Heuristic
				s.orderCandidatesLeftWithHistory(historyTable); 

			//}

			//->get last eliminated candidate
			lastEliminatedCandidate = s.getLastEliminatedCandidate();


			int winner = MAXN(newState, depth+=1);
			int currentScore = s.score(winner);
			if (currentScore ==  maxScore)
			{
				historyTable[currentVoter][lastEliminatedCandidate] += 1 << depth;
				return winner;
			}
			if (currentScore > maxScoreAction)
			{
				maxScoreAction = currentScore;
				maxWinner = winner;
				//indexMaxScoreAction = i;
			}
		}
		historyTable[currentVoter][lastEliminatedCandidate] += 1 << depth;
		return maxWinner;
	}

	int UCT(StateContext s, bool multiplayer = false){

		if (s.terminal())
		{
			return s.result();
		}
		int currentHash = s.getHash();
		if (infoTable.find(currentHash) != infoTable.end())			//if getHash() is in infoTable
		{
			double bestValue = -1000000.0;
			int best = 0;
			for (int i = 0; i < s.getNbChild(); ++i)
			{
				double val = 1000000.0;
				std::vector<std::vector<int>> &t = infoTable[currentHash];
				if (t[1][i]>0)
				{
					double Q = t[2][i] / (double) t[1][i];
					val = Q + 1.4*sqrt(log(t[0][0])/t[1][i]); //cst = 0.4
				}
				if (val>bestValue)
				{
					bestValue = val;
					best = i;
				}
			}

			StateContext sChild = s.getChild( best );
			int res = UCT(sChild, multiplayer);
			//std::cout << " @ @ @ -> "; 
			infoTable[currentHash][0][0] += 1;
			infoTable[currentHash][1][best] += 1;
			if (multiplayer)
			{
				//evaluation of a candidate by the current voter
				infoTable[currentHash][2][best] += s.score(res);

			}else
			{
				infoTable[currentHash][2][best] += res;
			}
			return res;
		}else
		{
			std::vector<int> playoutCurrentState = {1};		//state added so seen once
			std::vector<int> nPlayout(s.getNbChild(),0);
			std::vector<int> nScore(s.getNbChild(),0);
			std::vector<std::vector<int>> v = {playoutCurrentState, nPlayout, nScore};

			infoTable.insert({s.getHash(), v});
			return s.playout();
		}
	}

	int BestMoveUCT(StateContext &s, int const &n, bool multiplayer = false) {
		for (int i = 0; i < n; ++i)
		{
			StateContext sTemp(s);
			UCT(sTemp, multiplayer);
		}
		
		int best = 0;
		if ( infoTable.find(s.getHash()) == infoTable.end())
		{
			throw "shouldn't happen";
		}
		double bestValue = infoTable[s.getHash()][2][0];
		for (int i = 1; i < s.getNbChild(); ++i)
		{
			if (infoTable[s.getHash()][2][i] > bestValue)
			{
				//std::cout << "s.getHash() = " << s.getHash() << " and i = " << i << std::endl; 
				bestValue = infoTable[s.getHash()][2][i];
				best = i;
			}
		}
		return best;
	}

	
};

#endif