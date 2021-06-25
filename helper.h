#ifndef _h_helper
#define _h_helper

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <set>
#include <chrono>

#include "cell.h"
#include "net.h"
#include "leaf.h"
#include "branchbound.h"

template <typename arrayType>
void printArray(arrayType& arr) {
	std::cout << '|';
	for (auto& it : arr) {
		std::cout << it << ' ';
	}
	std::cout << '|' << std::endl;
}

void printLeaf(leaf* ThisLeaf);
//initialise all input variables
void init(int& CellPerPartition, int& TotalNetNo);

//print out the solution in terms of cell and net no in a seg
void printSol(solution* sol);
// print info about one seg
void printSeg(std::vector<cell*>& cells);

//read circuit file definition and parsing input file
int FileRead(std::string circuitName);
//print nets and fanout in ascending order
void printNet();
//print run time
void FinaliseTime(char circuitChoice, 
	std::chrono::high_resolution_clock::time_point start);
//initialise Windows circuit names and traversal method
void WinInit(char circuitChoice, char method, std::string& circuitName);

//draw one level of decision tree
void drawtree();
#endif // !_h_helper
