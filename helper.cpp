#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <string>
#include <queue>
#include <algorithm>
#include <thread>
#include <chrono>
#include <cmath>
#include <stdio.h>
#include <iterator>
#include "graphics.h"
#include "easygl_constants.h"
#include "cell.h"
#include "net.h"
#include "branchbound.h"
#include "helper.h"


using namespace std;
using namespace std::chrono;

//global vectors
extern vector<int> circuitdef;
extern map <int, vector<int>> circuitConnections;
extern vector<cell> cells;//cell num and cell object
extern vector<net> nets; //net num and net object
extern vector<int> CriticalNets;
extern set<int> uniqueNetNo;
extern int noncriticalNo;
extern int CellWithCNet;
extern map<int, map<int, leaf>> decisionTree;

void FinaliseTime(char circuitChoice, high_resolution_clock::time_point start) {
	//get program run time
	auto stop = high_resolution_clock::now();

	switch (circuitChoice) {
	case '1': {
		auto duration = duration_cast<milliseconds>(stop - start);
		cout << "Run time is " << duration.count() << "ms" << endl;
		break; }
	case '2': {
		auto duration = duration_cast<milliseconds>(stop - start);
		cout << "Run time is " << duration.count() << "ms" << endl;
		break; }
	case '3': {
		auto duration = duration_cast<milliseconds>(stop - start);
		cout << "Run time is " << duration.count() << "ms" << endl;
		break; }
	case '4': {
		auto duration = duration_cast<milliseconds>(stop - start);
		cout << "Run time is " << duration.count() << "ms" << endl;
		break;
	}
	default: {
		cout << "WRONG CIRCUIT FILE\n";
		exit(1);
	}
	}
}

void printNet() {
	for (auto& it : nets) {
		cout << "Net no." << it.getNetNum() << " is connected to " << it.getfanout() << " cells" << endl;
		cout << "===================================" << endl;
	}
}


void printSeg(vector<cell*>& cells) {
	for (auto it : cells)
		cout << it->getCellNum() << ' ';
	cout << endl;
}

void printSol(solution* sol) {
	set<int> segnet[4];

	auto GetNetsInSeg = [](vector<cell*>& cellsConnected,
		set<int>& netsInSeg) {
			for (auto& it : cellsConnected) {
				vector<int> tempNets;
				it->getNets(tempNets);
				for (auto& iit : tempNets) {
					netsInSeg.insert(iit);
				}
			}
	};

	for (int i = 0; i < 4; i++) {
		GetNetsInSeg(sol->segs[i], segnet[i]);
		cout << "Seg " << i << " has cells:";
		printSeg(sol->segs[i]);
		cout << "Seg " << i << " has nets:";
		printArray(segnet[i]);
	}
}

void printLeaf(leaf* ThisLeaf) {
	cout << "Leaf of level:" << ThisLeaf->getLevel();
	cout << " Leaf no:" << ThisLeaf->getLeafNo() << " prune decision is " << ThisLeaf->getDecision();
	cout << " Cut size is:" << ThisLeaf->getCutSize() << endl;
	printSol(ThisLeaf->getsol());
}

int FileRead(string circuitName) {
	//start reading circuit files

	ifstream InFile;
	InFile.open(circuitName, ios::in);

	//read input file
	if (InFile.is_open()) {
		int temp;
		cout << "Start reading file" << endl;
		while (InFile) {
			InFile >> temp;
			circuitdef.push_back(temp);
		}
		InFile.close();
	}
	else {
		cerr << "File not opened\n";
		return 1;
	}

	// parsing circuit file
	for (vector<int>::iterator it = circuitdef.begin(); it != circuitdef.end(); it++) {
		int BlockKey = *it;
		it++;
		while (*it != -1) {
			circuitConnections[BlockKey].push_back(*it);
			uniqueNetNo.insert(*it);
			it++;
		}
		if (*it == -1 && *(it + 1) == -1)
			break;
	}

	return 0;
}

void init(int& CellPerPartition, int& TotalNetNo) {
	//find all unique net no and initiate net objects
	TotalNetNo = uniqueNetNo.size();
	cout << "Total number of nets: " << TotalNetNo << endl;
	vector<int> AllNetNo;
	for (auto& it : uniqueNetNo) {
		net temp(it);
		nets.push_back(temp);
		AllNetNo.push_back(it);
	}


	// construct cell objects
	for (auto& it : circuitConnections) {
		cells.push_back(cell(it.first));
		vector<cell>::reverse_iterator thisCell = cells.rbegin();
		for (auto& iit : it.second)
			thisCell->setNets(iit);
	}

	for (auto& cellit : cells) {
		vector<int> ConnectedNets;
		cellit.getNets(ConnectedNets);
		for (auto& netit : ConnectedNets) {
			nets[netit - 1].addCell(cellit.getCellNum());
		}
	}

	//calculate net fanout
	for (auto& it : nets) {
		it.calFanout();
	}
	//sort nets with ascending fanout
	sort(nets.begin(), nets.end(), 
		[](net n1, net n2) {
			return (n1.getfanout() < n2.getfanout());
		});
	// calculate how many cells in a partition
	CellPerPartition = cells.size() / 4;
	cout << "Cells per partition is " << CellPerPartition << endl;
	noncriticalNo = 0;
	cout << "===================================" << endl;
	vector<int> nonCriticalNets;
	for (auto& it : nets) {
		it.printinfo();
		if (it.getfanout() > CellPerPartition) {
			noncriticalNo++;
			nonCriticalNets.push_back(it.getNetNum());
		}
	}
	//find critical net no
	sort(AllNetNo.begin(), AllNetNo.end());
	sort(nonCriticalNets.begin(), nonCriticalNets.end());
	set_difference(AllNetNo.begin(), AllNetNo.end(), nonCriticalNets.begin(), nonCriticalNets.end(),
		inserter(CriticalNets,CriticalNets.begin()));
	for (auto& it : cells)
		it.CalCellProperty();

	sort(cells.begin(), cells.end(), [](cell c1, cell c2) {
		return (c1.getCritialNetCount() > c2.getCritialNetCount());
		});

	
	for (auto& it : cells) {
		//debug
		it.PrintInfo();
		if (it.getCritialNetCount() != 0)
			CellWithCNet++;
	}
		

	cout << "Total number of non-critical nets " << noncriticalNo << endl;
	cout << "Number of cell with critical nets: " << CellWithCNet << endl;
	cout << "Critical net no: ";
	for (auto& it : CriticalNets) {
		cout << it << ' ';
	}
	cout << endl;
}


void WinInit(char circuitChoice, char method, string& circuitName) {
	switch (circuitChoice) {
	case '1': {
		circuitName = "cct1";
		break; }
	case '2': {
		circuitName = "cct2";
		break; }
	case '3': {
		circuitName = "cct3";
		break; }
	case '4': {
		circuitName = "cct4";
		break;
	}
	default: {
		cout << "WRONG CIRCUIT FILE\n";
		exit(1);
	}
	}
	//input checking
	if (method != 'a' && method != 'b') {
		cout << "Ilegal choice\n";
		exit(1);
	}
}

void drawtree() {

	decisionTree[1][1].coordinate.x = 500.f;
	decisionTree[1][1].coordinate.y = -decisionTree[1][1].getLevel() * 100.f;
	fillarc(decisionTree[1][1].coordinate.x, decisionTree[1][1].coordinate.y, 5.f, 0.0f, 361.f);
	setcolor(BLACK);
	string text = "Level " + to_string(1);
	drawtext(-60.f, decisionTree[1][1].coordinate.y, text.c_str(), 100.f);
	float x = 0.0;
	float y = 0.0;
	float segwidth;
	for (int i = 2; i <= cells.size(); i++) {
		y = -100.f * i;
		segwidth = 1000.f / (pow(4, i-1));
		setcolor(BLACK);
		string text = "Level " + to_string(i);
		drawtext(-60.f, y, text.c_str(), 100.f);
		for (int jj = 1; jj <= decisionTree[i].size(); jj++) {
			leaf* thisleaf = &decisionTree[i][jj];
			//printLeaf(thisleaf);
			if (thisleaf->getLeafNo() != -1) {
				thisleaf->coordinate.y = y;
				float parentx = decisionTree[i - 1][thisleaf->getParentNo()].coordinate.x;
				switch (thisleaf->getLeafSeq()) {
				case 0: {
					thisleaf->coordinate.x = parentx - 1.5 * segwidth;
					break;
				}
				case 1: {
					thisleaf->coordinate.x = parentx - 0.5 * segwidth;
					break;
				}
				case 2: {
					thisleaf->coordinate.x = parentx + 0.5 * segwidth;
					break;
				}
				case 3: {
					thisleaf->coordinate.x = parentx + 1.5 * segwidth;
					break;
				}
				default: {
					cerr << "Leaf config problem in drawing\n";
					exit(1);
				}
				}
				if (thisleaf->getDecision() == 'b')
					setcolor(RED);
				else if (thisleaf->getDecision() == 'l')
					setcolor(BLUE);
				else
					setcolor(BLACK);
				fillarc(thisleaf->coordinate.x, thisleaf->coordinate.y, 5.f, 0.0f, 361.f);

				if (i > 2)
					drawline(thisleaf->coordinate.x, thisleaf->coordinate.y, parentx, thisleaf->coordinate.y + 100.0f);
				else if (i == 2)
					drawline(thisleaf->coordinate.x, thisleaf->coordinate.y, 500.f, thisleaf->coordinate.y + 100.0f);
			}
		}
	}
	flushinput();

}