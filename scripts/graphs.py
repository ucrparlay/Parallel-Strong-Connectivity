
# If you want to download other graphs, you can uncomment the corresponding line,
# but may not download successful because of the band width limitation of dropbox.
# You can try to download them another day.
import os
CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
# GRAPH_DIR = f"{CURRENT_DIR}/../data"
GRAPH_DIR = "/ssd0/graphs/links"
dir_graphs = {
    # Social Graphs
    "LJ": ('soc-LiveJournal1', ),
    "TW": ('twitter', ),

    # Web Graphs
    "SD": ('sd_arc', ),
    # "CW": ('clueweb', ),
    # "HL14": ('hyperlink2014',),
    # "HL12": ('hyperlink2012', ),

    # KNN Graphs
    "HH5": ('Household.lines_5', ),
    "CH5": ('CHEM_5', ),
    "GL2": ('GeoLifeNoScale_2',),
    "GL5": ('GeoLifeNoScale_5',),
    "GL10": ('GeoLifeNoScale_10',),
    "GL15": ('GeoLifeNoScale_15', ),
    "GL20": ('GeoLifeNoScale_20', ),
    "COS5": ('Cosmo50_5', ),

    # Lattice Graphs
    "SQR": ('grid_4000_4000', ),
    "REC": ('grid_1000_10000',),
    "SQR'": ('grid_4000_4000_03', ),
    "REC'": ('grid_1000_10000_03',),
}

sym_graphs = {
    # Social Graphs
    "OK": ('com-orkut_sym', 'https://www.dropbox.com/s/2okcj8q7q1z3nkw/com-orkut_sym.bin?dl=0'),
    "LJ": ('soc-LiveJournal1_sym', 'https://www.dropbox.com/s/6pbryf0wpxgmj7c/soc-LiveJournal1_sym.bin?dl=0'),
    "TW": ('twitter_sym', 'https://www.dropbox.com/s/vtnfg6e75h0jama/twitter_sym.bin?dl=0'),
    "FT": ('friendster_sym', 'https://www.dropbox.com/s/6xiwui88bacbt6o/friendster_sym.bin?dl=0'),

    # Web Graphs
    "SD": ('sd_arc_sym', 'https://www.dropbox.com/s/xtkei44fh9v5v73/sd_arc_sym.bin?dl=0'),
    # "CW": ('clueweb_sym', ),
    # "HL14": ('hyperlink2014_sym',),
    # "HL12": ('hyperlink2012_sym', ),

    # Road Graphs
    "GE": ('Germany_sym', 'https://www.dropbox.com/s/42s5gb1gjs36x3z/Germany_sym.bin?dl=0'),
    "USA": ('RoadUSA_sym', 'https://www.dropbox.com/s/a6dhyw9ec0d3hlh/RoadUSA_sym.bin?dl=0'),

    # KNN Graphs
    "HH5": ('Household.lines_5_sym', 'https://www.dropbox.com/s/281aia4r73owtjk/Household.lines_5_sym.bin?dl=0'),
    "CH5": ('CHEM_5_sym', 'https://www.dropbox.com/s/r0c1smelm4lllbz/CHEM_5_sym.bin?dl=0'),
    "GL2": ('GeoLifeNoScale_2_sym', ),
    "GL5": ('GeoLifeNoScale_5_sym', 'https://www.dropbox.com/s/32cb0d645i2qf30/GeoLifeNoScale_5_sym.bin?dl=0'),
    "GL10": ('GeoLifeNoScale_10_sym', ),
    "GL15": ('GeoLifeNoScale_15_sym', ),
    "GL20": ('GeoLifeNoScale_20_sym', ),
    "COS5": ('Cosmo50_5_sym', 'https://www.dropbox.com/s/8oxqut1ff7l73ws/Cosmo50_5_sym.bin?dl=0'),

    # Lattice Graphs
    "SQR": ('grid_4000_4000_sym', ),
    "REC": ('grid_1000_10000_sym',),
    "SQR'": ('grid_4000_4000_03_sym', ),
    "REC'": ('grid_1000_10000_03_sym',),
}
