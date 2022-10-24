import os, sys

try:
	from watchdog.observers import Observer
except:
	print("\x1b[31mYou should install watchdog!\x1b[m")
	print(
		f"\x1b[1m    {sys.orig_argv[0]} "
		"-m pip install watchdog\x1b[m"
	)
	exit(1)

Observer()

os.chdir("docs")
os.system("doxygen")
os.system("make html")
