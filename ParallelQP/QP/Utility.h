/*
 * Utility.h
 *
 *  Created on: Dec 30, 2011
 *      Author: lyl
 */

#ifndef UTILITY_H_
#define UTILITY_H_

#include <iostream>
#include <vector>
#include <unordered_set>
#include <set>
#include <map>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <string>
#include <ctime>
using namespace std;

struct Entry {
	int r, c, val;
	Entry(int _r, int _c, int _val): r(_r), c(_c), val(_val) { }
};

// number of rows and columns
extern int m, n;

// values
extern vector<Entry> entry;

// indexes of entry of rows and columns
extern vector<vector<int> > vld_row, vld_col;

// coverage
extern vector<int> row_cover, col_cover;

// cmin, rmin
extern int cmin, rmin;

// alpha
extern double alpha;

// random sequence of rows and columns
extern vector<int> rand_row, rand_col;

// max row_cover, col_cover
extern int mrc, mcc;

// forward definition
struct Pattern;
struct PickSet;

#endif /* UTILITY_H_ */
