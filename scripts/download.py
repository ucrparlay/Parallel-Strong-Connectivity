from graphs import dir_graphs 
from graphs import sym_graphs
import subprocess
import os

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))

if __name__ == '__main__':
    print("Downloading directed graphs.")
    for key,val in dir_graphs.items():
        (graph,url) = val
        if (val is None):
             print(f"{graph} is not uploaded online.")
             continue
        print(f'Downloading {graph} from {url}')
        graph_file = f'{CURRENT_DIR}/../data/dir/{graph}.bin'
        if not os.path.exists(graph_file):
            if len(url) == 0:
                print(f'Cannot download {graph}!')
                continue
            subprocess.call(f'wget -O {graph_file} {url}', shell=True)
            print(f'Successfully downloaded {graph}')
        else:
            print(f'Using pre-downloaded {graph}')
    print("Downloading undirected graphs.")
    for key,val in sym_graphs.items():
        (graph,url) = val
        if (val is None):
             print(f"{graph} is not uploaded online.")
             continue
        print(f'Downloading {graph} from {url}')
        graph_file = f'{CURRENT_DIR}/../data/sym/{graph}.bin'
        if not os.path.exists(graph_file):
            if len(url) == 0:
                print(f'Cannot download {graph}!')
                continue
            subprocess.call(f'wget -O {graph_file} {url}', shell=True)
            print(f'Successfully downloaded {graph}')
        else:
            print(f'Using pre-downloaded {graph}')
