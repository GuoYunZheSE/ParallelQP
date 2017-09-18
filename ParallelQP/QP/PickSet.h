#ifndef PICKSET_H_
#define PICKSET_H_

#include <vector>
#include <queue>
#include <set>
#include "Utility.h"
#include "Pattern.h"
using namespace std;

struct PQ_CMP {
	bool operator() (const int& a, const int& b) const;
};

struct ROW_CMP {
	bool operator() (const int& a, const int& b) const;
};

struct PickSet {
	priority_queue<int, vector<int>, PQ_CMP> cols;
	set<int, ROW_CMP> cand_rows;
	unordered_set<int> choosen_rows, killed_cols, choosen_cols;

	PickSet();
	PickSet(int n);
	void init(int n);
	int pick(Pattern& ptn);
	void restore();
	void add_killed_col(int cid);
	void add_cand_row(int rid);
	bool empty();
};

#endif
