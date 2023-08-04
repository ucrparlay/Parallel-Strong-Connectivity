from graphs import dir_graphs
from graphs import GRAPH_DIR
import subprocess
import os
import multiprocessing

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
os.makedirs(f"{CURRENT_DIR}/../log", exist_ok=True)  
os.makedirs(f"{CURRENT_DIR}/../log/exp2", exist_ok=True) 

global par_rounds
global seq_rounds
# Parallel Running Time
def run_SCC_scale(graph_key, file_out, n_thread, cores):
    print("Testing Our Parallel SCC Running Time")
    scc = f'{CURRENT_DIR}/../src/scc'
    numa = "" if (n_thread==1) else "numactl -i all"
    print(f"Running on graph {graph_key}")
    file_in = f"{GRAPH_DIR}/{dir_graphs[graph_key][0]}.bin"
    cmd = f"PARLAY_NUM_THREADS={n_thread} taskset -c {cores} {numa} {scc} {file_in} -local_reach -local_scc -t {par_rounds} | tee -a {file_out}"
    subprocess.call(cmd, shell=True)
def run_GBBS_scale(graph_key, file_out, n_thread, cores):
    print("Testing SCC Baselines: Parallel GBBS")
    GBBS_DIR = f"{CURRENT_DIR}/../baselines/gbbs"
    scc = f"{GBBS_DIR}/bazel-bin/benchmarks/StronglyConnectedComponents/RandomGreedyBGSS16/StronglyConnectedComponents_main"
    file_in = f"{GRAPH_DIR}/{dir_graphs[graph_key][0]}.bin"
    numa = "" if (n_thread==1) else "numactl -i all"
    cmd= f"PARLAY_NUM_THREADS={n_thread} taskset -c {cores} {numa} {numa} {scc} -b -beta 1.5 -rounds {par_rounds} {file_in} | tee -a {file_out}"
    subprocess.call(cmd, shell=True)
def run_iSpan_scale(graph_key, file_out, n_thread, cores):
    iSpan_DIR = f"{CURRENT_DIR}/../baselines/iSpan/src"
    scc = f"{iSpan_DIR}/ispan"
    skip_graphs = {"TW", "CW", "HL14","HL12", "GL2", "GL5", "COS5"}
    if (graph_key in skip_graphs):
        raise Exception("iSpan does not support the graph")
    thread_count = (multiprocessing.Pool())._processes
    alpha=30
    beta=200
    gamma=10
    theta=0.10
    file_in = f"{GRAPH_DIR}/{dir_graphs[graph_key][0]}.bin"
    numa = "" if (n_thread==1) else "numactl -i all"
    cmd = f"OMP_NUM_THREADS={n_thread} taskset -c {cores} {numa} {scc} {file_in} {thread_count} {alpha} {beta} {gamma} {theta} {par_rounds} | tee -a {file_out}"
    subprocess.call(cmd, shell=True)

def run_MultiStep_scale(graph_key, file_out, n_thread, cores):
    print("Testing SCC Baselines: Parallal Multi-Step")
    MultiStep_DIR = f"{CURRENT_DIR}/../baselines/MultiStep/multistep"
    scc = f"{MultiStep_DIR}/scc"
    skip_graphs={"CW", "HL14", "HL12"}
    if (graph_key in skip_graphs):
        raise Exception("MultiStep does not support the graph")
    file_in = f"{GRAPH_DIR}/{dir_graphs[graph_key][0]}.bin"
    numa = "" if (n_thread==1) else "numactl -i all"
    cmd= f"OMP_NUM_THREADS={n_thread} taskset -c {cores} {numa} {scc} {file_in} -t {par_rounds} | tee -a {file_out}"
    subprocess.call(cmd, shell=True)

def run_scale(graph):
    threads={192: '0-191',96: '0-95',48: '0-95:2',24: '0-95:4',12: '0-47:4',8: '0-31:4',4: '0-15:4',2: '0-7:4',1: '0-3:4'}
    algorithms = {run_SCC_scale:"scc", run_GBBS_scale:"gbbs", run_iSpan_scale:"ispan", run_MultiStep_scale:"multistep"}
    for a, surfix in algorithms.items():
        file_out = f"{CURRENT_DIR}/../log/exp2/{graph}_scale_{surfix}.out"
        for n_thread, cores in threads.items():
            print(f"thread: {n_thread} cores: {cores}")
            subprocess.call(f"echo 'threads: {n_thread} cores: {cores}' >> {file_out}" , shell=True)
            try:
                a(graph, file_out, n_thread, cores)
            except:
                key_words = "# time per iter: float('nan')" if (a == run_GBBS_scale) else "average cost: float('nan')"
                subprocess.call(f'''echo "{key_words}" >>  {file_out}''', shell=True)

if __name__ == '__main__':
    global par_rounds, seq_rounds
    par_rounds = 1
    print("compile algorithms")
    subprocess.call(f'cd {CURRENT_DIR}/../src && make scc -B -j 8', shell=True)
    subprocess.call(f"cd {CURRENT_DIR}/../baselines/gbbs && bazel build //benchmarks/StronglyConnectedComponents/RandomGreedyBGSS16/...", shell=True)
    subprocess.call(f"cd {CURRENT_DIR}/../baselines/iSpan/src && make -B", shell=True)
    subprocess.call(f"cd {CURRENT_DIR}/../baselines/MultiStep/multistep && make -B", shell=True)
    graphs = ["TW", "SD", "CW", "SQR_s", "GL5", "COS5"]
    for g in graphs:
        run_scale(g)