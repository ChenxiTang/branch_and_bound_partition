#ifndef _h_bb
#define _h_bb

#include <iostream>
#include <vector>
#include <map>


#include "cell.h"
#include "net.h"
#include "leaf.h"

//#define earlyexit

struct cellComp {
	bool operator()(cell* c1, cell* c2) {
		return (c1->getCritialNetCount() > c2->getCritialNetCount());
	}
};

void MainBBCall(char method, const int CellPerPartition,	int& cutsize);

//generate decision tree
void BreathBB(int sizeperseg, leaf* bestleaf, int& BestCut, int maxlevel);
//lowest bound first travers order
void lbfirst(int sizeperseg, leaf* bestleaf,int& bestcutsize, int maxlevel);
//find initial solution
leaf InitSoln(int SizePerSeg);
#endif
 