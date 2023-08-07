# This script will test our Connectivity algorithm and LE-Lists algorithm
# on undirected graphs (symmetric graphs). 
# We test the running time under 96 core (192 hyperthreads) and 
# also the sequential running time with only one core.
# The script also test the baseline algorithms: DHS'21 (for the LDD-UF-JTB 
# connectivity implementation) and parlay (for LE-Lists algorithm) 
# The directed output of executing the code will be output to folder '../log/exp1'
from graphs import sym_graphs
from graphs import GRAPH_DIR
from algorithms import Algorithms
import subprocess
import os
import multiprocessing

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
os.makedirs(f"{CURRENT_DIR}/../log", exist_ok=True)  
os.makedirs(f"{CURRENT_DIR}/../log/exp5", exist_ok=True)  

global par_rounds
def run_Connectivity():
    print("Testing Our Parallel Connectivity Running Time")
    # For each experiment, run 11 times and take the average of the last 10 time
    cc = Algorithms["Ours_cc"]
    for key, val in sym_graphs.items():
        graph = val[0]
        dlarge= "-large" if (graph=="HL12") else ""
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp5/{key}_cc.out"
        print(f"Running on {key}")
        cmd = f"numactl -i all {cc} {graph_in} -t {par_rounds} {dlarge} >> {log_out}"
        subprocess.call(cmd, shell=True)
def run_LeList():
    print("Testing Our Parallel LE-List Running Time")
    LeList = Algorithms['Ours_lelist']
    for key, val in sym_graphs.items():
        graph = val[0]
        dlarge = "-large" if (graph=="HL12") else ""
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp5/{key}_lelist.out"
        print(f"Running on {key}")
        cmd = f"numactl -i all {LeList} {graph_in} -t {par_rounds} {dlarge} >> {log_out}"
        subprocess.call(cmd, shell=True)

def run_ConnectIt():
    print("Testing the LDD-UF-JTB connectivity implementation in ConnectIt")
    cc = Algorithms["GBBS_cc"]
    for key, val in sym_graphs.items():
        graph = val[0]
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp5/{key}_connectit.out"
        print(f"Running on {key}")
        cmd =f"numactl -i all {cc} -s -b -r {par_rounds+1} {graph_in} >> {log_out}"
        subprocess.call(cmd, shell=True)
def run_LeList_parlay():
    print("Testing parlaylib LE-List Running Time")
    LeList = Algorithms["parlay_lelist"]
    for key, val in sym_graphs.items():
        graph = val[0]
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp5/{key}_lelist_parlay.out"
        print(f"Running on {key}")
        cmd = f"numactl -i all {LeList} {graph_in} -t {par_rounds +1} >> {log_out}"
        subprocess.call(cmd, shell=True)
if __name__ == '__main__':
    global par_rounds
    par_rounds = 1
    run_Connectivity()
    run_LeList()
    run_ConnectIt()
    run_LeList_parlay()