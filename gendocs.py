import os

os.chdir("docs")
os.system("doxygen")
os.system("make html")
