#ifndef _H_leaf
#define _H_leaf

#include <iostream>
#include <vector>

#include "cell.h"
#include "net.h"

struct solution {
	std::vector<cell*> segs[4];
};
struct point {
	float x = 0.f;
	float y = 0.f;
};

//contain cells objects of each segment
//only store address to save up memory (i dont know)
class leaf {
private:
	//solution of this leaf
	solution sol;
	// <level,no>
	std::pair<int, int> leaflabel;
	//parent <level,leafno>
	std::pair<int, int> parentno;
	//b - balance prune, l - lb prune, others not pruned
	char prunedecision;
	//number of non-critical nets
	int basecost;
	//number of cut critical nets + basecost
	int cutsize;
	//lower bound result
	int lbno;
	int leafSeq; //1.2.3.4 for drawing
public:
	//constructor and destructor
	leaf();
	leaf(int lno,int parentn,int level, int base, int leafSequence);
	~leaf() {};
	//functional 
	void prune(char decision);
	void parentSol(solution* parent);
	//push new cell into the solution space and update cutsize
	void CopyParent(std::map<int, std::map<int, leaf>>& tree);
	void PushNewCell(cell* newcell, int seg);
	//check if a seg is available
	bool checkSeg(int seg, int cellnoToPush, int sizeperseg);
	//calcuate cut
	void UpdateCellProperty();
	void NewUpdateCellProperty();
	void leafChecker(int SizePerSeg, int bestcut, int maxlevel);
	//helper
	solution* getsol();
	char getDecision();
	int getCutSize();
	int getLB();
	int getLevel();
	int getLeafNo();
	int getParentNo();
	int getLeafSeq();

	point coordinate;
};

void ProcessLeaf(leaf* ThisLeaf, cell* celltopush, int segNo, int SizePerSeg, int bestcut, std::map<int, std::map<int, leaf>>& tree, int maxlevel);

void LeafVisitedTimes();

#endif // !_H_leaf

