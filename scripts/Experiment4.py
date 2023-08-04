from graphs import dir_graphs
from graphs import GRAPH_DIR
import subprocess
import os
import multiprocessing

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
os.makedirs(f"{CURRENT_DIR}/../log", exist_ok=True)  
os.makedirs(f"{CURRENT_DIR}/../log/exp4", exist_ok=True) 

global par_rounds
Tau = [2**i for i in range(18)]
def SCC_tau(graph, n_thread, cores, file_out):
    scc = f'{CURRENT_DIR}/../src/scc'
    numa = "" if (n_thread==1) else "numactl -i all"
    print(f"Running on graph {graph}")
    file_in = f"{GRAPH_DIR}/{dir_graphs[graph][0]}.bin"
    for tau in Tau:
        cmd = f"PARLAY_NUM_THREADS={n_thread} taskset -c {cores} {numa} {scc} {file_in} -local_reach -local_scc -tau {tau} -t {par_rounds} | tee -a {file_out}"
        subprocess.call(cmd, shell=True)

def SCC_tau_scale(graph):
    threads={192: '0-191',96: '0-95',48: '0-95:2',24: '0-95:4',4: '0-15:4'}
    for thread, cores in threads.items():
        file_out = f"{CURRENT_DIR}/../log/exp4/{graph}_{thread}.out"
        SCC_tau(graph, thread, cores, file_out)
def run_SCC_tau_scale():
    print("compile algorithms")
    subprocess.call(f'cd {CURRENT_DIR}/../src && make scc -B -j 8', shell=True)
    graphs = ["TW", "SD", "CW", "GL5", "COS5","SQR_s"]
    for g in graphs:
        try:
            SCC_tau_scale(g)
        except:
            continue
if __name__ == "__main__":
    global par_rounds
    par_rounds = 10
    run_SCC_tau_scale()