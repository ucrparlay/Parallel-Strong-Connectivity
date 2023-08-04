from graphs import dir_graphs
from graphs import GRAPH_DIR
import subprocess
import os
import multiprocessing

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
os.makedirs(f"{CURRENT_DIR}/../log", exist_ok=True)  
os.makedirs(f"{CURRENT_DIR}/../log/exp6", exist_ok=True) 
def SCC_ROUND(graph, file_out):
    file_in = f"{GRAPH_DIR}/{dir_graphs[graph][0]}.bin"
    scc = f'{CURRENT_DIR}/../src/scc'
    cmd_plain=f"numactl -i all {scc} {file_in} -t 0 | tee -a {file_out}"
    cmd_final= f"numactl -i all {scc} {file_in} -t 0 -local_reach  -local_scc | tee -a {file_out}"
    cmds = [cmd_plain, cmd_final]
    for cmd in cmds:
        subprocess.call(cmd, shell=True)
def run_SCC_ROUND():
    print("compile algorithms")
    subprocess.call(f'cd {CURRENT_DIR}/../src && make scc -B ROUND=1 -j 8', shell=True)
    graphs = ["LJ", "TW", "SD", "CH5", "GL2", "GL10", "GL20", "COS5", "SQR", "SQR_s"]
    for g in graphs:
        # try:
        SCC_ROUND(g, f"{CURRENT_DIR}/../log/exp6/{g}_round.out")
        # except:
        #     continue
if __name__ == "__main__":
    run_SCC_ROUND()