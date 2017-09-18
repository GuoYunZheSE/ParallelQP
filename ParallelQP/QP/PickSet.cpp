#include "PickSet.h"

bool PQ_CMP::operator() (const int& a, const int& b) const {
	return col_cover[a] < col_cover[b] || 
		(col_cover[a] == col_cover[b] && rand_col[a] < rand_col[b]);
}

bool ROW_CMP::operator() (const int& a, const int& b) const {
//	return rand_row[a] < rand_row[b];
	return (row_cover[a] < row_cover[b]) || 
		(row_cover[a] == row_cover[b] && rand_row[a] < rand_row[b]);
}

PickSet::PickSet() { }

PickSet::PickSet(int n) {
	vector<int> v = vector<int>(n, 0);
	for (int i = 0; i < n; i++)
		v[i] = i;
	cols = priority_queue<int, vector<int>, PQ_CMP>(v.begin(), v.end());
	choosen_cols.clear();
	killed_cols.clear();
	cand_rows.clear();
	choosen_rows.clear();
}

void PickSet::init(int n) {
	vector<int> v = vector<int>(n, 0);
	for (int i = 0; i < n; i++)
		v[i] = i;
	cols = priority_queue<int, vector<int>, PQ_CMP>(v.begin(), v.end());
	choosen_cols.clear();
	killed_cols.clear();
	cand_rows.clear();
	choosen_rows.clear();
}

int PickSet::pick(Pattern& ptn) {
	if (ptn.empty()) {
		if (cols.empty())
			return -1;
		int res = cols.top();
		cols.pop();
		while (choosen_cols.count(res) > 0) {
			if (cols.empty())
				return -1;
			res = cols.top();
			cols.pop();
		}
		choosen_cols.insert(res);
		return res;
	} else {
		while (!cand_rows.empty()) {
			int rid = *cand_rows.begin();
			if (ptn.rows.count(rid) == 0) {
				cand_rows.erase(rid);
				continue;
			}

			int sz = vld_row[rid].size(), val1 = ptn.rows[rid]->max_val;

			int res = -1, min_cover = -1;

			for (int i = 0; i < sz; i++) {
				int eid = vld_row[rid][i], cid = entry[eid].c,
						val2 = entry[eid].val;
				if (killed_cols.count(cid) > 0 || choosen_cols.count(cid) > 0)
					continue;
				if (val1 <= val2 && (min_cover < 0 || col_cover[cid] < min_cover)) {
					// found
					res = cid;
					min_cover = col_cover[cid];
				}
			}
			cand_rows.erase(rid);

			if (res >= 0) {
				choosen_rows.insert(rid);
				choosen_cols.insert(res);
				return res;
			}

		}
		return -1;
	}
}

void PickSet::add_cand_row(int rid) {
	cand_rows.insert(rid);
}

void PickSet::add_killed_col(int cid) {
	killed_cols.insert(cid);
}

void PickSet::restore() {
	killed_cols.clear();
	cand_rows.clear();
	choosen_rows.clear();
}

bool PickSet::empty() {
	return cols.empty();
}
