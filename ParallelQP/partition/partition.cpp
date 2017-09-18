#include <iostream>
#include <fstream>
#include <db_cxx.h>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <map>
#include <cmath>

const int num_of_node = 10;
const int m = 1000990;
const int n = 624961;
using namespace std;

struct row_entry {
	int cid, val;
};

bool exists(const char *filename) {
	ifstream ifile(filename);
    return ifile;
}

int seed, me, run_cnt, row_map[m], col_map[n], row_idx[m], col_idx[n];
int rc[m], cc[n];
int rmin, cmin;


vector<int> row_par[num_of_node], col_par[num_of_node];
map<int, int> row_par_map[num_of_node], col_par_map[num_of_node];

double alpha;
char cmd[150];

void create_partition() {
	for (int i = 0; i < num_of_node; i++) {
		row_par[i].clear();
		col_par[i].clear();
		row_par_map[i].clear();
		col_par_map[i].clear();
	}
	
	for (int i = 0; i < m; i++)
		row_idx[i] = i;
	for (int i = 0; i < m; i++)
		swap(row_idx[i], row_idx[i + rand() % (m - i)]);
		
	for (int i = 0; i < n; i++)
		col_idx[i] = i;
	for (int i = 0; i < n; i++)
		swap(col_idx[i], col_idx[i + rand() % (n - i)]);

	for (int i = 0; i < m; i++) {
		int par = row_idx[i] % num_of_node;
		row_map[i] = par;
		row_par[par].push_back(i);
		row_par_map[par][i] = (int) row_par[par].size() - 1;
	}
	for (int i = 0; i < n; i++) {
		int par = col_idx[i] % num_of_node;
		col_map[i] = par;
		col_par[par].push_back(i);
		col_par_map[par][i] = (int) col_par[par].size() - 1;
	}
}

void create_files() {
	Db db(NULL, 0);
	db.open(NULL, "kdd.db", "NULL", DB_HASH, 0, 0);
	// write input matrix files
	FILE* f[num_of_node];
	FILE* ft[num_of_node];

	for (int i = 0; i < num_of_node; i++) {
		char fn[25];
		sprintf(fn, "in.%d.txt", i);
		f[i] = fopen(fn, "w");
		sprintf(fn, "test.%d.txt", i);
		ft[i] = fopen(fn, "w");
	}
	
	for (int i = 0; i < (int) row_par[me].size() ; i++) {
		int rid = row_par[me][i];
		Dbt key(&rid, sizeof(rid));
		Dbt result;

		db.get(NULL, &key, &result, 0);
		int sz = result.get_size() / sizeof(row_entry);
		row_entry* row = (row_entry*) result.get_data();
		for (int j = 0; j < sz; j++) {
			int cid = row[j].cid, pid = col_map[cid];
			
			// partition with 80-20 percentages
			if (j % 5 > 0) {
				fprintf(f[pid], "%d %d %d\n", row_par_map[me][rid], col_par_map[pid][cid], row[j].val);
			} else {
				fprintf(ft[pid], "%d %d %d\n", row_par_map[me][rid], col_par_map[pid][cid], row[j].val);
			}
		}
	}
	
	for (int i = 0; i < num_of_node; i++) {
		fclose(f[i]);
		fclose(ft[i]);
	}
	db.close(0);

	// write rc, cc files
	char fn[25];
	FILE* ff;	
	sprintf(fn, "rc.%d.txt", me);
	ff = fopen(fn, "w");
	for (int i = 0; i < (int) row_par[me].size(); i++)
		fprintf(ff, "%d\n", rc[row_par[me][i]]);
	fclose(ff);

	sprintf(fn, "cc.%d.txt", me);
	ff = fopen(fn, "w");
	for (int i = 0; i < (int) col_par[me].size(); i++)
		fprintf(ff, "%d\n", cc[col_par[me][i]]);
	fclose(ff);
}

void run_QP() {
	// run quick-probing command
	for (int i = 0; i < num_of_node; i++) {
		int pid = (me + i) % num_of_node, nxt_pid = (pid + 1) % num_of_node, 
			pre_pid = (pid + num_of_node - 1) % num_of_node;
		
		// run
		printf("%d: Running on in.%d.txt\n", me, i);
		sprintf(cmd, "./quick-probing in.%d.txt test.%d.txt %d %d %d %d %lf rc.%d.txt cc.%d.txt opsm_%d_%d.txt", 
			pid, pid, (int) row_par[me].size(), (int) col_par[pid].size(), rmin, cmin, alpha,
			me, pid, me, run_cnt++);
		system(cmd);

		if (i != num_of_node - 1) {
			// send (send to previous)
			printf("%d: Sending...\n", me);
			sprintf(cmd, "scp ~/cc.%d.txt host%d: && rm ~/cc.%d.txt", pid, 
				(me + num_of_node - 1) % num_of_node, pid); // "localhost" should be changed to the next machine
			system(cmd);
			/*
			sprintf(cmd, "rm ~/cc.%d.txt", pid);
			system(cmd);
			*/

			// receive (wait for next)
			char next_cc[15];
			sprintf(next_cc, "cc.%d.txt", nxt_pid);
			clock_t st = clock();
			printf("%d: Waiting...\n", me);
			while (!exists(next_cc));
			printf("%d: Finish waiting. Waiting time = %lf\n", me, (double) (clock() - st) / CLOCKS_PER_SEC);
		}
	}
	
	// final send
	int pid_rc = me, pid_cc = (me + num_of_node - 1) % num_of_node;
	sprintf(cmd, "mv rc.%d.txt rc.fin.%d.txt && mv cc.%d.txt cc.fin.%d.txt", 
		pid_rc, pid_rc, pid_cc, pid_cc);
	system(cmd);
	for (int i = 0; i < num_of_node; i++)
		if (i != me) {
			sprintf(cmd, "scp rc.fin.%d.txt cc.fin.%d.txt host%d:~/", 
				pid_rc, pid_cc, i);
			system(cmd);
		}
	// final receive
	for (int i = 0; i < num_of_node; i++) {
		char fn[15];
		sprintf(fn, "rc.fin.%d.txt", i);
		while (!exists(fn));
		sprintf(fn, "cc.fin.%d.txt", i);
		while (!exists(fn));
	}
}

void combine() {
	// reload rc & cc
	for (int i = 0; i < num_of_node; i++) {
		char fn[20];
		int sz;
		// rc
		sprintf(fn, "rc.fin.%d.txt", i);
		FILE* f = fopen(fn, "r");
		sz = (int) row_par[i].size();
		for (int j = 0; j < sz; j++)
			fscanf(f, "%d", &rc[row_par[i][j]]);
		fclose(f);

		// cc
		sprintf(fn, "cc.fin.%d.txt", i);
		f = fopen(fn, "r");
		sz = (int) col_par[i].size();
		for (int j = 0; j < sz; j++)
			fscanf(f, "%d", &cc[col_par[i][j]]);
		fclose(f);
	}

	FILE* ff = fopen("rc_test.txt", "w");
	for (int i = 0; i < m; i++)
		fprintf(ff, "%d ", rc[i]);
	fprintf(ff, "\n");
	fclose(ff);

	ff = fopen("cc_test.txt", "w");
	for (int i = 0; i < n; i++)
		fprintf(ff, "%d ", cc[i]);
	fprintf(ff, "\n");
	fclose(ff);
	system("rm *.fin.*.txt");
}

double pre_rcr = -1, pre_ccr = -1;
time_t start;

bool finish() {
	int row_cover = 0, col_cover = 0, mrc = 0, mcc = 0;
	for (int i = 0; i < m; i++) {
		row_cover += (rc[i] > 0);
		mrc = max(mrc, rc[i]);
	}

	for (int i = 0; i < n; i++) {
		col_cover += (cc[i] > 0);
		mcc = max(mcc, cc[i]);	
	}

	printf("%d: row_cover = %d, col_cover = %d\n", 
		me, row_cover, col_cover);
	
	double rcr = (double) row_cover / m, ccr = (double) col_cover / n;
	
	if (row_cover == m && col_cover == n || 
		(abs(rcr - pre_rcr) < 1e-2 && abs(ccr - pre_ccr) < 1e-2)) {
		char fn[30];
		int tcnt = 0, tot_r = 0, tot_c = 0;
		double tot_den = 0, tot_k = 0, test_t = 0;

		for (int i = 0; i < run_cnt; i++) {
			sprintf(fn, "opsm_%d_%d.txt", me, i);
			FILE* f = fopen(fn, "r");

			int r, c, v;
			double k, tt = 0;
			fscanf(f, "%lf", &tt);
			test_t += tt;

			while (fscanf(f, "%d%d%lf%d", &r, &c, &k, &v) == 4) {
				tcnt++;
				tot_r += r;
				tot_c += c;
				tot_den += (double) v / r / c;
				tot_k += k;
			}
			fclose(f);
		}
		FILE* ff = fopen("result.txt", "w");
		fprintf(ff, "%d\n%d\n%d\n%lf\n%lf\n%d\n%d\n%lf\n%lf\n%d\n%lf\n", 
			tcnt, tot_r, tot_c, tot_den, tot_k, mrc, mcc, rcr, ccr, (int) (time(NULL) - start), test_t);
		fclose(ff);
		return true;
	}

	pre_rcr = rcr;
	pre_ccr = ccr;
	return false;
}

int main(int argc, char** argv) {
	start = time(NULL);
	if (argc < 6)
		cerr << "partition: 5 parameter." << endl;
	else {
		seed = atoi(argv[1]);
		me = atoi(argv[2]);
		rmin = atoi(argv[3]);
		cmin = atoi(argv[4]);
		alpha = strtod(argv[5], NULL);
	}
	run_cnt = 0;

	srand(seed);
	while (true) {
		printf("%d: Creating Partition..\n", me);
		create_partition();
		clock_t io1 = clock();
		printf("%d: Creating Files (in, rc, cc)..\n", me);
		create_files();
		printf("IO time = %lf\n", (double) (clock() - io1) / CLOCKS_PER_SEC);

		printf("%d: Start Running..\n", me);
		clock_t io2 = clock();
		run_QP();
		printf("QP time = %lf\n", (double) (clock() - io2) / CLOCKS_PER_SEC);
		printf("%d: Finish and start combining results..\n", me);
		
		combine();
		if (finish()) {
			printf("finish! Time = %d\n", (int) (time(NULL) - start));
			break;
		}
	}
}

