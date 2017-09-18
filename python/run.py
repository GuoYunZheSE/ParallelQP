import os

rmin_l = [50]
tmin_l = [50]
alpha_l = [0.10]
m = 480891
n = 17770

ratio = 1.0
idx = 1
db = 'kdd_netflix%d.db' % (idx)
mm = int(m * ratio)
nn = int(n * ratio)

for i in range(10):
	if (i != 4):
		os.system('ssh host%d "rm *.txt"' % (i))
		os.system('scp partition quick-probing host%d:' % (i))

for dt in range(len(alpha_l)):
	os.system('rm *.txt')
	rmin = rmin_l[dt]
	tmin = tmin_l[dt]
	alpha = alpha_l[dt]

	for i in range(10):
		if (i != 4):
			os.system('ssh -f host%d "./partition 10 %d %s %d %d %d %d %lf | cat >> stdout.txt"' % (i, i, db, mm, nn, rmin, tmin, alpha))
	os.system("./partition 10 4 %s %d %d  %d %d %lf && python get_stat.py >> stat1.dat" % (db, mm, nn, rmin, tmin, alpha))
os.system('mv all_opsm.txt all_opsm_netflix.txt')

