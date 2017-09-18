import os

for i in range(10):
	os.system('ssh host%d "killall -e partition3"' % (i))
	os.system('ssh host%d "killall -e quick-probing"' % (i))
