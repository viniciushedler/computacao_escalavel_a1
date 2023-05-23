import subprocess
import sys


comando = ["python3", "mock/mock.py", sys.argv[1]]
subprocess.Popen(comando, start_new_session=True)