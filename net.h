#ifndef _H_net
#define _H_net
#include <iostream>
#include <vector>

#include "cell.h"

//net object
class net {
	int netnum;
	std::vector<int> ConnectedCells;
	int fanout;
	int Cuts; //1 for cut, 0 for not cut
public:
	~net() {};
	net(int no) {
		netnum = no;
		Cuts = -1;
		fanout = -1;
	};
	//helper functions
	int getfanout();
	int getCuts();
	int getNetNum();
	void getCells(std::vector<int>& CellsToNet);
	//functional functions
	void addCell(int cellNo);
	void calFanout();
	void calNetProperty();
	void printinfo();

};

#endif // !_H_net
