# This script will run our SCC algorithms on the directed. 
# Test our SCC running time under 96 cores 192 hyperthreads 
# and also the sequential running time with only one core.
# The script also test the baseline algorithms: GBBS(both parallel and sequential)
# iSpan, Multi-Step and Tarjan's SCC algorithm
# The directed output of executing the code will be output to folder '../log/exp1'
from graphs import dir_graphs
from graphs import GRAPH_DIR
from algorithms import Algorithms
import subprocess
import os
import multiprocessing

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
os.makedirs(f"{CURRENT_DIR}/../log", exist_ok=True)  
os.makedirs(f"{CURRENT_DIR}/../log/exp1", exist_ok=True)  

global par_rounds
global seq_rounds
# Parallel Running Time
def Our_scc():
    print("Testing Our Parallel SCC Running Time")
    for key, val in dir_graphs.items():
        graph = val[0]
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}.out"
        print(f"Running on {key}")
        dlong = "-long" if (key=="HL12") else ""
        cmd = f'''numactl -i all {Algorithms["Ours_scc"]} {graph_in} {dlong} -local_reach -local_scc -t {par_rounds} >> {log_out}'''
        subprocess.call(cmd, shell=True)

def Our_scc_serial():
    print("Testing Our Sequential SCC Running Time")
    for key, val in dir_graphs.items():
        graph = val[0]
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}_seq.out"
        print(f"Running on {key}")
        dlong = "-long" if (key=="HL12") else ""
        cmd = f'''{Algorithms["Ours_scc_serial"]} {graph_in} {dlong} -local_reach -local_scc -t {seq_rounds} >> {log_out}'''
        subprocess.call(cmd, shell=True)

def Tarjan_scc():
    print("Testing SCC Baselines: Tarjan's Algorithm")
    for key, val in dir_graphs.items():
        graph = val[0]
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}_tarjan.out"
        print(f"Running on {key}")
        dlong = "-long" if (key=="HL12") else ""
        cmd = f'''ulimit -s unlimited && {Algorithms["Tarjan"]} {graph_in} {dlong} -t {seq_rounds} >> {log_out}'''
        subprocess.call(cmd, shell=True)

def GBBS_scc():
    print("Testing SCC Baselines: Parallel GBBS")
    for key, val in dir_graphs.items():
        graph=val[0]
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}_gbbs.out"
        print(f"Running on {key}")
        dlong = "-long" if (key=="HL12") else ""
        cmd= f'''numactl -i all {Algorithms["GBBS_scc"]} {dlong} -b -beta 1.5 -rounds {par_rounds} {graph_in} >> {log_out}'''
        subprocess.call(cmd, shell=True)
def GBBS_scc_serial():
    print("Testing SCC Baselines: Parallel GBBS")
    for key, val in dir_graphs.items():
        graph=val[0]
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}_gbbs_seq.out"
        print(f"Running on {key}")
        dlong = "-long" if (key=="HL12") else ""
        cmd= f'''{Algorithms["GBBS_scc_serial"]} {dlong} -b -beta 1.5 -rounds {seq_rounds} {graph_in} >> {log_out}'''
        subprocess.call(cmd, shell=True)

def MultiStep():
    print("Testing SCC Baselines: Parallal Multi-Step")
    skip_graphs={"CW", "HL14", "HL12"}
    for key, val in dir_graphs.items():
        if (key in skip_graphs):
            continue
        graph=val[0]
        graph_in = f"{GRAPH_DIR}/{graph}.bin"
        log_out = f"{CURRENT_DIR}/../log/exp1/{key}_multistep.out"
        print(f"Running on {key}")
        cmd= f'''numactl -i all {Algorithms["MultiStep"]} {graph_in} -t {par_rounds} >> {log_out}'''
        subprocess.call(cmd, shell=True)
def iSpan():
    print("Testing SCC Baselines: Parallel iSpan")
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
        print(f"Running on {key}")
        cmd = f'''numactl -i all {Algorithms["iSpan"]} {graph_in} {thread_count} {alpha} {beta} {gamma} {theta} {par_rounds} >> {log_out}'''
        subprocess.call(cmd, shell=True)


if __name__ == '__main__':
    global par_rounds, seq_rounds
    par_rounds = 1
    seq_rounds = 1
    # Our_scc()
    # Our_scc_serial()
    # GBBS_scc()
    # GBBS_scc_serial()
    # MultiStep()
    iSpan()
    # Tarjan_scc()