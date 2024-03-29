import os
import sys
import subprocess
import re
from graphs import dir_graphs
from graphs import sym_graphs
import numpy as np
import pandas as pd

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
LOG_DIR = f"{CURRENT_DIR}/../log"
RESULT_DIR = f"{CURRENT_DIR}/../result"

def geo_mean(iterable):
    a = np.array(iterable)
    return a.prod()**(1.0/len(a))

def collect_data(file_in, key_words):
    f = open(file_in,'r')
    res = f.read()
    f.close()
    data_lines = re.findall(f"{key_words}.*", res)
    data = list(map(lambda x: eval(x.split(" ")[-1]), data_lines))
    return data

def collect_exp1():
    data = dict()
    algs={"Ours.par": "","Ours.seq": "_seq","Ours.spd":None,"GBBS.par": "_gbbs","GBBS.seq": "_gbbs_seq","GBBS.spd":None,"iSpan": "_ispan","MultiStep": "_multistep","SEQ": "_tarjan",
    }
    for a, surfix in algs.items():
        data[a]=[]
        if (surfix==None):
            continue
        key_words = "average cost"
        if (a in ["GBBS.par", "GBBS.seq"]):
            key_words = "# time per iter:"
        for graph in dir_graphs.keys():
            try: 
                data_g = collect_data(f"{LOG_DIR}/exp1/{graph}{surfix}.out", key_words)
                data[a].append(data_g[0])
            except:
                data[a].append(np.nan)
        data[a]=np.array(data[a])
    data["Ours.spd"] = data["Ours.seq"]/data['Ours.par']
    data["GBBS.spd"] = data["GBBS.seq"]/data['GBBS.par']
    BestBaseline= np.nanmin(np.array([data['GBBS.par'],data["iSpan"], data["MultiStep"], data["SEQ"]]), axis=0)
    data["Best/Ours"] = BestBaseline/data["Ours.par"]
    df = pd.DataFrame.from_dict(data)
    df = df.set_axis(dir_graphs.keys())
    # pd.set_option('max_columns', None)
    table3_file = f"{RESULT_DIR}/Table3.csv"
    print(f"Table 3 is stored to {table3_file}")
    df.to_csv(table3_file)
    relative_data = {"Ours": data["SEQ"]/data["Ours.par"],
                    "GBBS": data["SEQ"]/data["GBBS.par"],
                    "iSpan": data["SEQ"]/data['iSpan'],
                    'MultiStep':data["SEQ"]/data['MultiStep'],
                    'SEQ':data["SEQ"]/data["SEQ"]}
    heatdf=pd.DataFrame.from_dict(relative_data)
    heatdf = heatdf.set_axis(dir_graphs.keys()).T
    heatdf["OverallMean"] = heatdf[dir_graphs.keys()].apply(lambda row: geo_mean(row[~row.isna()]), axis=1)
    fig1_file = f"{RESULT_DIR}/Figure1.csv"
    print(f"Figure1 Heatmap tabel is stored to {fig1_file}")
    heatdf.to_csv(fig1_file)
    
def collect_exp5():
    data = dict()
    algs = {"Our Connectivity": ("_cc", "average_time:"),"DHS21": ("_connectit", "### t ="), "CC:Spd.": (None,None), "Our LE-List": ("_lelist", "average cost:"),"parlaylib": ("_lelist_parlay", "Time: le_list:"), "LE-List:Spd.":(None,None)}
    for a, val in algs.items():
        data[a]=[]
        (surfix, key_words)=val
        if (surfix==None):
            continue
        for graph in sym_graphs.keys():
            try: 
                data_g = collect_data(f"{LOG_DIR}/exp5/{graph}{surfix}.out", key_words)
                if (len(data_g)==1):
                    data[a].append(data_g[0])
                elif (len(data_g) >1):
                    data[a].append(np.mean(data_g[1:]))
                else:
                    data[a].append(np.nan)
            except:
                data[a].append(np.nan)
        data[a]=np.array(data[a])
    data["CC:Spd."] = data["DHS21"]/data['Our Connectivity']
    data["LE-List:Spd."] = data["parlaylib"]/data['Our LE-List']
    df = pd.DataFrame.from_dict(data)
    df = df.set_axis(sym_graphs.keys())
    table4_file = f"{RESULT_DIR}/Table4.csv"
    print(f"Table 4 is stored to {table4_file}")
    df.to_csv(table4_file)
def collect_exp2():
    graphs = ["TW", "SD", "CW", "SQR_s", "GL5", "COS5"]
    data = dict()
    threads = [192, 96, 48,24,12,8,4,2,1]
    file_out = f"{RESULT_DIR}/Figure7.csv"
    algorithms = {"Ours":"scc", "GBBS": "gbbs", "iSpan": "ispan", "MultiStep":"multistep"}
    for g in graphs:
        data[g] = dict()
        for a, surfix in algorithms.items():
            key_words = "average cost" if (a != "GBBS") else "# time per iter"
            try:
                data[g][a]=collect_data(f"{LOG_DIR}/exp2/{g}_scale_{surfix}.out", key_words)
            except:
                data[g][a]=[float('nan')]*len(threads)
    print(f"SCC scalability information is stored to {file_out}")
    with open(file_out, 'a') as f:
        for g in graphs:
            f.write(f"graph {g}\n")
            df = pd.DataFrame.from_dict(data[g])
            df = df.set_axis(threads)
            df.to_csv(f)
            f.write("\n")
    data_self=dict()
    for g in graphs:
        data_self[g]= data[g]["Ours"][-1]/np.array(data[g]["Ours"])
    file_out = f"{RESULT_DIR}/Figure8.csv"
    print(f"SCC self speedup is stored to {file_out}")
    df = pd.DataFrame.from_dict(data_self)
    df = df.set_axis(threads)
    df.to_csv(file_out)
def collect_exp3():
    graphs = ["LJ", "TW", "SD", "CW", "HL12", "CH5", "GL2", "GL10", "GL20", "COS5", "SQR", "SQR_s"]
    file_out = f"{RESULT_DIR}/Figure9.csv"
    data=dict()
    key_words = ["init_time", "first_round_time", "multi_search time", "hash table resize time"]
    for g in graphs:
        if (g not in dir_graphs.keys()):
            continue
        data[g]=dict()
        data[g]["GBBS"] =[]
        data[g]["Plain"]=[]
        data[g]["VGC1"]=[]
        data[g]["Final"]=[]
        GBBS_file = f"{LOG_DIR}/exp3/{g}_break_gbbs.out"
        our_file = f"{LOG_DIR}/exp3/{g}_break.out"
        try:
            gbbs_total = collect_data(GBBS_file, "# time per iter")
            our_totals = collect_data(our_file, "average cost")
            for key in key_words:
                gbbs_data = collect_data(GBBS_file, key)
                if (len(gbbs_data) == 0):
                    print(g, key)
                data[g]["GBBS"].append(np.mean(gbbs_data[1:]))
                our_data = collect_data(our_file, key)
                data[g]["Plain"].append(our_data[0])
                data[g]["VGC1"].append(our_data[1])
                data[g]["Final"].append(our_data[2])
            data[g]["GBBS"].append(gbbs_total[0]-np.sum(data[g]["GBBS"]))
            data[g]["Plain"].append(our_totals[0]-np.sum(data[g]["Plain"]))
            data[g]["VGC1"].append(our_totals[1]-np.sum(data[g]["VGC1"]))
            data[g]["Final"].append(our_totals[2]-np.sum(data[g]["Final"]))
        except:
            for key in data[g].keys():
                data[g][key]=[float('nan')]*(len(key_words)+1)
    print(f"Breakdown information is writen to {file_out}")
    with open(file_out, 'a') as f:
        for g in graphs:
            if (g not in dir_graphs.keys()):
                continue
            f.write(f"graph {g}\n")
            df = pd.DataFrame.from_dict(data[g])
            df = df.set_axis(["Trimming", "First SCC", "Multi-search","Hash Table Resizing", "Others"])
            df.to_csv(f)
            f.write("\n")
def collect_exp4():
    threads=[192, 96, 48, 24, 4]
    tau = [f"2^{i}" for i in range(18)]
    data = dict()
    file_out = f"{RESULT_DIR}/Figure11.csv"
    graphs = ["TW", "SD", "CW", "GL5", "COS5","SQR_s"]
    for g in graphs:
        if g not in dir_graphs.keys():
            continue
        data[g]=dict()
        try:
            for thread in threads:
                data[g][thread]=collect_data(f"{LOG_DIR}/exp4/{g}_{thread}.out", "average cost")
        except:
            for thread in threads:
                data[g][thread]=[float('nan')]*len(tau)
    print(f"Parameter tau information is writen to {file_out}")
    with open(file_out, 'a') as f:
        for g in data.keys():
            f.write(f"graph {g}\n")
            df = pd.DataFrame.from_dict(data[g])
            df=df.set_axis(tau)
            df.to_csv(f)
            f.write("\n")
def collect_exp6():
    # graphs_rounds = ["LJ", "TW", "SD", "CW","HL12","CH5", "GL2", "GL10", "GL20", "COS5", "SQR", "SQR_s"]
    data=dict()
    file_out = f"{RESULT_DIR}/Figure10.csv"
    for g in dir_graphs.keys():
        data[g]=dict()
        try:
            data_ = collect_data(f"{LOG_DIR}/exp6/{g}_round.out", "depth")
            mid = int(len(data_)/2)
            data[g]["Plain"] = data_[:mid]
            data[g]["VGC"]=data_[mid:]
        except:
            data[g]["Plain"]=[float('nan')]
            data[g]["VGC"]=[float('nan')]
    print(f"Write number of rounds with/without VGC to {file_out}")
    with open(file_out, 'a') as f:
        for g in data.keys():
            f.write(f"graph {g}\n")
            df = pd.DataFrame.from_dict(data[g])
            df.to_csv(f)
            f.write("\n")
    # for generate Table 2
    graph_info=dict()
    for g in dir_graphs.keys():
        file_in = f"{LOG_DIR}/exp6/{g}_round.out"
        graph_info[g]=[]
        f = open(file_in,'r')
        res = f.read()
        f.close()
        data_lines = re.findall(f"number of vertices.*", res)
        data_v = list(map(lambda x: eval(x.replace(',', '').split(" ")[3]), data_lines))
        graph_info[g].append(data_v[0])
        graph_info[g].append(collect_data(file_in, "number of vertices")[0])
        graph_info[g].append(collect_data(file_in, "Largest StronglyConnectedComponents")[0])
        graph_info[g].append(graph_info[g][2]/graph_info[g][0])
        graph_info[g].append(collect_data(file_in, "n_scc =")[0])
    df = pd.DataFrame.from_dict(graph_info)
    df=df.set_axis(["n", "m", "|SCC1|", "|SCC1|/n", "#SCC"])
    df=df.T
    file_out=f"{RESULT_DIR}/Table2.csv"
    print(f"Graph information is stored to {file_out}")
    df.to_csv(file_out)
        
if __name__ == "__main__":
    collect_exp1()
    collect_exp2()
    collect_exp3()
    collect_exp4()
    collect_exp5()
    collect_exp6()