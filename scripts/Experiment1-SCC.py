# This script will run our SCC algorithms on the graphs in dir_graphs. 
# Test our SCC running time under 96 cores 192 hyperthreads 
# and also the sequential running time with only one core.
# The directed output of executing the code will be output to folder '../log/exp1'
from graphs import dir_graphs
from graphs import GRAPH_DIR
import subprocess
import os

DIR_GRAPH_DIR=f"{GRAPH_DIR}"
# DIR_GRAPH_DIR=f"{GRAPH_DIR}/dir"
CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
if not os.path.exists(f"{CURRENT_DIR}/../log"):
    subprocess.call(f"mkdir {CURRENT_DIR}/../log", shell=True)
if not os.path.exists(f"{CURRENT_DIR}/../log/exp1"):
    subprocess.call(f'mkdir {CURRENT_DIR}/../log/exp1', shell=True)
par_rounds = 10
seq_rounds = 1
# Parallel Running Time
def run_parallel():
    print("Testing Our Parallel SCC Running Time")
    subprocess.call(f'cd {CURRENT_DIR}/../src && make scc -B -j', shell=True)
    # For each experiment, run 11 times and take the average of the last 10 time
    for key, val in dir_graphs.items():
        graph = val[0]
        scc = f'{CURRENT_DIR}/../src/scc'
        graph_in = f"{DIR_GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}.out"
        print(f"Running {key}")
        cmd = f"numactl -i all {scc} {graph_in} -local_reach -local_scc -t {par_rounds} | tee -a {log_out}"
        subprocess.call(cmd, shell=True)

def run_serial():
    print("Testing Our Sequential SCC Running Time")
    subprocess.call(f'cd {CURRENT_DIR}/../src && make scc -B SERIAL=1 -j', shell=True)
    # For each experiment, run 11 times and take the average of the last 10 time
    for key, val in dir_graphs.items():
        graph = val[0]
        scc = f'{CURRENT_DIR}/../src/scc'
        graph_in = f"{DIR_GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}_seq.out"
        print(f"Running {key}")
        cmd = f"{scc} {graph_in} -local_reach -local_scc -t {seq_rounds} | tee -a {log_out}"
        subprocess.call(cmd, shell=True)

def run_Tarjan():
    print("Testing SCC Baselines: Tarjan's Algorithm")
    subprocess.call(f'cd {CURRENT_DIR}/../baselines/tarjan_scc && make SERIAL=1 -B', shell=True)
    for key, val in dir_graphs.items():
        graph = val[0]
        scc = f'{CURRENT_DIR}/../baselines/tarjan_scc/tarjan_scc'
        graph_in = f"{DIR_GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}_tarjan.out"
        print(f"Running {key}")
        cmd = f"{scc} {graph_in} -t {seq_rounds} | tee -a {log_out}"
        subprocess.call(cmd, shell=True)

def run_GBBS():
    print("Testing SCC Baselines: Parallel GBBS")
    GBBS_DIR = f"{CURRENT_DIR}/../baselines/gbbs/benchmarks/StronglyConnectedComponents/RandomGreedyBGSS16"
    subprocess.call(f"cd {GBBS_DIR} && make -B", shell=True)
    scc = f"{GBBS_DIR}/StronglyConnectedComponents"
    for key, val in dir_graphs.items():
        graph=val[0]
        graph_in = f"{DIR_GRAPH_DIR}/{graph}.bin"
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
        graph_in = f"{DIR_GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}_gbbs_seq.out"
        cmd= f"{scc} -b -beta 1.5 -rounds {seq_rounds} {graph_in} | tee -a {log_out}"
        subprocess.call(cmd, shell=True)

def run_MultiStep():
    print("Testing SCC Baselines: Parallal Multi-Step")
    MultiStep_DIR = f"{CURRENT_DIR}/../baselines/MultiStep/multistep"
    subprocess.call(f"cd {MultiStep_DIR} && make -B", shell=True)
    scc = f"{MultiStep_DIR}/scc"
    for key, val in dir_graphs.items():
        graph=val[0]
        graph_in = f"{DIR_GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}_multistep.out"
        cmd= f"numactl -i all {scc} {graph_in} -t {par_rounds} | tee -a {log_out}"
        subprocess.call(cmd, shell=True)
# run_parallel()
# run_serial()
# run_Tarjan()
# run_GBBS_serial()
run_MultiStep()