#ifndef _h_cell
#define _h_cell

#include <iostream>
#include <vector>

//cell object
class cell {
	int cellNum;
	std::vector<int> nets;
	int partitionNo; //1,2,3,or 4
	int TotalCriticalNo;
	char status; // 'p' = partitioned, 'u' = un-processed
	bool LockStatus; //ture if it is locked, false if it is not
public:
	~cell() {};
	cell(int no) {
		cellNum = no;
		TotalCriticalNo = 0;
		partitionNo = -1;
		status = 'u';
		LockStatus = false;
	};

	//config
	void setNets(int netNo);
	void CalCellProperty();
	//debug
	void PrintInfo();

	//helper functions
	int getCellNum() ;
	int getPartitionNo() ;
	int getNetsize() ;
	char getStatus() ;
	int getCritialNetCount();
	void getNets(std::vector<int>& ConnectedNets);
	//if the cell has been pushed to init soln, lock it
	void lockcell();
	bool CheckLockStatus();


};


#endif