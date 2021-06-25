#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <stdio.h>
#include <string>
#include <queue>
#include <algorithm>
#include <thread>
#include <functional>
#include <chrono>

#include "graphics.h"
#include "easygl_constants.h"
#include "branchbound.h"
#include "cell.h"
#include "net.h"
#include "leaf.h"
#include "helper.h"

using namespace std;
using namespace std::chrono;

//global vectors
extern vector<int> circuitdef;
extern map <int, vector<int>> circuitConnections;
extern vector<cell> cells;//cell num and cell object
extern vector<net> nets; //net num and net object
extern set<int> uniqueNetNo;
extern map<int, map<int, leaf>> decisionTree;
extern int noncriticalNo;
extern char circuitChoice;
extern int CellWithCNet;
extern vector<int> removeList;
extern vector<int> CriticalNets;


vector<cell>::iterator FindCell(int CellNo) {

	for (vector<cell>::iterator it = cells.begin(); it != cells.end(); it++) {
		if (it->getCellNum() == CellNo)
			return it;
	}

}

bool AllCellCheck(vector<int>& celllist) {
	for (auto& it : celllist) {
		auto cellit = FindCell(it);
		if (cellit->CheckLockStatus() == true)
			return false;
	}
	return true;
}

bool PushCheck(leaf& initsoln, int segment, int sizeperseg, int netfanout,
	vector<int>& celllist) {
	if (initsoln.checkSeg(segment, netfanout, sizeperseg) == true && AllCellCheck(celllist))
		return true;

	else
		return false;
}

leaf InitSoln(int SizePerSeg) {
	leaf initsoln(-5, -5, -5, noncriticalNo,0);
	int maxMiminisation = 0;
	cout << "Initial solution============" << endl;
	for (auto& it : nets) {
		cout << "Check net: " << it.getNetNum() << endl;
		if (it.getfanout() <= SizePerSeg) {
			maxMiminisation++;
			for (int i = 0; i < 4; i++) {
				vector<int> CellList;
				it.getCells(CellList);
				printArray(CellList);
				if (PushCheck(initsoln,i,SizePerSeg,it.getfanout(), CellList)) {
					//if a segment is available to accommodate all cells connected to the net, push all cells to this seg
					for (auto& celltopush : CellList) {
						int Order = 0;
						cell* tempcell = & cells[Order];
						for (auto& it : cells) {
							if (it.getCellNum() == celltopush) {
								tempcell = &cells[Order];
								break;
							}
							else
								Order++;
						}

						if (tempcell->CheckLockStatus() == false) {
							tempcell->lockcell();
							initsoln.PushNewCell(tempcell, i);
						}
					}
					break;
				}
			}
			//only deal with one first
			//break;
		}

	}
	int seg = 0;
	for (auto& it : cells) {
		if (it.CheckLockStatus() == false) {
			if (initsoln.checkSeg(seg, 1, SizePerSeg) == true) {
				initsoln.PushNewCell(&it, seg);
				it.lockcell();
			}
			else {
				seg++;
				if (initsoln.checkSeg(seg, 1, SizePerSeg) == true) {
					initsoln.PushNewCell(&it, seg);
					it.lockcell();
				}
			}
		}
	}
	initsoln.UpdateCellProperty();
	printSol(initsoln.getsol());
	cout << "Initial solution cut size " << initsoln.getCutSize() << endl;
	cout << "Initial solution============" << endl;
	cout << "Maximal cut size reduction " << maxMiminisation << endl;
	return initsoln;
}

//main branch and bound functions
void MainBBCall(char method,const int CellPerPartition,int& cutsize) {
	int maxlevel = cells.size();
	cout << "Max level " << maxlevel << endl;
	//quick and dirty initial solution
	leaf bestleaf = InitSoln(CellPerPartition);

	bestleaf.UpdateCellProperty();

	//printSol(bestleaf.getsol());

	cutsize = bestleaf.getCutSize();
	cout << "Number of nets cut is (initial solution):" << cutsize << endl;
	//decision tree gen
	if (method == 'a') {
		high_resolution_clock::time_point start = high_resolution_clock::now();
		BreathBB(CellPerPartition, &bestleaf, cutsize, maxlevel);
		FinaliseTime(circuitChoice, start);
	}
	else if (method == 'b') {
		//leaf testinitsol = InitSoln(CellPerPartition);
		high_resolution_clock::time_point start = high_resolution_clock::now();
		lbfirst(CellPerPartition, &bestleaf, cutsize, maxlevel);
		FinaliseTime(circuitChoice, start);
	}
}

void BreathBB(int sizeperseg, leaf* bestleaf, int& BestCut, int maxlevel) {
	int pruneNo = 0;
	int balance = 0;
	int levelcounter = 1;
	//maxlevel = 16;
#ifdef earlyexit
	for (int ProcessedCell = 0; ProcessedCell < CellWithCNet; ProcessedCell++) {
#else
	for (int ProcessedCell = 0; ProcessedCell < maxlevel; ProcessedCell++) {
#endif
		cell* CellInProcess = &cells[ProcessedCell];
		
		cout << "Processing level " << levelcounter << " cell no: " << CellInProcess->getCellNum() << endl;
		if (levelcounter == 1) {
			//first cell, always put to segment 1
			leaf firstSol(1,0, levelcounter, noncriticalNo,1);
			ProcessLeaf(&firstSol, CellInProcess, 0, sizeperseg, BestCut, decisionTree, maxlevel);
			printLeaf(&firstSol);
			(decisionTree[levelcounter])[1] = firstSol;
		}
		else {
			//for 2:end cells
			int leafno = 1;
			for (map<int, leaf>::iterator parentLeafit = decisionTree[levelcounter - 1].begin(); parentLeafit != decisionTree[levelcounter - 1].end(); parentLeafit++) {
				//iterate through all leaves in the parent leaf and create four solution each
				if((parentLeafit)->second.getDecision()=='u')
					//if parent is not pruned, generate children
					if (levelcounter == 2) {
						for (int i = 0; i <2; i++) {
							leaf newsol(leafno, (parentLeafit)->second.getLeafNo(), levelcounter, noncriticalNo,i);
							ProcessLeaf(&newsol, CellInProcess, i, sizeperseg, BestCut, decisionTree, maxlevel);
							printLeaf(&newsol);
							(decisionTree[levelcounter])[leafno] = newsol;
							leafno++;
						}
					}
					else if(levelcounter==3){
						for (int i = 0; i < 3; i++) {
							leaf newsol(leafno, (parentLeafit)->second.getLeafNo(), levelcounter, noncriticalNo, i);
							ProcessLeaf(&newsol, CellInProcess, i, sizeperseg, BestCut, decisionTree, maxlevel);
							printLeaf(&newsol);
							(decisionTree[levelcounter])[leafno] = newsol;
							leafno++;
						}
					}
					else {
						for (int i = 0; i < 4; i++) {
							leaf newsol(leafno, (parentLeafit)->second.getLeafNo(), levelcounter, noncriticalNo,i);
							ProcessLeaf(&newsol, CellInProcess, i, sizeperseg, BestCut, decisionTree, maxlevel);
							//printLeaf(&newsol);
							(decisionTree[levelcounter])[leafno] = newsol;
							leafno++;
						}
					}

			}

		}
		//iterate to next level
		levelcounter++;
	}

	//print best solution
#ifdef earlyexit
	map<int, leaf> LastLevel = decisionTree[CellWithCNet];
#else
	map<int,leaf> LastLevel = decisionTree[levelcounter-1];
#endif
	int MinCut = bestleaf->getCutSize();
	for (auto& it : LastLevel) {
		//printLeaf(&(it.second));
		if (it.second.getCutSize() < MinCut)
			MinCut = it.second.getCutSize();
	}
	leaf OptimalSoln;
	for (auto& it : LastLevel) {
		if (it.second.getDecision() == 'u' && it.second.getCutSize() == MinCut) {
			OptimalSoln=it.second;
			break;
		}
	}
	int segtopush = 0;

#ifdef earlyexit
	for (int ProcessedCell = CellWithCNet; ProcessedCell < maxlevel; ProcessedCell++) {
#else
	for (int ProcessedCell = maxlevel; ProcessedCell < maxlevel; ProcessedCell++) {
#endif
		if (OptimalSoln.checkSeg(segtopush, 1, sizeperseg))
			OptimalSoln.PushNewCell(&cells[ProcessedCell], segtopush);
		else {
			ProcessedCell--;
			segtopush++;
		}
	}

	cout << "Best cut^^^^^^^^^^^^^^^^^^" << endl;
	printLeaf(&OptimalSoln);

	
}

class compare {
public:
	bool operator()(leaf l1, leaf l2) {
		if (l1.getLevel() > l2.getLevel())
			return true;
		else if (l1.getLevel() < l2.getLevel())
			return false;
		else {
			if (l1.getCutSize() < l2.getCutSize())
				return true;
			else
				return false;
		}
	}
};

void lbfirst(int sizeperseg, leaf* bestleaf, int& BestCut, int maxlevel) {

	int bestleafno;
	int levelcounter = 1;
	int leafno = 1;
	//search queue
	priority_queue<leaf, vector<leaf>, compare> searchQueue;

	cell* CellInProcess = &cells[levelcounter - 1];

	//cout << "Processing level " << levelcounter << " cell no: " << CellInProcess->getCellNum() << endl;
	leaf firstSol(1, 0, 1, noncriticalNo,0);
	ProcessLeaf(&firstSol, CellInProcess, 0, sizeperseg, BestCut, decisionTree, maxlevel);

	(decisionTree[levelcounter])[leafno] = firstSol;
	searchQueue.push(firstSol);
	leafno++;

	leaf CurrentLeaf = searchQueue.top();
	searchQueue.pop();
	int parentLevel = CurrentLeaf.getLevel();
	levelcounter = parentLevel + 1;
	CellInProcess = &cells[parentLevel];
	for (int i = 0; i < 2; i++) {
		leaf secondLevelsol(leafno, CurrentLeaf.getLeafNo(), levelcounter, noncriticalNo,i);
		ProcessLeaf(&secondLevelsol, CellInProcess, i, sizeperseg, BestCut, decisionTree, maxlevel);
		(decisionTree[levelcounter])[leafno] = secondLevelsol;
		searchQueue.push(secondLevelsol);
		leafno++;
	}

	int Flag = 1;
#ifdef earlyexit
	if (CellWithCNet > 2) {
#else
	if (CellWithCNet >= 2) {
#endif

		while ((!searchQueue.empty()) && Flag) {
			CurrentLeaf = searchQueue.top();
			searchQueue.pop();

			if (CurrentLeaf.getDecision() == 'u') {
				int parentLevel = CurrentLeaf.getLevel();
				levelcounter = parentLevel + 1;
				CellInProcess = &cells[parentLevel];

				if (levelcounter >= 4) {
					for (int i = 0; i < 4; i++) {
						leaf newsol(leafno, CurrentLeaf.getLeafNo(), levelcounter, noncriticalNo, i);
						ProcessLeaf(&newsol, CellInProcess, i, sizeperseg, BestCut, decisionTree, maxlevel);
						(decisionTree[levelcounter])[leafno] = newsol;
						searchQueue.push(newsol);
						leafno++;
					}
				}
				else if (levelcounter == 3) {
					for (int i = 0; i < 3; i++) {
						leaf newsol(leafno, CurrentLeaf.getLeafNo(), levelcounter, noncriticalNo, i);
						ProcessLeaf(&newsol, CellInProcess, i, sizeperseg, BestCut, decisionTree, maxlevel);
						(decisionTree[levelcounter])[leafno] = newsol;
						searchQueue.push(newsol);
						leafno++;
					}
				}

#ifdef earlyexit
				if (levelcounter == CellWithCNet) {
#else
				if (levelcounter == maxlevel) {
#endif

					map<int, leaf> LastLevel = decisionTree[levelcounter];
					int MinCut = bestleaf->getCutSize();
					for (auto& it : LastLevel) {
						//printLeaf(&(it.second));
						if (it.second.getCutSize() < MinCut)
							MinCut = it.second.getCutSize();
					}
					leaf OptimalSoln;
					for (auto& it : LastLevel) {
						if (it.second.getDecision() == 'u' && it.second.getCutSize() == MinCut) {
							OptimalSoln = it.second;
							break;
						}
					}
					int segtopush = 0;
					for (int ProcessedCell = CellWithCNet; ProcessedCell < maxlevel; ProcessedCell++) {

						if (OptimalSoln.checkSeg(segtopush, 1, sizeperseg))
							OptimalSoln.PushNewCell(&cells[ProcessedCell], segtopush);
						else {
							ProcessedCell--;
							segtopush++;
						}
					}
					cout << "+++++Bset Cut+++++++++++++++\n";
					printLeaf(&OptimalSoln);
					cout << "Minimal cut size (lowest-bound first): " << OptimalSoln.getCutSize() << endl;
					cout << "++++++++++++++++++++++++++++++++++++++++\n";
					for (int i = 1; i <= 4; i++) {
						if ((decisionTree[levelcounter])[leafno - i].getDecision() == 'u') {
							bestleafno = leafno - i;
							Flag = 0;
							break;
						}
					}
				}


			}
		}

	}
	else {
		leaf OptimalSoln;
		int MinCut = bestleaf->getCutSize();
		for (auto& it : decisionTree[CellWithCNet]) {
			//printLeaf(&(it.second));
			if (it.second.getCutSize() < MinCut)
				MinCut = it.second.getCutSize();
		}
		for (auto& it : decisionTree[CellWithCNet]) {
			if (it.second.getDecision() == 'u' && it.second.getCutSize() == MinCut) {
				OptimalSoln = it.second;
				break;
			}
		}
		int segtopush = 0;
		for (int ProcessedCell = CellWithCNet; ProcessedCell < maxlevel; ProcessedCell++) {

			if (OptimalSoln.checkSeg(segtopush, 1, sizeperseg))
				OptimalSoln.PushNewCell(&cells[ProcessedCell], segtopush);
			else {
				ProcessedCell--;
				segtopush++;
			}
		}
		cout << "+++++Bset Cut+++++++++++++++\n";
		printLeaf(&OptimalSoln);
		cout << "Minimal cut size (lowest-bound first): " << OptimalSoln.getCutSize() << endl;
		cout << "++++++++++++++++++++++++++++++++++++++++\n";
		for (int i = 1; i <= 4; i++) {
			if ((decisionTree[levelcounter])[leafno - i].getDecision() == 'u') {
				bestleafno = leafno - i;
				Flag = 0;
				break;
			}
		}
	}
}