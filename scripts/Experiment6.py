from graphs import dir_graphs
from graphs import GRAPH_DIR
from algorithms import Algorithms
import subprocess
import os
import multiprocessing

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
os.makedirs(f"{CURRENT_DIR}/../log", exist_ok=True)  
os.makedirs(f"{CURRENT_DIR}/../log/exp6", exist_ok=True) 
def SCC_ROUND(graph, file_out):
    file_in = f"{GRAPH_DIR}/{dir_graphs[graph][0]}.bin"
    scc = Algorithms["Ours_scc_round"]
    dlarge = "-large" if (graph=="HL12") else ""
    cmd_plain=f"numactl -i all {scc} {file_in} -t 0 {dlarge} >> {file_out}"
    cmd_final= f"numactl -i all {scc} {file_in} -t 0 -local_reach  -local_scc -status {dlarge} >>{file_out}"
    cmds = [cmd_plain, cmd_final]
    for cmd in cmds:
        subprocess.call(cmd, shell=True)
def run_SCC_ROUND():
    graphs = ["LJ", "TW", "SD", "CW", "HL12", "CH5", "GL2", "GL10", "GL20", "COS5", "SQR", "SQR_s"]
    for g in graphs:
        if (g not in dir_graphs.keys()):
            continue
        SCC_ROUND(g, f"{CURRENT_DIR}/../log/exp6/{g}_round.out")
if __name__ == "__main__":
    run_SCC_ROUND()