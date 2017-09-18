/*
 * Pattern.cpp
 *
 *  Created on: Dec 30, 2011
 *      Author: lyl
 */
#include "Pattern.h"

Pattern::Pattern(): tr() {
	vld_row_size = 0;
	min_vld = 2;
	mmcc = mcc;
	// size 0
	vld_cnt_cnt.push_back(0);
	vld_cnt_rows.push_back(unordered_set<int>());
}

Pattern::~Pattern() {
	for (map<int, RowEntry*>::iterator it = rows.begin(); it != rows.end(); it++)
		delete it->second;
}

void Pattern::update_cnt(int cnt, int chg, int rid) {
	vld_cnt_cnt[cnt] += chg;
	if (cnt >= min_vld)
		vld_row_size += chg;

	if (chg > 0) {
		vld_cnt_rows[cnt].insert(rid);
		if (cnt >= min_vld)
			tr.insert(row_cover[rid]);
	}
	else {
		vld_cnt_rows[cnt].erase(rid);
		if (cnt >= min_vld)
			tr.remove(row_cover[rid]);
	}
}

void Pattern::insert(int cid, PickSet& ps) {
	int sz = vld_col[cid].size();

	// create new columns
	cols.push_back(cid);

	for (int i = 0; i < sz; i++) {
		int eid = vld_col[cid][i], rid = entry[eid].r, 
			val = entry[eid].val;

		if (killed.count(rid) > 0)
			continue;

		// update rows
		if (rows.count(rid) == 0) {
			// insert r to \mc{R}
			rows[rid] = new RowEntry(rid, val, row_cover[rid], 1);
			ps.add_cand_row(rid);
		} else {
			if (rows[rid]->max_val <= val) {
				// update r in \mc{R}
				int vld = rows[rid]->vld_cnt + 1;
				rows[rid]->vld_cnt = vld;
				rows[rid]->max_val = val;

				if (ps.choosen_rows.count(rid) > 0) {
					int sz2 = vld_row[rid].size();
					for (int j = rows[rid]->killed_col_idx; j < sz2; j++) {
						int e2 = vld_row[rid][j], c2 = entry[e2].c, 
							v = entry[e2].val;
						if (v < val)
							ps.add_killed_col(c2);
						else {
							rows[rid]->killed_col_idx = j;
							break;
						}
					}
				}
			} else {
				killed.insert(rid);
				rows.erase(rid);
			}
		}
	}
}


int Pattern::getsize() {
	return vld_row_size;
}

bool RowEntryCMP(RowEntry* r1, RowEntry* r2) {
	return r1->cover < r2->cover;
}

vector<int> Pattern::getrows(int& vld_cnt) {
	map<int, RowEntry*>::iterator it = rows.begin(), ed = rows.end();
	vector<RowEntry*> vec;
	int min_vld = max((int) ceil(cols.size() * alpha), 2);

	for ( ; it != ed; it++)
		if (it->second->vld_cnt >= min_vld)
			vec.push_back(it->second);
	sort(vec.begin(), vec.end(), RowEntryCMP);

	vector<int> res;
	vld_cnt = 0;

	int sz = (int) vec.size(), res_sz = 0, mmrc = mrc;
	for (int i = 0; i < sz; i++) {

		if (res_sz < rmin) {
			res_sz++;
			res.push_back(vec[i]->rid);
			vld_cnt += vec[i]->vld_cnt;
			mmrc = max(mmrc, vec[i]->cover + 1);
		} else {
			if (vec[i]->cover + 1 <= mmrc) {
				res_sz++;
				res.push_back(vec[i]->rid);
				vld_cnt += vec[i]->vld_cnt;
			}
		}
	}

	return res;
}

bool Pattern::check_inc(int cid) {
	int pre_mmrc = max(mrc, tr.select(rmin) + 1);

	int sz = vld_col[cid].size();
	vld_cnt_cnt.push_back(0);
	vld_cnt_rows.push_back(unordered_set<int>());

	int tmp_min_vld = max((int) ceil((cols.size() + 1) * alpha), 2);
	if (tmp_min_vld > min_vld) {
		unordered_set<int>::iterator it = vld_cnt_rows[min_vld].begin(),
				ed = vld_cnt_rows[min_vld].end();
		for ( ; it != ed; it++)
			tr.remove(rows[*it]->cover);
		vld_row_size -= vld_cnt_cnt[min_vld];
		min_vld = tmp_min_vld;
	}

	for (int i = 0; i < sz; i++) {
		int eid = vld_col[cid][i], rid = entry[eid].r, 
			val = entry[eid].val;

		if (killed.count(rid) > 0)
			continue;

		// update rows
		if (rows.count(rid) == 0) {
			// insert r to \mc{R}
			update_cnt(1, 1, rid);
		} else {
			if (rows[rid]->max_val <= val) {
				// update r in \mc{R}
				int vld = rows[rid]->vld_cnt + 1;
				update_cnt(vld - 1, -1, rid);
				update_cnt(vld, 1, rid);
			} else {
				update_cnt(rows[rid]->vld_cnt, -1, rid);
			}
		}
	}

	int mmrc = max(mrc, tr.select(rmin) + 1);
	int pre_mmcc = mmcc;
	mmcc = max(mmcc, col_cover[cid] + 1);

	if (vld_row_size < rmin || ((mmrc - mrc) + (mmcc - mcc) > 
		(pre_mmrc - mrc) + (pre_mmcc - mcc)))
		return false;
	return true;
}

vector<int> Pattern::getcols() {
	return cols;
}

bool Pattern::empty() {
	return rows.empty() && cols.empty();
}

void Pattern::restore() {
	for (map<int, RowEntry*>::iterator it = rows.begin(); it != rows.end(); it++)
		delete it->second;
	rows.clear();
	cols.clear();
	vld_cnt_cnt = vector<int>(1, 0);
	vld_cnt_rows = vector<unordered_set<int> >(1, unordered_set<int>());
	killed.clear();
	vld_row_size = min_vld = 2;
	tr.clear();
}
