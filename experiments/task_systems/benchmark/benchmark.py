import subprocess
import os
from datetime import datetime
import multiprocessing
import time
import re

output_folder = datetime.now().strftime('%Y-%m-%d_%H-%M-%S')
os.mkdir(output_folder)

def run_configuration(args):
    print(args)
    out = subprocess.Popen(args, 
           stdout=subprocess.PIPE, 
           stderr=subprocess.STDOUT)
    out.wait()
    stdout,stderr = out.communicate()

    file_name = '_'.join(args[1:])
    file_path = os.path.join(output_folder, file_name)
    
    if stderr is not None:
        with open(file_path + '.err', 'w') as ferr:
            ferr.write(stderr.decode('utf-8'))
    
    outstr = stdout.decode('utf-8')
    with open(file_path + '.out', 'w') as fout:
        fout.write(outstr)
    match = re.match('Running time: (\d*)ms', outstr)
    execution_time = match.group(1)
    print(f'Executed in: {execution_time} ms')

    assert stderr is None

SHORTEST_TASK_IN_NS=100
STEP_IN_NS=100
LONGEST_TASK_IN_NS=1500
CPU_COUNT=multiprocessing.cpu_count()
MAGIC_NUMBER=3000000000

def calculate_number_of_tasks(shortest_task_in_ns, longest_task_in_ns):
    average_task = (shortest_task_in_ns + longest_task_in_ns) / 2
    all_runtime = MAGIC_NUMBER * CPU_COUNT
    all_task = int((all_runtime/average_task))
    return all_task


for shortest in range(SHORTEST_TASK_IN_NS, LONGEST_TASK_IN_NS-STEP_IN_NS+1, STEP_IN_NS):
    for longest in range(shortest+ STEP_IN_NS, LONGEST_TASK_IN_NS+1, STEP_IN_NS):
        task_count = str(calculate_number_of_tasks(shortest, longest))
        for task_system in [0, 1, 2, 3, 4, 5, 6]:
            run_configuration(['/home/kovi/Repos/miscellaneous/build/task_systems/task_systems', str(shortest), str(longest), task_count, str(task_system)])
            time.sleep(2)



