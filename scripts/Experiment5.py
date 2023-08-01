# This script will test our Connectivity algorithm and LE-Lists algorithm
# on undirected graphs (symmetric graphs). 
# We test the running time under 96 core (192 hyperthreads) and 
# also the sequential running time with only one core.
# The script also test the baseline algorithms: DHS'21 (for the LDD-UF-JTB 
# connectivity implementation) and parlay (for LE-Lists algorithm) 
# The directed output of executing the code will be output to folder '../log/exp1'
from graphs import sym_graphs
from graphs import GRAPH_DIR
import subprocess
import os
import multiprocessing

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
os.makedirs(f"{CURRENT_DIR}/../log", exist_ok=True)  
os.makedirs(f"{CURRENT_DIR}/../log/exp5", exist_ok=True)  

global par_rounds
global seq_rounds
def run_Connectivity():
    print("Testing Our Parallel Connectivity Running Time")
    if not os.path.exists(f"{CURRENT_DIR}/../src/Connectivity"):
        subprocess.call(f'cd {CURRENT_DIR}/../src && make Connectivity', shell=True)
    # For each experiment, run 11 times and take the average of the last 10 time
    cc = f'{CURRENT_DIR}/../src/Connectivity'
    for key, val in sym_graphs.items():
        graph = val[0]
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp5/{key}_cc.out"
        print(f"Running {key}")
        cmd = f"numactl -i all {cc} {graph_in} -t {par_rounds} | tee -a {log_out}"
        subprocess.call(cmd, shell=True)
def run_LeList():
    print("Testing Our Parallel LE-List Running Time")
    if not os.path.exists(f"{CURRENT_DIR}/../src/LeList"):
        subprocess.call(f"cd {CURRENT_DIR}/../src && make LeList", shell=True)
    LeList = f"{CURRENT_DIR}/../src/LeList"
    for key, val in sym_graphs.items():
        graph = val[0]
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp5/{key}_lelist.out"
        print(f"Running {key}")
        cmd = f"numactl -i all {LeList} {graph_in} -t {par_rounds} | tee -a {log_out}"
        subprocess.call(cmd, shell=True)

def run_ConnectIt():
    print("Testing the LDD-UF-JTB connectivity implementation in ConnectIt")
    cmd = f"cd {CURRENT_DIR}/../baselines/gbbs/ && bazel build //benchmarks/Connectivity/..."
    subprocess.call(cmd, shell=True)
    cc = f"{CURRENT_DIR}/../baselines/gbbs/bazel-bin/benchmarks/Connectivity/ConnectIt/mains/unite_rem_cas_ldd"
    for key, val in sym_graphs.items():
        graph = val[0]
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp5/{key}_connectit.out"
        print(f"Running {key}")
        cmd =f"numactl -i all {cc} -s -b -r {par_rounds+1} {graph_in} | tee -a {log_out}"
        subprocess.call(cmd, shell=True)
def run_LeList_parlay():
    print("Testing parlaylib LE-List Running Time")
    os.makedirs(f"{CURRENT_DIR}/../parlaylib/build", exist_ok=True)
    cmd = f"cd {CURRENT_DIR}/../parlaylib/build && cmake .. -DPARLAY_EXAMPLES=On && cmake --build ."
    subprocess.call(cmd, shell = True)
    LeList = f"{CURRENT_DIR}/../parlaylib/build/examples/le_list"
    for key, val in sym_graphs.items():
        graph = val[0]
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp5/{key}_lelist_parlay.out"
        print(f"Running {key}")
        cmd = f"numactl -i all {LeList} {graph_in} -t {par_rounds +1} | tee -a {log_out}"
        subprocess.call(cmd, shell=True)
if __name__ == '__main__':
    global par_rounds, seq_rounds
    par_rounds = 1
    seq_rounds = 1
    run_Connectivity()
    run_LeList()
    run_ConnectIt()
    run_LeList_parlay()