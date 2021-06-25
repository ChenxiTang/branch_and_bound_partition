#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <string>
#include <queue>
#include <set>
#include <algorithm>

#include "cell.h"
#include "net.h"
#include "leaf.h"
#include "helper.h"
#include "branchbound.h"

using namespace std;

static int balanceprune = 0;
static int lbprune = 0;
static int nodeVisited = 0;

extern vector<int> CriticalNets;
extern vector<net> nets;
extern vector<int> removeList;

leaf::leaf() {
	leaflabel.first = -1;
	leaflabel.second = -1;
	parentno.first = -1;
	parentno.second = -1;
	cutsize = -1;
	prunedecision = 'u';
	lbno = -1;
	leafSeq = 0;
	basecost = -1;
}

leaf::leaf(int lno, int parentn, int level, int base, int leafSequence) {
	leaflabel.first = level;
	leaflabel.second = lno;
	parentno.first = level - 1;
	parentno.second = parentn;
	basecost = base;
	cutsize = -1;
	prunedecision = 'u';
	lbno = -1;
	leafSeq = leafSequence;
}

int leaf::getLeafSeq() {
	return leafSeq;
}

//functional functions
void leaf::prune(char decision) {
	if (prunedecision != 'b' && prunedecision != 'l')
		prunedecision = decision;
	else
		cout << "This branch has already been pruned\n";
}

void leaf::parentSol(solution* parent) {
	sol = *parent;
}

void leaf::PushNewCell(cell* newcell, int seg) {
	sol.segs[seg].push_back(newcell);
	//cout << "Cell pushed " << newcell->getCellNum() << " To seg: " << seg << endl;
}

bool leaf::checkSeg(int seg, int cellnoToPush, int sizeperseg) {
	if (sol.segs[seg].size() + cellnoToPush <= sizeperseg)
		return true;
	else
		return false;
}

void leaf::UpdateCellProperty(){

	set<int> segnet[4];
	set<int> cutnet;
	vector<int> temp;


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
		GetNetsInSeg(sol.segs[i], segnet[i]);
	}

	for (int i = 0; i < 4; i++) {
		for (int j = i + 1; j < 4; j++) {
			set_intersection(segnet[i].begin(), segnet[i].end(),
				segnet[j].begin(), segnet[j].end(), back_inserter(temp));
		}
	}
	//remove repetitive net nos
	for (auto& it : temp)
		cutnet.insert(it);
	cutsize = cutnet.size();

}

void leaf::leafChecker(int SizePerSeg, int bestcut, int maxlevel) {
	nodeVisited++;
	if (sol.segs[0].size() > SizePerSeg ||
		sol.segs[1].size() > SizePerSeg ||
		sol.segs[2].size() > SizePerSeg ||
		sol.segs[3].size() > SizePerSeg) {
		prunedecision = 'b';
	}
	else if (cutsize > bestcut) {
		prunedecision = 'l';
	}

}

void leaf::CopyParent(map<int, map<int, leaf>>& tree) {
	sol = (*(tree[parentno.first])[parentno.second].getsol());
	
	//printSol(&sol);
}

//helper functions
solution* leaf::getsol() {
	return &sol;
}

char leaf::getDecision() {
	return prunedecision;
}

int leaf::getCutSize() {
	return cutsize;
}

int leaf::getLB() {
	return lbno;
}

int leaf::getLevel() {
	return leaflabel.first;
}

int leaf::getLeafNo() {
	return leaflabel.second;
}

int leaf::getParentNo() {
	return parentno.second;
}

void leaf::NewUpdateCellProperty(){


	set<int> segnet[4];
	set<int> cutnet;
	vector<int> temp;


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
		GetNetsInSeg(sol.segs[i], segnet[i]);
	}

	for (int i = 0; i < 4; i++) {
		for (int j = i + 1; j < 4; j++) {
			set_intersection(segnet[i].begin(), segnet[i].end(),
				segnet[j].begin(), segnet[j].end(), back_inserter(temp));
		}
	}
	//remove repetitive net nos
	for (auto& it : temp)
		cutnet.insert(it);
	cutsize = basecost;
	
	for (auto& it : CriticalNets) {

		auto pos = cutnet.find(it);
		if (pos != cutnet.end()) {
			cutsize++;
			//removeList.push_back(it);
		}	
	}
	//sort(removeList.begin(), removeList.end());


}


//callable helper functions not a class member
void leafCheck(leaf* singleLeaf, const int& SizePerSeg, int& bestcut) {
	solution* leafSol = singleLeaf->getsol();
	if (leafSol->segs[0].size() > SizePerSeg ||
		leafSol->segs[1].size() > SizePerSeg ||
		leafSol->segs[2].size() > SizePerSeg ||
		leafSol->segs[3].size() > SizePerSeg)
		singleLeaf->prune('b');
	else if (singleLeaf->getCutSize() >= bestcut) {
		singleLeaf->prune('l');
	}
}

void ProcessLeaf(leaf* ThisLeaf, cell* celltopush, int segNo, int SizePerSeg, int bestcut, map<int,map<int,leaf>>&tree, int maxlevel) {
	if(ThisLeaf->getLevel()>1)
		ThisLeaf->CopyParent(tree);
	ThisLeaf->PushNewCell(celltopush, segNo);
	//ThisLeaf->UpdateCellProperty();
	ThisLeaf->NewUpdateCellProperty();
	ThisLeaf->leafChecker(SizePerSeg,bestcut, maxlevel);
}

void LeafVisitedTimes() {
	//cout << "Balance pruned " << balanceprune << " times" << endl;
	//cout << "LB function called " << lbprune << " times" << endl;
	cout << "# of node visited " << nodeVisited << endl;
}