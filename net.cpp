#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <string>
#include <queue>
#include <set>
#include <thread>
#include "cell.h"
#include "helper.h"
#include "branchbound.h"

using namespace std;

extern vector<cell> cells;

void net::addCell(int cellNo) {
	ConnectedCells.push_back(cellNo);
}

void net::calFanout() {
	fanout = ConnectedCells.size();
}


void net::calNetProperty() {
	int firstCell = cells[ConnectedCells[0]].getPartitionNo();
	for (vector<cell>::iterator it = cells.begin(); it != cells.end(); it++) {
		int tempPartition = it->getPartitionNo();
		if (tempPartition != firstCell) {
			Cuts = 1;
			break;
		}
	}
	Cuts = 0;
}

void net::printinfo() {
	cout << "Net no." << netnum << " conected to ";
	printArray(ConnectedCells);
	if (Cuts == 0)
		cout << "This net is not cut\n";
	else if (Cuts == 1)
		cout << "THIS NET IS CUT\n";
	else {
		cout << "Fanout of this net is " << fanout << endl;
		cout << "===================================" << endl;
	}
		
}

int net::getfanout() {
	return fanout;
}

int net::getCuts() {
	return Cuts;
}

int net::getNetNum() {
	return netnum;
}

void net::getCells(vector<int>& CellsToNet) {
	CellsToNet = ConnectedCells;
}
