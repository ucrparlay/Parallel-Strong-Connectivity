# This script will run our SCC algorithms on the directed. 
# Test our SCC running time under 96 cores 192 hyperthreads 
# and also the sequential running time with only one core.
# The script also test the baseline algorithms: GBBS(both parallel and sequential)
# iSpan, Multi-Step and Tarjan's SCC algorithm
# The directed output of executing the code will be output to folder '../log/exp1'
from graphs import dir_graphs
from graphs import GRAPH_DIR
import subprocess
import os
import multiprocessing

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
os.makedirs(f"{CURRENT_DIR}/../log", exist_ok=True)  
os.makedirs(f"{CURRENT_DIR}/../log/exp1", exist_ok=True)  

global par_rounds
global seq_rounds
# Parallel Running Time
def run_parallel():
    print("Testing Our Parallel SCC Running Time")
    subprocess.call(f'cd {CURRENT_DIR}/../src && make scc -B -j 8', shell=True)
    # For each experiment, run 11 times and take the average of the last 10 time
    scc = f'{CURRENT_DIR}/../src/scc'
    for key, val in dir_graphs.items():
        graph = val[0]
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}.out"
        print(f"Running {key}")
        cmd = f"numactl -i all {scc} {graph_in} -local_reach -local_scc -t {par_rounds} | tee -a {log_out}"
        subprocess.call(cmd, shell=True)

def run_serial():
    print("Testing Our Sequential SCC Running Time")
    subprocess.call(f'cd {CURRENT_DIR}/../src && make scc -B SERIAL=1 -j 8', shell=True)
    # For each experiment, run 11 times and take the average of the last 10 time
    scc = f'{CURRENT_DIR}/../src/scc'
    for key, val in dir_graphs.items():
        graph = val[0]
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}_seq.out"
        print(f"Running {key}")
        cmd = f"{scc} {graph_in} -local_reach -local_scc -t {seq_rounds} | tee -a {log_out}"
        subprocess.call(cmd, shell=True)

def run_Tarjan():
    print("Testing SCC Baselines: Tarjan's Algorithm")
    subprocess.call(f'cd {CURRENT_DIR}/../baselines/tarjan_scc && make SERIAL=1 -B', shell=True)
    scc = f'{CURRENT_DIR}/../baselines/tarjan_scc/tarjan_scc'
    for key, val in dir_graphs.items():
        graph = val[0]
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}_tarjan.out"
        print(f"Running {key}")
        cmd = f"ulimit -s unlimited && {scc} {graph_in} -t {seq_rounds} | tee -a {log_out}"
        subprocess.call(cmd, shell=True)

def run_GBBS():
    print("Testing SCC Baselines: Parallel GBBS")
    GBBS_DIR = f"{CURRENT_DIR}/../baselines/gbbs/benchmarks/StronglyConnectedComponents/RandomGreedyBGSS16"
    subprocess.call(f"cd {GBBS_DIR} && make -B", shell=True)
    scc = f"{GBBS_DIR}/StronglyConnectedComponents"
    for key, val in dir_graphs.items():
        graph=val[0]
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}_gbbs.out"
        cmd= f"numactl -i all {scc} -b -beta 1.5 -rounds {par_rounds} {graph_in} | tee -a {log_out}"
        subprocess.call(cmd, shell=True)
def run_GBBS_serial():
    print("Testing SCC Baselines: Parallel GBBS")
    GBBS_DIR = f"{CURRENT_DIR}/../baselines/gbbs/benchmarks/StronglyConnectedComponents/RandomGreedyBGSS16"
    subprocess.call(f"cd {GBBS_DIR} && make SERIAL=1 -B", shell=True)
    scc = f"{GBBS_DIR}/StronglyConnectedComponents"
    for key, val in dir_graphs.items():
        graph=val[0]
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}_gbbs_seq.out"
        cmd= f"{scc} -b -beta 1.5 -rounds {seq_rounds} {graph_in} | tee -a {log_out}"
        subprocess.call(cmd, shell=True)

def run_MultiStep():
    print("Testing SCC Baselines: Parallal Multi-Step")
    MultiStep_DIR = f"{CURRENT_DIR}/../baselines/MultiStep/multistep"
    subprocess.call(f"cd {MultiStep_DIR} && make -B", shell=True)
    scc = f"{MultiStep_DIR}/scc"
    skip_graphs={"CW", "HL14", "HL12"}
    for key, val in dir_graphs.items():
        if (key in skip_graphs):
            continue
        graph=val[0]
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}_multistep.out"
        cmd= f"numactl -i all {scc} {graph_in} -t {par_rounds} | tee -a {log_out}"
        subprocess.call(cmd, shell=True)
def run_iSpan():
    print("Testing SCC Baselines: Parallel iSpan")
    iSpan_DIR = f"{CURRENT_DIR}/../baselines/iSpan/src"
    subprocess.call(f"cd {iSpan_DIR} && make -B", shell=True)
    scc = f"{iSpan_DIR}/ispan"
    skip_graphs = {"TW", "CW", "HL14","HL12", "GL2", "GL5", "COS5"}
    thread_count = (multiprocessing.Pool())._processes
    alpha=30
    beta=200
    gamma=10
    theta=0.10
    for key, val in dir_graphs.items():
        if (key in skip_graphs):
            continue
        graph = val[0]
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}_ispan.out"
        cmd = f"numactl -i all {scc} {graph_in} {thread_count} {alpha} {beta} {gamma} {theta} {par_rounds} | tee -a {log_out}"
        subprocess.call(cmd, shell=True)


if __name__ == '__main__':
    global par_rounds, seq_rounds
    par_rounds = 1
    seq_rounds = 1
    run_parallel()
    run_GBBS()
    run_MultiStep()
    run_iSpan()
    run_Tarjan()
    run_serial()
    run_GBBS_serial()