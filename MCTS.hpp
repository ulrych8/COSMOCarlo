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


struct tupleResult{
	int winner;
	std::vector<int> elSeq;
};

//template<class Statistique, class RandomVariable, class Measurement, class RNG>
//void MonteCarlo(Statistique & res, )
template <class StateContext> //CTX for CONTEXT
class MCTS 
{

	struct tupleMinMaxState{
		int min;
		int max;
		StateContext state;
	};

	private :

	std::map<int,std::vector<std::vector<int>>> infoTable;
	std::vector<std::vector<uint>> historyTable; //
	std::vector<int> killerTable;	 //
	int nbCandidates;
	int nbVoters;
	std::vector<std::vector<int>> votersPreferences;
	std::string heuristic;
	//int cpteurUtile = 0;
	std::vector<int> eliminationSeq;	//sequence of each candidate eliminated

	public :
	int cpteur = 0;

	MCTS(int _nbCandidates, int _nbVoters, std::string _heuristic="",const std::vector<std::vector<int>>&  _votersPreferences=std::vector<std::vector<int>>())
	{
		heuristic = _heuristic;
		nbCandidates = _nbCandidates;
		nbVoters = _nbVoters;
		votersPreferences = _votersPreferences;
		eliminationSeq = std::vector<int>(nbCandidates,0);
		if (heuristic.find("history") != std::string::npos)
		{
			for (int i = 0; i < nbVoters; ++i)
			{
				historyTable.push_back( std::vector<uint>(nbCandidates,0) );
			}
		}else if (heuristic.find("killer") != std::string::npos)
		{
			//first dimension is depth then candidates
			for (int i = 0; i < nbCandidates; ++i)
			{
				killerTable = std::vector<int>(nbCandidates,0);
			}
		}
	}

	void seeFinal()
	{
		for (int i = 0; i < nbCandidates-1; ++i)
		{
			std::cout << eliminationSeq[i] << " ";
		}
		std::cout << std::endl;
	}

	void seeKillerTable()
	{
		for (int i = 0; i < nbCandidates; ++i)
		{
			std::cout << killerTable[i] << " ";
		}
		std::cout << std::endl;
	}

	//Take a state and the optimumScore for one player (nbCandidate-1)
	//Return the winner
	/*tupleResult MAXN(StateContext s, int depth = 0)
	{
		//std::cout << "|" << std::endl;
		if (s.getCandidatesLeft().size() == 2)
		{
			int candidateEliminated = s.eraseLeastPreferedSincere();
			std::vector<int> v = {candidateEliminated};
			tupleResult res = {s.getCandidatesLeft()[0], v};
			return res;
		}

		int currentVoter = s.getCurrentVoter();

		//improve order of children if it may
		int maxScoreAction = -1;
		//int maxWinner=-1;
		tupleResult maxTuple;
		//int indexMaxScoreAction;

		int lastEliminatedCandidate;
		int bestEliminatedCandidate;
		int maxScore = s.scoreMaxForCurrentVoter();

		for (int i = 0; i < s.getNbChild(); ++i)
		{
			StateContext newState = s.getChild(i);
			int currentScore;
			tupleResult tupleRes;

			if (heuristic=="simpleOrder" || heuristic=="killer+simpleOrder")	
			    //->order list of children == History Heuristic
				newState.orderCandidatesLeft(); 
			if (heuristic=="history")	
			    //->order list of children == History Heuristic
				newState.orderCandidatesLeftWithHistory(historyTable); 

			if ((heuristic=="killer" || heuristic=="killer+simpleOrder") && newState.getCandidatesLeft()[0]!=killerTable[depth]){
			    //->order list of children == Killer Heuristic
				newState.orderCandidatesLeftWithKiller(killerTable[depth], cpteur, cpteurUtile); 
			}
			//if (heuristic=="cache")
				//newState.orderCandidatesLeftWithCache( cacheTable ); 
			
			lastEliminatedCandidate = newState.getLastEliminatedCandidate();
			

			tupleRes = MAXN(newState, depth+1);
			currentScore = s.score(tupleRes.winner);
			if (currentScore > maxScoreAction)
			{
				maxScoreAction = currentScore;
				//maxWinner = tupleRes.winner;
				bestEliminatedCandidate = lastEliminatedCandidate;
				maxTuple = tupleRes;

				if (currentScore ==  maxScore)
				{
					if (heuristic=="history")	
						historyTable[currentVoter][lastEliminatedCandidate] += 1 << depth;
					if (heuristic=="killer" || heuristic=="killer+simpleOrder")	
						killerTable[depth] = lastEliminatedCandidate;
					
					//if(depth==0) std::cout << "depth 0 -> " << lastEliminatedCandidate << std::endl;
					//if(depth==1) std::cout << "depth 1 -> " << lastEliminatedCandidate << std::endl;
					std::vector<int>::iterator it = maxTuple.elSeq.begin();
					maxTuple.elSeq.insert(it, bestEliminatedCandidate );
					return maxTuple;
				}
			}
		}

		if (heuristic=="history")	
			historyTable[currentVoter][bestEliminatedCandidate] += 1 << depth;
		if (heuristic=="killer" || heuristic=="killer+simpleOrder")	
			killerTable[depth] = bestEliminatedCandidate;
		
		//if(depth==0) std::cout << "depth 0 -> " << lastEliminatedCandidate << std::endl;
		//if(depth==1) std::cout << "depth 1 -> " << lastEliminatedCandidate << std::endl;
					
		std::vector<int>::iterator it = maxTuple.elSeq.begin();
		maxTuple.elSeq.insert(it, bestEliminatedCandidate );
		return maxTuple;
	}*/

	tupleResult MAXN(StateContext s, std::vector<int> pruningValues, int parentVoter=-1, int bestWinnerParent=-1, int depth=0)
	{
		cpteur++;
		if (s.getCandidatesLeft().size() == 2)
		{
			int candidateEliminated = s.eraseLeastPreferedSincere();
			std::vector<int> v = {candidateEliminated};
			tupleResult res = {s.getCandidatesLeft()[0], v};
			std::cout << "-------------------------------> " << s.getCurrentVoter() << " eliminate candidate " << candidateEliminated << " - - - > winner = " << res.winner << std::endl;

			return res;
		}

		int currentVoter = s.getCurrentVoter();

		std::vector<tupleMinMaxState> leftOver;
		
		//improve order of children if it may
		int maxScoreAction = -1;
		//int maxWinner = -1;
		tupleResult maxTuple;
		maxTuple.winner = -1;
		int lastEliminatedCandidate;
		int bestEliminatedCandidate=-1;
		int maxScore = s.scoreMaxForCurrentVoter();

		//================== order child heuristic
		if (heuristic.find("simple order") != std::string::npos ){
		    //->order list of children == History Heuristic
			s.orderCandidatesLeft(); 
			/*std::cout << "before order : ";
			cL = s.getCandidatesLeft();
			s.displayVec(cL);*/
		}	
		if (heuristic.find("history") != std::string::npos)	
		    //->order list of children == History Heuristic
			s.orderCandidatesLeftWithHistory(historyTable); 
		if ( (heuristic.find("killer") != std::string::npos) && s.getCandidatesLeft()[0]!=killerTable[depth]){
		    //->order list of children == Killer Heuristic
			s.orderCandidatesLeftWithKiller(killerTable[depth]); //, cpteur, cpteurUtile); 
		}
		//if (heuristic=="cache")
			//newState.orderCandidatesLeftWithCache( cacheTable );
		//================== =======================

		for (int i = 0; i < s.getNbChild(); ++i)
		{

			//if (depth==1) s.displayVec(pruningValues);
			StateContext newState = s.getChild(i);
			
			
			if (depth==0) std::cout << "-----> ";
			if (depth==1) std::cout << "------------------> ";
			std::cout << currentVoter << " eliminate candidate " << newState.getLastEliminatedCandidate() << std::endl; 
			std::cout << " .....  parentVoter = "<<parentVoter<< " | bestWinnerParent = "<<bestWinnerParent<<std::endl;
			int currentScore=-1;
			tupleResult tupleRes;

			//================== Pruning
			if (heuristic.find("pruning") != std::string::npos){

				if (newState.scoreMaxVoterSeqIndex(depth) <= pruningValues[depth]) goto jump;
				
				if (heuristic.find("complex pruning") != std::string::npos )
				{
					for (int j = depth-1; j >= 0; --j)
					{
						if (newState.scoreMaxVoterSeqIndex(j) <= pruningValues[j])
						{
							//give the minimum score
							//tupleRes.winner = newState.eraseLeastPreferedSincere(currentVoter);		
							int min = newState.scoreMinForVoter(currentVoter);		
							int max = newState.scoreMaxForVoter(currentVoter);		
							tupleMinMaxState checkLast = {min, max, newState};
							typename std::vector<tupleMinMaxState>::iterator it = leftOver.begin();
							if ( heuristic.find("ordered complex pruning") != std::string::npos ){
								for (size_t k=0; k < leftOver.size(); k++ )
								{
									if ((*it).max < checkLast.max) break;
									++it;
								}
							}
							leftOver.insert(it, checkLast);
							//std::cout << "cut" << std::endl;
							goto jump ;
						}
					}
				}
			}
			//================== ======================

			lastEliminatedCandidate = newState.getLastEliminatedCandidate();

			tupleRes = MAXN(newState,pruningValues, currentVoter,maxTuple.winner,depth+1);
			
			//jump :

			currentScore = s.score(tupleRes.winner);
			//if (depth==0) std::cout << "i="<< i <<" after jump where tupleRes.winner = " << tupleRes.winner << " with score = " << currentScore << " for currentVoter " << currentVoter << std::endl;
			//std::cout << "and maxScoreAction = " << maxScoreAction <<  std::endl;
			
			if (currentScore > maxScoreAction)
			{
				maxScoreAction = currentScore;
				//maxWinner = tupleRes.winner;
				bestEliminatedCandidate = lastEliminatedCandidate;
				maxTuple = tupleRes;
				//std::cout << "current voter " <<currentVoter  << " (winner:"<< maxTuple.winner <<") currentScore = " << currentScore << " , pruningValues["<<depth<<"] = " << pruningValues[depth] << std::endl;
				if (currentScore > pruningValues[depth]){
					pruningValues[depth] = currentScore;
				}
				if (heuristic.find("no cut") == std::string::npos && currentScore ==  maxScore)
				{
					goto end;
				}
				if ( (heuristic.find("cut") != std::string::npos ) && cut(s,s.getCandidatesLeft(), parentVoter, bestWinnerParent, currentVoter, maxTuple.winner))
				{
					std::vector<int> cL = s.getCandidatesLeft();
					s.displayVec(cL);
					goto end;
				}
			}

			jump : continue;
		}
		
		//going through left over and repeat search if necessary
		for (size_t i = 0; i < leftOver.size(); ++i)
		{
			//std::cout << "leftOver size = " << leftOver.size() << std::endl;
			//std::cout << currentVoter << " current max for " << i << " is " << maxScoreAction << " max to evalutate : " << leftOver[i].max << std::endl;
			if (maxScoreAction<leftOver[i].max){
				tupleResult tupleRes = MAXN(leftOver[i].state,pruningValues, currentVoter, bestEliminatedCandidate, depth+1);
				int currentScore = s.score(tupleRes.winner);
				//std::cout << currentVoter << " score found for " << i << "is  = " << currentScore << std::endl;

				if (currentScore > maxScoreAction)
				{
					maxScoreAction = currentScore;
					//maxWinner = tupleRes.winner;
					bestEliminatedCandidate = leftOver[i].state.getLastEliminatedCandidate();
					maxTuple = tupleRes;
					//std::cout << "current voter " <<currentVoter  << " (winner:"<< maxTuple.winner <<") currentScore = " << currentScore << " , pruningValues["<<depth<<"] = " << pruningValues[depth] << std::endl;
					if (currentScore > pruningValues[depth]){
						pruningValues[depth] = currentScore;
					}
					if (currentScore ==  maxScore)
					{
						goto end;
					}

				}
			}else if (heuristic=="ordered complex pruning"){
				break;
			}
		}
		
		end :

		//if (depth==1) std::cout << "------------------^ "<< currentVoter << " choose to eliminate " << bestEliminatedCandidate << std::endl;

		if (heuristic.find("history") != std::string::npos )	
			historyTable[currentVoter][lastEliminatedCandidate] += 1 << depth;
		if (heuristic.find("killer") != std::string::npos )	
			killerTable[depth] = lastEliminatedCandidate;

		std::vector<int>::iterator it = maxTuple.elSeq.begin();
		maxTuple.elSeq.insert(it, bestEliminatedCandidate );
		//std::cout << currentVoter << " < back up result < winner : "<< maxTuple.winner << std::endl;
		return maxTuple;
	}


	bool cut(StateContext s ,std::vector<int> candidates, int voter1, int c1, int voter2, int c2){
		if (c1==-1) return false;
		if (voter1==-1) return false;
		
		int valueC2;
		s.findElement(votersPreferences[voter1], c2, valueC2);
		valueC2 = nbCandidates-valueC2;
		int valueC1;
		s.findElement(votersPreferences[voter1], c1, valueC1);
		valueC1 = nbCandidates-valueC1;
		if (valueC2 > valueC1 ) return false;
		//if (votersPreferences[voter1][c2] > votersPreferences[voter1][c1]) return false;
		
		s.findElement(votersPreferences[voter2], c2, valueC2);
		valueC2 = nbCandidates-valueC2;
		//s.findElement(votersPreferences[voter1], c1, valueC1);
		//valueC1 = nbCandidates-valueC1;
		for (size_t i=0; i<candidates.size(); ++i)
		{
			int valueCi;
			s.findElement(votersPreferences[voter2], candidates[i], valueCi);
			valueCi = nbCandidates-valueCi;
			if (valueCi>= valueC2 ){
				//std::cout << "prefs["<<voter2<<"]["<<candidates[i]<<"] >= votersPreferences[voter2][c2]"  << std::endl;
				int valueCiBis;
				s.findElement(votersPreferences[voter1], candidates[i], valueCiBis);
				valueCiBis = nbCandidates-valueCiBis;
				if (valueCiBis >= valueC1 ){
					return false;
				}
				std::cout << "prefs["<<voter1<<"]["<<candidates[i]<<"] = "<<votersPreferences[voter1][candidates[i]]<<" not > votersPreferences["<<voter1<<"]["<<c1<<"]"<<std::endl;
			}
		}
		std::cout << "cut for v1 = "<< voter1 <<" c1 = "<< c1 <<" v2 = " << voter2 <<  " c2 = " << c2 << " -- and candidates left = ";// std::endl;  
		return true;
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