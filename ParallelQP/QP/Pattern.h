/*
 * SupportRowSet.h
 *
 *  Created on: Dec 30, 2011
 *      Author: lyl
 */

#ifndef PATTERN_H_
#define PATTERN_H_


#include <unordered_set>
#include <map>
#include <vector>
#include <algorithm>
#include "Utility.h"
#include "PickSet.h"
#include "Tree.h"
using namespace std;

struct RowEntry {
	int rid, max_val, cover, vld_cnt, killed_col_idx;

	RowEntry(int _rid, int _max_val, int _cover, int _vld_cnt):
		rid(_rid), max_val(_max_val), cover(_cover), vld_cnt(_vld_cnt),
		killed_col_idx(0) { }
};


struct Pattern {
	map<int, RowEntry*> rows;
	vector<int> cols;
	vector<int> vld_cnt_cnt;
	vector<unordered_set<int> > vld_cnt_rows;
	unordered_set<int> killed;
	Tree tr;
	int vld_row_size, min_vld, mmcc;

	Pattern();
	~Pattern();

	void insert(int cid, PickSet& ps);
	int getsize();
	void update_cnt(int cnt, int chg, int rid);
	vector<int> getrows(int& vld_cnt);
	vector<int> getcols();
	bool check_inc(int cid);
	bool empty();
	void restore();
};

#endif /* SUPPORTROWSET_H_ */
