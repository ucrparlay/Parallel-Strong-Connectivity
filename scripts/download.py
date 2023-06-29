from graphs import graphs
import subprocess
import os

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))

if __name__ == '__main__':
    for graph, url in graphs:
        print(f'Downloading {graph} from {url}')
        graph_file = f'{CURRENT_DIR}/../data/{graph}.bin'
        if not os.path.exists(graph_file):
            if len(url) == 0:
                print(f'Cannot download {graph}!')
                continue
            subprocess.call(f'wget -O {graph_file} {url}', shell=True)
            print(f'Successfully downloaded {graph}')
        else:
            print(f'Using pre-downloaded {graph}')



# #/bin/bash

# wget -c -O ./data/soc-LiveJournal1.bin "https://www.dropbox.com/s/frgo03tkxcezb1e/soc-LiveJournal1.bin?dl=1"

# wget -c -O ./data/soc-LiveJournal1_sym.bin "https://www.dropbox.com/s/6mbmwerbzqa0cc1/soc-LiveJournal1_sym.bin?dl=1"

# wget -c -O ./data/GeoLifeNoScale_5.bin "https://www.dropbox.com/s/6jcqocm225wfvdg/GeoLifeNoScale_5.bin?dl=1"

