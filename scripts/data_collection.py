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

def collect_exp1(RESULT_DIR):
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
    pd.set_option('max_columns', None)
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
    fig1_file = f"{RESULT_DIR}/Fig1.csv"
    print(f"Figure1 Heatmap tabel is stored to {fig1_file}")
    heatdf.to_csv(fig1_file)
    
def collect_exp5(RESULT_DIR):
    data = dict()
    algs = {"Our Connectivity": ("_cc", "average_time:"),"DHS21": ("_connectit", "### t ="), "CC:Spd.": (None,None), "Our LE-List": ("_lelist", "average cost:"),"parlaylib": ("_lelist_parlay", "Time: le_list:"), "LE-List:Spd.":(None,None)}
    for a, val in algs.items():
        data[a]=[]
        (surfix, key_words)=val
        if (surfix==None):
            continue
        for graph in dir_graphs.keys():
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
    df = df.set_axis(dir_graphs.keys())
    pd.set_option('max_columns', None)
    table4_file = f"{RESULT_DIR}/Table4.csv"
    print(f"Table 4 is stored to {table4_file}")
    df.to_csv(table4_file)

collect_exp1(f"{CURRENT_DIR}/../result")
collect_exp5(f"{CURRENT_DIR}/../result")