from graphs import dir_graphs
from graphs import GRAPH_DIR
from algorithms import Algorithms
import subprocess
import os
import multiprocessing

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
os.makedirs(f"{CURRENT_DIR}/../log", exist_ok=True)  
os.makedirs(f"{CURRENT_DIR}/../log/exp3", exist_ok=True) 

global par_rounds
def SCC_breakdown(graph, file_out):
    file_in = f"{GRAPH_DIR}/{dir_graphs[graph][0]}.bin"
    scc = Algorithms["Ours_scc"]
    dlarge = "-large" if (graph=="HL12") else ""
    cmd_plain=f"numactl -i all {scc} {file_in} {dlarge} -t {par_rounds} >> {file_out}"
    cmd_vgc1= f"numactl -i all {scc} {file_in} {dlarge} -t {par_rounds} -local_reach >> {file_out}"
    cmd_final = f"numactl -i all {scc} {file_in} {dlarge} -t {par_rounds} -local_reach -local_scc >> {file_out}"
    cmds = [cmd_plain, cmd_vgc1, cmd_final]
    print(f"Testing Our SCC Breakdown on graph {graph}")
    for cmd in cmds:
        subprocess.call(cmd, shell=True)
def GBBS_breakdown(graph, file_out):
    file_in = f"{GRAPH_DIR}/{dir_graphs[graph][0]}.bin"
    GBBS_scc = Algorithms["GBBS_scc"]
    dlarge = "-large" if (graph=="HL12") else ""
    cmd_gbbs= f"numactl -i all {GBBS_scc} {dlarge} -b -beta 1.5 -rounds {par_rounds} {file_in} >> {file_out}"
    print(f"Testing GBBS Breakdown on graph {graph}")
    subprocess.call(cmd_gbbs, shell=True)

def run_breakdown():
    graphs = ["LJ", "TW", "SD", "CW", "HL12", "CH5", "GL2", "GL10", "GL20", "COS5", "SQR", "SQR_s"]
    for g in graphs:
        if (g not in dir_graphs.keys()):
            continue
        scc_out = f"{CURRENT_DIR}/../log/exp3/{g}_break.out"
        gbbs_out = f"{CURRENT_DIR}/../log/exp3/{g}_break_gbbs.out"
        SCC_breakdown(g, scc_out)
        GBBS_breakdown(g, gbbs_out)

if __name__ == "__main__":
    par_rounds = 1
    run_breakdown()