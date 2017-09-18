#include "Utility.h"
#include "Pattern.h"
#include "PickSet.h"
#include <fstream>
using namespace std;

// number of rows and columns
int m, n;

// values
vector<Entry> entry;

// indices of entry of rows and columns
vector<vector<int> > vld_row, vld_col;

// test entries
vector<Entry> test_entry;

// test entries' indices
vector<vector<int> > test_row, test_col;

// coveraged
vector<int> row_cover, col_cover;

// cmin, rmin
int cmin, rmin;

// alpha
double alpha;

// max row_cover, col_cover
int mrc, mcc;

// random sequence of rows
vector<int> rand_row, rand_col;

struct OPSM {
	vector<int> rows, cols;
	double kscore;
	int vld_cnt;

	OPSM(vector<int>& r, vector<int>& c, double k, int v): 
		rows(r), cols(c), kscore(k), vld_cnt(v) { }
};

vector<OPSM> opsm;

void init(const char* filename, const char* fn_test, const char* fn_rc, 
			const char* fn_cc) {
	FILE* f = fopen(filename, "r");
	int r, c, val, ptr = 0;

	vld_row = vector<vector<int> >(m, vector<int>());
	vld_col = vector<vector<int> >(n, vector<int>());
	
	test_row = vector<vector<int> >(m, vector<int>());
	test_col = vector<vector<int> >(n, vector<int>());

	while (fscanf(f, "%d%d%d", &r, &c, &val) == 3) {
		entry.push_back(Entry(r, c, val));
		vld_row[r].push_back(ptr);
		vld_col[c].push_back(ptr);
		ptr++;
	}
	fclose(f);

	f = fopen(fn_test, "r");
	ptr = 0;
	while (fscanf(f, "%d%d%d", &r, &c, &val) == 3) {
		test_entry.push_back(Entry(r, c, val));
		test_row[r].push_back(ptr);
		test_col[c].push_back(ptr);
		ptr++;
	}
	fclose(f);

	row_cover = vector<int>(m, 0);
	col_cover = vector<int>(n, 0);
	rand_row = vector<int>(m, 0);
	rand_col = vector<int>(n, 0);
	mrc = mcc = 0;
	
	f = fopen(fn_rc, "r");
	for (int i = 0; i < m; i++) {
		fscanf(f, "%d", &row_cover[i]);
		mrc = max(mrc, row_cover[i]);
	}
	fclose(f);
	
	f = fopen(fn_cc, "r");
	for (int i = 0; i < n; i++) {
		fscanf(f, "%d", &col_cover[i]);
		mcc = max(mcc, col_cover[i]);
	}
	fclose(f);
}

void calc_dist(OPSM& opsm) {
	vector<int>& row = opsm.rows;
	vector<int>& col = opsm.cols;
	unordered_set<int> row_set(row.begin(), row.end());
	map<int, vector<double>* > pre_val, tru_val;
	map<int, vector<bool>* > is_pd;
	int vld_cnt = 0;

	// init
	for (int i = 0; i < (int) row.size(); i++) {
		int rid = row[i];
		pre_val[rid] = new vector<double>();
		tru_val[rid] = new vector<double>();
		is_pd[rid] = new vector<bool>();
	}

	for (int i = 0; i < col.size(); i++) {
		int cid = col[i];
		
		int sz = (int) vld_col[cid].size();
		for (int j = 0; j < sz; j++) {
			int eid = vld_col[cid][j], rid = entry[eid].r,
				val = entry[eid].val;
			if (row_set.count(rid) == 0)
				continue;
			pre_val[rid]->push_back(val);
			tru_val[rid]->push_back(val);
			is_pd[rid]->push_back(false);
			vld_cnt++;
		}
		sz = (int) test_col[cid].size();
		for (int j = 0; j < sz; j++) {
			int eid = test_col[cid][j], rid = test_entry[eid].r,
				val = test_entry[eid].val;
			if (row_set.count(rid) == 0)
				continue;
			pre_val[rid]->push_back(-1);
			tru_val[rid]->push_back(val);
			is_pd[rid]->push_back(true);
		}
	}
	
	map<int, vector<double>* >::iterator it = pre_val.begin(), 
		ed = pre_val.end();
	
	for ( ; it != ed; it++) {
		vector<double>& val = *(it->second);
		int sz = val.size();

		for (int j = 0; j < sz; j++) {
			if (val[j] == -1) {
				int ll = -1, rr = -1, pos1 = j, pos2 = j;
				while (pos1 >= 0 && val[pos1] == -1)
					pos1--;
				if (pos1 >= 0)
					ll = val[pos1];
				pos2 = j;
				while (pos2 < (int) val.size() && val[pos2] == -1)
					pos2++;
				if (pos2 < (int) val.size())
					rr = val[pos2];
				if (ll < 0)
					ll = rr;
				else if (rr < 0)
					rr = ll;
				double pd = (double) (ll + rr) / 2;
				for (int k = pos1 + 1; k <= pos2 - 1; k++)
					val[k] = pd;
			}
		}
	}

	map<int, vector<double>* >::iterator it1, it2;
	map<int, vector<bool>* >::iterator it3;
	double kscore = 0, kcnt = 0;
	for (it1 = pre_val.begin(), it2 = tru_val.begin(), it3 = is_pd.begin();
		it1 != pre_val.end(); it1++, it2++, it3++) {
		
		vector<double>& val1 = *(it1->second);
		vector<double>& val2 = *(it2->second);
		vector<bool>& pd = *(it3->second);
		
		int sz = val1.size();
		
		if (sz < 2)
			continue;
	
		double dist = 0, cnt = 0;

		for (int i = 0; i < sz; i++)
			cnt += (int) pd[i];
		//if (cnt < 1)
		//	continue;

		for (int i = 0; i < sz; i++) 
			for (int j = 0; j < sz; j++) {
				if (i == j)
					continue;
				if (val2[i] == val2[j]) {
					if (val1[i] != val1[j])
						dist += 0.5;
				} else if (val2[i] > val2[j]) {
					if (val1[i] < val1[j])
						dist += 1.0;
					else if (val1[i] == val1[j])
						dist += 0.5;
				} else if (val2[i] < val2[j]) {
					if (val1[i] > val1[j])
						dist += 1.0;
					else if (val1[i] == val1[j])
						dist += 0.5;
				}
			}
		dist /= (double) (sz - 1) * sz;
		
		kscore += dist;
		kcnt++;
	}

	// clear
	it = pre_val.begin(), ed = pre_val.end();
	for ( ; it != ed; it++)
		delete it->second;
	it = tru_val.begin(), ed = tru_val.end();
	for ( ; it != ed; it++)
		delete it->second;

	if (kcnt == 0)
		kscore = -1;
	else
		kscore /= kcnt;
	opsm.kscore = kscore;
	opsm.vld_cnt = vld_cnt;
}

void add_opsm(vector<int>& row, vector<int>& col) {
	opsm.push_back(OPSM(row, col, -1, -1));	
}

void work() {
	srand(time(NULL));
	clock_t start = clock();

	int iter_cnt = 0;
	double d_tot = 0, avg_den = 0;

	while (iter_cnt < 5) {
		iter_cnt++;
		// inner iteration
		PickSet rps(n);
		Pattern ptn;

		// generate random order of rows
		for (int i = 0; i < m; i++)
			rand_row[i] = i;
		for (int i = 0; i < m; i++)
			swap(rand_row[i], rand_row[i + rand() % (m - i)]);

		for (int i = 0; i < n; i++)
			rand_col[i] = i;
		for (int i = 0; i < n; i++)
			swap(rand_col[i], rand_col[i + rand() % (n - i)]);

		bool ptn_valid = false;

		// inner iteration
		while (!rps.empty()) {
			int c = rps.pick(ptn);

			bool inc_flag;
			if (c < 0)
				inc_flag = false;
			else
				inc_flag = ptn.check_inc(c);
			// check whether a valid pattern exists and we can improve
			// the pattern
			if (ptn_valid && !inc_flag) {
				// adding new pattern
				int vld_cnt = 0;
				vector<int> rows = ptn.getrows(vld_cnt), cols = ptn.getcols();
				int sz1 = rows.size(), sz2 = cols.size();
				double den = (double) vld_cnt / sz1 / sz2;
				avg_den += den;
				add_opsm(rows, cols);

				for (int i = 0; i < sz1; i++) {
					row_cover[rows[i]] ++;
					if (row_cover[rows[i]] > mrc)
						mrc = row_cover[rows[i]];
				}
				for (int i = 0; i < sz2; i++) {
					col_cover[cols[i]] ++;
					if (col_cover[cols[i]] > mcc)
						mcc = col_cover[cols[i]];
				}

				//add_opsm(rows, cols);
				d_tot += sz1 * sz2;
				ptn.restore();
				rps.restore();
				ptn_valid = false;
				continue;
			}

			if (c < 0) {
				ptn.restore();
				rps.restore();
				ptn_valid = false;
				continue;
			}

			ptn.insert(c, rps);
			int vld_row_cnt = ptn.getsize();

			if (vld_row_cnt >= rmin && (int) ptn.cols.size() >= cmin) {
				ptn_valid = true;
			}
		}
		int rc = 0, cc = 0;
		for (int i = 0; i < m; i++)
			rc += row_cover[i] > 0;
		for (int i = 0; i < n; i++)
			cc += col_cover[i] > 0;
		printf("Coverage Ratio: %lf %lf\n", (double) rc / m, (double) cc / n);
	}
}

void output(const char* fn, const char* fn_rc, const char* fn_cc) {
	// output opsm sizes and kscore
	// r, c, kscore, valid entry, cover new
	clock_t st = clock();
	for (int i = 0; i < (int) opsm.size(); i++)
		calc_dist(opsm[i]);
	
	FILE* f = fopen(fn, "w");
	fprintf(f, "%lf\n", (double) (clock() - st) / CLOCKS_PER_SEC);
	
	for (int i = 0; i < (int) opsm.size(); i++) {
		fprintf(f, "%d %d %lf %d\n", (int) opsm[i].rows.size(), 
			(int) opsm[i].cols.size(), opsm[i].kscore, opsm[i].vld_cnt);
	}
	fclose(f);

	// output rc and cc
	f = fopen(fn_rc, "w");
	for (int i = 0; i < m; i++)
		fprintf(f, "%d\n", row_cover[i]);
	fclose(f);
	
	f = fopen(fn_cc, "w");
	for (int i = 0; i < n; i++)
		fprintf(f, "%d\n", col_cover[i]);
	fclose(f);
}

int main(int argc, char** argv) {
	clock_t start = clock();
	string fn_train, fn_test, fn_rc, fn_cc, fn_out;

	if (argc >= 11) {
		// ./quick-probing in_u1.base in_u1.test 943 1682 25 25 0.08 rc.txt cc.txt out.txt
		fn_train = argv[1];
		fn_test = argv[2];
		m = atoi(argv[3]);
		n = atoi(argv[4]);
		rmin = atoi(argv[5]);
		cmin = atoi(argv[6]);
		alpha = strtod(argv[7], NULL);
		fn_rc = argv[8];
		fn_cc = argv[9];
		fn_out = argv[10];
	} else {
		return 0;
	}

	init(fn_train.c_str(), fn_test.c_str(), fn_rc.c_str(), fn_cc.c_str());
	work();
	output(fn_out.c_str(), fn_rc.c_str(), fn_cc.c_str());
	printf("Sub-QP time = %lf\n", (double) (clock() - start) / CLOCKS_PER_SEC);
}

