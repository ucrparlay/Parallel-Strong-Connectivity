from graphs import dir_graphs
from graphs import GRAPH_DIR
import subprocess
import os
import multiprocessing

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
os.makedirs(f"{CURRENT_DIR}/../log", exist_ok=True)  
os.makedirs(f"{CURRENT_DIR}/../log/exp3", exist_ok=True) 

global par_rounds
def SCC_breakdown(graph, file_out):
    file_in = f"{GRAPH_DIR}/{dir_graphs[graph][0]}.bin"
    scc = f'{CURRENT_DIR}/../src/scc'
    cmd_plain=f"numactl -i all {scc} {file_in} -t {par_rounds} | tee -a {file_out}"
    cmd_vgc1= f"numactl -i all {scc} {file_in} -t {par_rounds} -local_reach | tee -a {file_out}"
    cmd_final = f"numactl -i all {scc} {file_in} -t {par_rounds} -local_reach -local_scc | tee -a {file_out}"
    cmds = [cmd_plain, cmd_vgc1, cmd_final]
    for cmd in cmds:
        subprocess.call(cmd, shell=True)
def GBBS_breakdown(graph, file_out):
    file_in = f"{GRAPH_DIR}/{dir_graphs[graph][0]}.bin"
    GBBS_DIR = f"{CURRENT_DIR}/../baselines/gbbs"
    GBBS_scc = f"{GBBS_DIR}/bazel-bin/benchmarks/StronglyConnectedComponents/RandomGreedyBGSS16/StronglyConnectedComponents_main"
    cmd_gbbs= f"numactl -i all {GBBS_scc} -b -beta 1.5 -rounds {par_rounds} {file_in} | tee -a {file_out}"
    subprocess.call(cmd_gbbs, shell=True)

def run_breakdown():
    print("compile with BREAKDOWN flag")
    subprocess.call(f'cd {CURRENT_DIR}/../src && make scc BREAKDOWN=1 -B', shell=True)
    GBBS_DIR = f"{CURRENT_DIR}/../baselines/gbbs"
    subprocess.call(f"cd {GBBS_DIR} && bazel build --define breakdown=1 //benchmarks/StronglyConnectedComponents/RandomGreedyBGSS16/...", shell=True)
    graphs = ["LJ", "TW", "SD", "CW", "CH5", "GL2", "GL20", "COS5", "SQR", "SQR’"]
    for g in graphs:
        scc_out = f"{CURRENT_DIR}/../log/exp3/{g}_break.out"
        gbbs_out = f"{CURRENT_DIR}/../log/exp3/{g}_break_gbbs.out"
        try:
            SCC_breakdown(g, scc_out)
        except:
            continue
        try:
            GBBS_breakdown(g, gbbs_out)
        except:
            continue    

if __name__ == "__main__":
    par_rounds = 1
    run_breakdown()