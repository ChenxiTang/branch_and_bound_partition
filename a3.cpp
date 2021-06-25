#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <queue>
#include <utility>
#include <algorithm>
#include <thread>
#include <chrono>
#include "graphics.h"
#include "easygl_constants.h"

#include "cell.h"
#include "net.h"
#include "leaf.h"
#include "helper.h"
#include "branchbound.h"

using namespace std;
using namespace std::chrono;

// graphics 
void drawscreen(void);
void act_on_button_press(float x, float y);
static bool rubber_band_on = false;
static bool have_entered_line, have_rubber_line;
static bool line_entering_demo = false;
static float xx1, yy1, xx2, yy2;
void Blank(void) {};

string circuitName;

//global vectors
vector<int> circuitdef;
map <int, vector<int>> circuitConnections;
vector<cell> cells;//cell num and cell object
vector<net> nets; //net num and net object
set<int> uniqueNetNo;
int cutsize = 0;
//int - level number, start with 1 map<int,solution> is a leaf in the 
//decision tree
map<int, map<int,leaf>> decisionTree;
vector<int> CriticalNets;
vector<int> removeList;
int noncriticalNo;
char circuitChoice;
char method;
int CellWithCNet = 0;

#ifndef ecf
int main() {
	

	//choose circuit file
	cout << "Choose circuit file:\n";
	cout << "1:cct1\n2:cct2\n3:cct3\n4:cct4";
	cout << endl;
	circuitChoice ='1';
	//cin >> circuitChoice;

	// choose traverse method
	cout << endl;
	cout << "Chose a search method: ";
	cout << endl;
	cout << "a: breath-first\nb: lowest-bound-first\n";
	method = 'b';
	//cin >> method;

	WinInit(circuitChoice, method, circuitName);

#else
int main(int argc, char** argv) {

	if (argc != 3) {
		cout << "[Error main] Number of input argument is not correct" << endl;
		exit(-1);
}

	circuitName = argv[1];

	if (circuitName == "cct1")
		circuitChoice = '1';
	else if (circuitName == "cct2")
		circuitChoice = '2';
	else if (circuitName == "cct3")
		circuitChoice = '3';
	else if (circuitName == "cct4")
		circuitChoice = '4';
	else
	{
		cout << "Wrong circuit file input" << endl;
		exit(1);
	}

	cout << "Circuit name is " << argv[1] << ", traversal method is " << argv[2] << endl;

	if (strcmp(argv[2], "breadth") == 0)
		method = 'a';
	else if (strcmp(argv[2], "lbf") == 0)
		method = 'b';
	else {
		cout << "Method input incorrect" << endl;
		exit(1);
	}
	
#endif

	if (FileRead(circuitName))
		return 1;
	
	int CellPerPartition = 0;
	int TotalNetNo = 0;

	init(CellPerPartition, TotalNetNo);

	
	MainBBCall(method, CellPerPartition,cutsize);

	cout << "decision level:" << decisionTree.size() << endl;
	for (auto& it : decisionTree) {
		cout << "Level " << it.first << " has " << it.second.size() << " elements" << endl;
	}

	init_graphics("Assignment 3 Branch and Bound", WHITE);
	init_world(0.0, 0.0, 1000.0f, -100.f-100.0f*cells.size());
	event_loop(act_on_button_press, NULL, NULL, drawscreen);

	

	LeafVisitedTimes();

	exit(0);
}

//other support funcitons
void drawscreen(void) {
	set_draw_mode(DRAW_NORMAL);
	clearscreen();
	setfontsize(10);
	setlinestyle(DASHED);
	setlinewidth(1);
	setcolor(BLACK);
	drawline(0., 0, 0., -100.f - 100.0f * cells.size());
	drawline(250.f, 0, 250.f, -100.f - 100.0f * cells.size());
	drawline(2*250.f, 0, 2*250.f, -100.f - 100.0f * cells.size());
	drawline(3*250.f, 0, 3*250.f, -100.f - 100.0f * cells.size());
	drawline(4 * 250.f, 0, 4 * 250.f, -100.f - 100.0f * cells.size());

	drawline(250.f/4, 0, 250.f/4, -100.f - 100.0f * cells.size());
	drawline(2 * 250.f/4, 0, 2 * 250.f/4, -100.f - 100.0f * cells.size());
	drawline(3 * 250.f/4, 0, 3 * 250.f/4, -100.f - 100.0f * cells.size());

	drawtree();
	
}

void act_on_button_press(float x, float y) {

}
