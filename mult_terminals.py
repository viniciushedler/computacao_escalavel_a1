import threading
import subprocess
from time import sleep

def run_script(num):
    subprocess.call(['python3', 'terminal_comand.py', str(num)])

threads = []
for num in range(50):
    thread = threading.Thread(target=run_script, args=(num,))
    threads.append(thread)

for thread in threads:
    sleep(1)
    thread.start()

for thread in threads:
    thread.join()