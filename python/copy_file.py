import os

for i in range(2, 10):
	os.system('scp kdd.db host%d:' % (i))
