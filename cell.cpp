#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <string>
#include <queue>
#include <set>
#include <thread>
#include <iterator>
#include "net.h"
#include "helper.h"
#include "branchbound.h"

using namespace std;

extern vector<int> CriticalNets;

void cell::CalCellProperty() {

	for (auto& critialNetit : CriticalNets) {
		for (auto& cellnetit : nets) {
			if (cellnetit == critialNetit)
				TotalCriticalNo++;
		}
	}

}

int cell::getCritialNetCount() {
	return TotalCriticalNo;
}

void cell::lockcell() {
	if (LockStatus == false)
		LockStatus = true;
	else {
		cerr << "Double push one cell to init soln" << endl;
		exit(-1);
	}

}

bool cell::CheckLockStatus() {
	return LockStatus;
}

void cell::setNets(int netno) {
	nets.push_back(netno);
}

void cell::PrintInfo() {
	cout << "Cell no." << cellNum <<" has " << TotalCriticalNo<<" critical nets is connected to: ";
	printArray(nets);
	if (partitionNo != -1)
		cout << "This cell is partitioned to " << partitionNo << endl;
}

int cell::getCellNum() {
	return cellNum;
}

int cell::getPartitionNo() {
	return partitionNo;
}

int cell::getNetsize() {
	return nets.size();
}

char cell::getStatus() {
	return status;
}

void cell::getNets(vector<int>& ConnectedNets) {
	ConnectedNets = nets;
}
