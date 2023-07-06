# This script will run our SCC algorithms on the graphs in dir_graphs. 
# Test our SCC running time under 96 cores 192 hyperthreads 
# and also the sequential running time with only one core.
# The directed output of executing the code will be output to folder '../log/exp1'
from graphs import dir_graphs
import subprocess
import os

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
subprocess.call(f'mkdir {CURRENT_DIR}/../log/exp1', shell=True)
# Parallel Running Time
def run_parallel():
    print("Testing Our Parallel SCC Running Time")
    subprocess.call(f'cd {CURRENT_DIR}/../src && make scc -B -j', shell=True)
    # For each experiment, run 11 times and take the average of the last 10 time
    for key, val in dir_graphs.items():
        graph = val[0]
        scc = f'{CURRENT_DIR}/../src/scc'
        graph_in = f"{CURRENT_DIR}/../data/dir/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}.out"
        print(f"Running {key}")
        cmd = f"numactl -i all {scc} {graph_in} -local_reach -local_scc > {log_out}"
        subprocess.call(cmd, shell=True)

def run_serial():
    print("Testing Our Sequential SCC Running Time")
    subprocess.call(f'cd {CURRENT_DIR}/../src && make scc -B SERIAL=1 -j', shell=True)
    # For each experiment, run 11 times and take the average of the last 10 time
    for key, val in dir_graphs.items():
        graph = val[0]
        scc = f'{CURRENT_DIR}/../src/scc'
        graph_in = f"{CURRENT_DIR}/../data/dir/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}_seq.out"
        print(f"Running {key}")
        cmd = f"{scc} {graph_in} -local_reach -local_scc > {log_out}"
        subprocess.call(cmd, shell=True)

def run_Tarjan():
    print("Testing SCC Baselines: Tarjan's Algorithm")
    subprocess.call(f'cd {CURRENT_DIR}/../baselines/tarjan_scc && make SERIAL=1 -B', shell=True)
    for key, val in dir_graphs.items():
        graph = val[0]
        scc = f'{CURRENT_DIR}/../baselines/tarjan_scc/tarjan_scc'
        graph_in = f"{CURRENT_DIR}/../data/dir/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}_tarjan.out"
        print(f"Running {key}")
        cmd = f"{scc} {graph_in} > {log_out}"
        subprocess.call(cmd, shell=True)

def run_GBBS():
    print("Testing SCC Baselines: Parallel GBBS")
    alg = "//benchmarks/StronglyConnectedComponents/RandomGreedyBGSS16:StronglyConnectedComponents_main"
    for key, val in dir_graphs.items():
        graph=val[0]
        graph_in = f"{CURRENT_DIR}/../data/dir{graph}.bin"
        # cmd= f"$cd {CURRENT_DIR}/../baselines/gbbs && numactl -i all bazel run ${alg} -- -b -beta 1.5 ${graph_in}"
        cmd= f"$cd {CURRENT_DIR}/../baselines/gbbs && bazel run ${alg} -- -b -beta 1.5 ${graph_in}"
        subprocess.call(cmd, shell=True)

# run_parallel()
# run_serial()
# run_Tarjan()
run_GBBS()