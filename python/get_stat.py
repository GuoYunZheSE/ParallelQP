import os

rmin = tmin = 0
alpha = 0.0
iter_cnt = tcnt = 0
tot_r = tot_c = 0
tot_den = tot_k1 = tot_k2 = tot_ts = tot_rmse = 0.0
mrc = mcc = 0
rcr = ccr = 0.0
rt = tt = 0.0
cnt = 0.0

for i in range(10):
	cnt += 1
	while (not os.path.exists('result_%d.txt' % (i))):
		os.system('scp host%d:result.txt result_%d.txt' % (i, i))

	f = open('result_%d.txt' % (i))
	rmin = int(f.readline())
	tmin = int(f.readline())
	alpha = float(f.readline())
	iter_cnt = int(f.readline())
	tcnt += int(f.readline())
	tot_r += int(f.readline())
	tot_c += int(f.readline())
	tot_den += float(f.readline())
	tot_k1 += float(f.readline())
	tot_k2 += float(f.readline())
	tot_ts += float(f.readline())
	tot_rmse += float(f.readline())
	mrc = int(f.readline())
	mcc = int(f.readline())
	rcr += float(f.readline())
	ccr += float(f.readline())
	rt += float(f.readline())
	tt += float(f.readline())

avg_r = float(tot_r) / float(tcnt)
avg_c = float(tot_c) / float(tcnt)
avg_den = tot_den / tcnt
avg_k1 = tot_k1 / tcnt
avg_k2 = tot_k2 / tcnt
avg_ts = tot_ts / tcnt
avg_rmse = tot_rmse / tcnt
rcr /= cnt
ccr /= cnt
rt /= cnt
tt /= cnt

print('%d %d %lf %d %d %lf %lf %lf %lf %lf %lf %lf %d %d %lf %lf %lf %lf\n' % (rmin, tmin, alpha, iter_cnt, tcnt, avg_r, avg_c, avg_den, avg_k1, avg_k2, avg_ts, avg_rmse, mrc, mcc, rcr, ccr, rt, tt))


for i in range(10):
	if (i != 4):
		os.system('scp host%d:all_opsm.txt all_opsm_%d.txt' % (i, i));
		os.system('cat all_opsm_%d.txt >> all_opsm.txt' % (i))
