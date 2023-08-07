import numpy as np # linear algebra
import pandas as pd # data processing, CSV file I/O (e.g. pd.read_csv)
import matplotlib.pyplot as plt    # used for drawing figures
import numpy as np                 # for linear algebra operations
import seaborn as sns              # the others are for format details
import pandas as pd
import os

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
RESULT_DIR = f"{CURRENT_DIR}/../result"
FIGURE_DIR = f"{CURRENT_DIR}/../figures"
os.makedirs(FIGURE_DIR, exist_ok=True)  
def read_multiple_dataframes(file):
    data = dict()
    f = open(file, 'r')
    lines = f.readlines()
    start = 0
    end = 0
    for i in range(len(lines)):
        if lines[i].startswith("graph"):
            start = i
        elif lines[i]=="\n":
            end = i
            g = lines[start].replace("\n","").split(" ")[-1]
            start +=1
            end-=1
            data[g]=pd.read_csv(file,  skiprows=start,nrows= end-start, header=0, index_col=0)
    return data

def Figure7(data, Tarjan_data):
    graph_avail = list(data.keys())
    graphs = ["TW", "SD", "CW", "SQR_s", "GL5","COS5"]
    fig, ax = plt.subplots(2,3, figsize=(9,3), sharex=True)
    plt.subplots_adjust(left=None, bottom=None, right=None, top=None, wspace=0.16, hspace=0.25)
    fig.add_subplot(111, frameon=False)
    ax = ax.flatten()
    # x data, which is the number of cores in this example
    i = 0
    x_major = [1,2,4,8,24,96]
    major_labels = ['1','2','4','8','24','96']
    x_minor = [12,48, 150]
    minor_labels=['12','48','96h']
    algorithms = list(data[graph_avail[0]].keys())
    for g in graphs:
        if g in graph_avail:
            y_min = 1e5
            for a in algorithms:
                # note not all algorithms can run on all graphs
                try:
                    ax[i].plot(Tarjan_data[g]/data[g][a], markersize = 8, label=a,  alpha=1)
                    y_min = min(min(Tarjan_data[g]/data[g][a]), y_min)
                except:
                    continue
        else:
            y_min=0
        # add horizontal reference line, show its ticks in red
        ax[i].hlines(y=1, xmin = 0, xmax=155, linestyle=':', color='r')
        ax[i].text(0.55,0.98, "{:.0f}".format(1),color='red', ha = 'left', va = 'center')
        # reduce the distance between title to the plot
        ax[i].set_title(g, fontsize=14, y=0.95)
        # set x/y-axis to log2-scaled
        ax[i].set_xscale('log', base = 2)
        ax[i].set_yscale('log', base = 2)
        # customize x-ticks position, label, font, rotation
        ax[i].set_xticks(ticks=x_major, labels=major_labels, fontsize=14, rotation = 60)
        ax[i].set_xticks(ticks=x_minor, labels=minor_labels, minor=True, fontsize=14, rotation=60)
        # ax[i].set_xticks(ticks=x_major)
        # ax[i].set_xticks(ticks=x_minor, minor=True)
        # reduce the margin between y-ticks and y-axis
        ax[i].tick_params(axis = 'y', pad = 0.1)
        # set the minimum y value can be shown
        if (y_min < 0.01):
            ax[i].set_ylim(bottom=0.01)
        # show grid background and mark major and minor with different style
        ax[i].grid(which='major', color='#CCCCCC', linewidth=1.1)
        # ax[i].grid(which='minor', color='#CCCCCC', linestyle=':')
        i+=1
    # show the legend
    ax[1].legend(loc='lower center',bbox_to_anchor=(0.5,1.1), ncol=len(algorithms), fontsize=14) 
    # mute the ticks on the new added axes
    plt.tick_params(labelcolor='none', which='both', top=False, bottom=False, left=False, right=False)
    # add common x/y-label, labelpad adjust the margin between label to the figure
    plt.xlabel("number of processors", fontsize=14, labelpad=15)
    plt.ylabel("running time/Tarjan's", fontsize=14, labelpad=10)
    # ------------------New added -------------------------------
    # save figure
    plt.savefig(f"{FIGURE_DIR}/Figure7.pdf", bbox_inches='tight')
    # -----------------------------------------------------------
def Figure8(data):
    f, ax = plt.subplots(1,1,sharex=True, figsize=(3,1.5))
    for g in data.keys():
        ax.plot(data[g], label=f'{g}',markersize=9.5)
    ax.set_xscale('log', base=2)
    ax.set_xticks([1,4,12,48,96,140],[1,4,12,48,96,'96h'] , fontsize=14, rotation=60)
    ax.set_yscale('log', base=2)
    ax.set_yticks([1,2,4,8,16,32,64],['1','2','4','8','16','32','64'],fontsize=14)
    plt.legend(loc='lower center',bbox_to_anchor=(0.45, 1.0) ,ncol=2, fontsize=14)
    # -------Add common x and y axis label-------------------
    f.text(0.5, -0.3, 'number of processors', ha='center', fontsize = 14)
#     f.text(-0.06, 0.5, 'self speedup', va='center', rotation='vertical', fontsize=14)
    #---------------------------------------------------------
    plt.savefig(f"{FIGURE_DIR}/Figure8.pdf", bbox_inches='tight')

def Figure9(data):
    graph_avail = list(data.keys())
    graphs = ["LJ", "TW","SD","CW", "HL12", "CH5", "GL2","GL10","GL20","COS5", "SQR","SQR_s"]
    algorithms = list(data[graph_avail[0]].keys())
    i = 0
    f, ax = plt.subplots(2,6,sharex=True, figsize=(10,3.5))
    f.add_subplot(111, frameon=False)
    ax = ax.flatten()
    plt.subplots_adjust(left=None, bottom=None, right=None, top=0.9, wspace=0.45, hspace=0.25)
    x = [0,1,2,3]
    # Names of group and bar width
    for g in graphs:
        if g in graph_avail:
            ax[i].bar(x, data[g].loc["Trimming"], width = 0.5, label = "Trimming")
            bottom_bar = (data[g].loc["Trimming"]).copy()
            ax[i].bar(x, data[g].loc["First SCC"], bottom = bottom_bar, width = 0.5, label='First SCC')
            bottom_bar += data[g].loc["First SCC"]
            ax[i].bar(x, data[g].loc['Multi-search'], bottom = bottom_bar, width = 0.5, label='Multi-search')
            bottom_bar += data[g].loc['Multi-search']
            ax[i].bar(x, data[g].loc['Hash Table Resizing'],  bottom=bottom_bar, width = 0.5, label='Hash Table Resizing')
            bottom_bar += data[g].loc['Hash Table Resizing']
            ax[i].bar(x, data[g].loc['Others'],  bottom=bottom_bar, width = 0.5, label='Others')
            total_time = bottom_bar+data[g].loc['Others']
            speedup=total_time['GBBS']/total_time["Final"]
            ax[i].set_title(f'{g}: {speedup:.1f}', y=0.97)
            ax[i].set_xticks(x,algorithms , fontsize=14, rotation=60)
        else:
            ax[i].set_title(f'{g}', y=0.97)
            ax[i].set_xticks(x,algorithms , fontsize=14, rotation=60)
        i+=1
    ax[2].legend(loc='lower center',bbox_to_anchor=(0.5,1.15), fontsize=14, ncol=3)
    plt.tick_params(labelcolor='none', which='both', top=False, bottom=False, left=False, right=False)
    plt.ylabel("running time(s)", fontsize=14, labelpad=10)
    plt.savefig(f"{FIGURE_DIR}/Figure9.pdf", bbox_inches='tight')
def Figure10(data):
    graph_avail = list(data.keys())
    graphs = ["LJ", "TW","SD","CW", "HL12", "CH5", "GL2","GL10","GL20","COS5", "SQR","SQR_s"]
    f, ax = plt.subplots(2,6, figsize=(10,3.5))
    ax = ax.flatten()
    plt.subplots_adjust(left=None, bottom=None, right=None, top=0.9, wspace=0.5, hspace=0.60)
    i = 0
    for g in graphs:
        if g in graph_avail:
            ax[i].tick_params(axis = 'y', pad = 0.1)
            # ax[i].yaxis.set_major_locator(MaxNLocator(5, integer=True))
            colors_map = list(sns.color_palette('tab10'))
            ax[i].scatter(data[g]["VGC"], data[g]["Plain"], alpha=0.2, color = colors_map[0])
            factor = sum(data[g]["Plain"])/sum(data[g]["VGC"])
            ax[i].set_title("{} k={:.0f}".format(g, factor), fontsize=14, y=0.97)
            ax[i].tick_params(axis='x', rotation=90)
        else:
            ax[i].set_title(f"{g}", fontsize=14, y=0.97)
            ax[i].tick_params(axis='x', rotation=90)
        i+=1
    f.add_subplot(111, frameon=False)
    # hide tick and tick label of the big axes
    plt.tick_params(labelcolor='none', top=False, bottom=False, left=False, right=False)
    plt.grid(False)
    plt.xlabel("# rounds with VGC", fontsize=14, labelpad=10)
    plt.ylabel("# rounds without VGC", fontsize=14, labelpad=15)
    plt.savefig(f"{FIGURE_DIR}/Figure10.pdf", bbox_inches='tight')

def Figure11(data):
    graph_avail=list(data.keys())
    graphs = ["TW", "SD","CW","GL5", "COS5","SRQ_s"]
    threads = list(data[graph_avail[0]].keys())
    f, ax = plt.subplots(2,3, figsize=(9,3),sharex=True)
    ax = ax.flatten()
    plt.subplots_adjust(left=None, bottom=None, right=None, top=None,wspace=0.15, hspace=0.25)
    i = 0
    tau = [1<<x for x in range(18)]
    x = range(len(tau))
    colors_map={
        "192": "#000000",
        "96": "#3b28cc",
        "48":"#2667ff",
        "24":"#3f8efc",
        "4":"#87bfff",
    }
    lines_map={
        "192":"solid",
        '96':"--",
        "48":"-.",
        "24": ":", #"dashed",
        "4": "solid", #"dotted",
    }
    for g in graphs:
        if g in graph_avail:
            for t in threads:
                t_label=t
                if (t=='192'):
                    t_label='96h'
                ax[i].plot(tau, data[g][t]/data[g][t].loc["2^0"], label=t_label, color=colors_map[t], linestyle=lines_map[t])
            ax[i].set_xscale('log', base=2)
            ax[i].set_xticks([2**0, 2**8, 2**16],['$2^0$','$2^8$','$2^{16}$'] , fontsize=14)
            ax[i].tick_params(axis = 'y', pad = 0.1)
            ax[i].set_ylim(ymin=0)
            ax[i].set_title(f'{g}')
        else:
            ax[i].set_title(f"{g}")
        i+=1
    f.text(0.5, -0.05, r'$\tau$', ha='center', fontsize=16)
    f.text(0.07, 0.5, 'running time/no VGC', va='center', rotation='vertical', fontsize=14)
    ax[1].legend(loc = 'lower center', bbox_to_anchor=(0.5,1.2), ncol = len(threads), fontsize=14)
    plt.savefig(f"{FIGURE_DIR}/Figure11.pdf", bbox_inches='tight')

if __name__ == "__main__":
    data_scale = read_multiple_dataframes(f"{RESULT_DIR}/Figure7.csv")
    table3=pd.read_csv(f"{RESULT_DIR}/Table3.csv",header=0, index_col=0)
    Tarjan_data = table3["SEQ"]
    data_self_speedup = pd.read_csv(f"{RESULT_DIR}/Figure8.csv",header=0, index_col=0)
    data_breakdown=read_multiple_dataframes(f"{RESULT_DIR}/Figure9.csv")
    data_rounds = read_multiple_dataframes(f"{RESULT_DIR}/Figure10.csv")
    data_tau = read_multiple_dataframes(f"{RESULT_DIR}/Figure11.csv")
    print("drawing Figure7")
    Figure7(data_scale, Tarjan_data)
    print(f"store Figure7.pdf to {FIGURE_DIR}/Figure7.pdf")
    print("drawing Figure8")
    Figure8(data_self_speedup)
    print(f"store Figure8.pdf to {FIGURE_DIR}/Figure8.pdf")
    print("drawing Figure9")
    Figure9(data_breakdown)
    print(f"store Figure9.pdf to {FIGURE_DIR}/Figure9.pdf")
    print("drawing Figure10")
    Figure10(data_rounds)
    print(f"store Figure10.pdf to {FIGURE_DIR}/Figure10.pdf")
    Figure11(data_tau)
    print(f"store Figure11.pdf to {FIGURE_DIR}/Figure11.pdf")