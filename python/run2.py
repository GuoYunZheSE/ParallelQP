import os

os.system('rm *.txt')
for i in range(10):
	if (i != 4):
		os.system('ssh host%d "rm *.txt"' % (i))
		os.system('scp partition2 quick-probing host%d:' % (i))


for i in range(10):
	if (i != 4):
		os.system('ssh -f host%d "./partition2 10 %d %d %d %lf | cat >> stdout.txt"' % (i, i, 100, 100, 0.02))
	else:
		os.system("./partition2 10 4 100 100 0.02 &")
