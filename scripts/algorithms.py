import subprocess
import os
CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))

ALG_DIRS={
    "OURS": f"{CURRENT_DIR}/../src",
    "GBBS": f"{CURRENT_DIR}/../baselines/gbbs",
    "Tarjan": f"{CURRENT_DIR}/../baselines/tarjan_scc",
    "MultiStep": f"{CURRENT_DIR}/../baselines/MultiStep/multistep",
    "iSpan": f"{CURRENT_DIR}/../baselines/iSpan/src",
    "parlay": f"{CURRENT_DIR}/../parlaylib/build"
}
Algorithms={
    "Ours_scc": f"{ALG_DIRS['OURS']}/scc",
    "Ours_scc_serial": f"{ALG_DIRS['OURS']}/scc_serial",
    "Ours_scc_round": f"{ALG_DIRS['OURS']}/scc_round",
    "Ours_cc": f"{ALG_DIRS['OURS']}/Connectivity",
    "Ours_lelist": f"{ALG_DIRS['OURS']}/LeList",
    "GBBS_scc": f"{ALG_DIRS['GBBS']}/bazel-bin/benchmarks/StronglyConnectedComponents/RandomGreedyBGSS16/StronglyConnectedComponents_main",
    "GBBS_scc_serial": f"{ALG_DIRS['GBBS']}/bazel-bin/benchmarks/StronglyConnectedComponents/RandomGreedyBGSS16/StronglyConnectedComponents_serial_main",
    "GBBS_cc": f"{ALG_DIRS['GBBS']}/bazel-bin/benchmarks/Connectivity/ConnectIt/mains/unite_rem_cas_ldd",
    "Tarjan": f"{ALG_DIRS['Tarjan']}/tarjan_scc",
    "MultiStep": f"{ALG_DIRS['MultiStep']}/scc",
    "iSpan": f"{ALG_DIRS['iSpan']}/ispan",
    "parlay_lelist": f"{ALG_DIRS['parlay']}/examples/le_list"
}

def compile_algorithms():
    print("compile our algorithms")
    subprocess.call(f'''cd {ALG_DIRS["OURS"]} && make''', shell=True)
    print("compile Tarjan's algorithm")
    subprocess.call(f'''cd {ALG_DIRS["Tarjan"]} && make SERIAL=1''', shell=True)
    print("compile GBBS's algorithms")
    subprocess.call(f'''cd {ALG_DIRS["GBBS"]} && bazel build --config=serial //benchmarks/StronglyConnectedComponents/RandomGreedyBGSS16:StronglyConnectedComponents_serial_main''', shell=True)
    subprocess.call(f'''cd {ALG_DIRS["GBBS"]} && bazel build //benchmarks/StronglyConnectedComponents/RandomGreedyBGSS16:StronglyConnectedComponents_main''', shell=True)
    subprocess.call(f'''cd {ALG_DIRS["GBBS"]} && bazel build //benchmarks/Connectivity/...''', shell=True)
    print("compile MultiStep SCC algorithm")
    subprocess.call(f'''cd {ALG_DIRS["MultiStep"]} && make''', shell=True)
    print("compile iSpan's SCC algorithm")
    subprocess.call(f'''cd {ALG_DIRS["iSpan"]} && make''', shell=True)
    print("compile parlaylib's LE-List algorithm")
    os.makedirs(ALG_DIRS["parlay"], exist_ok=True)
    cmd = f'''cd {ALG_DIRS["parlay"]} && cmake .. -DPARLAY_EXAMPLES=On && cmake --build . --target le_list'''
    subprocess.call(cmd, shell = True)

if __name__ == "__main__":
    compile_algorithms()