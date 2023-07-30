
# If you want to download other graphs, you can uncomment the corresponding line,
# but may not download successful because of the band width limitation of dropbox.
# You can try to download them another day.
import os
CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
GRAPH_DIR = f"{CURRENT_DIR}/../data"
# GRAPH_DIR = "/ssd0/graphs/links"
dir_graphs = {
    # Social Graphs
    "LJ": ('soc-LiveJournal1', 'https://www.dropbox.com/scl/fi/836oq1mpruk6y0pul4d4t/soc-LiveJournal1.bin?rlkey=a97bmdoi31a0v87x2j6lmrldg&dl=0'),
    "TW": ('twitter', 'https://www.dropbox.com/scl/fi/033q9zfjon0jylnevejm5/twitter.bin?rlkey=2j9jcrzp55dm6gg0oypxw0r85&dl=0'),

    # Web Graphs
    "SD": ('sd_arc', 'https://www.dropbox.com/scl/fi/3lf3eoff3nafyz5cxd4m8/sd_arc.bin?rlkey=m1cdvqe1obnbkd68xksuaaa1u&dl=0'),
    # "CW": ('clueweb', ),
    # "HL14": ('hyperlink2014',),
    # "HL12": ('hyperlink2012', ),

    # KNN Graphs
    "HH5": ('Household.lines_5', 'https://www.dropbox.com/scl/fi/ndpp5pq2jjlhuqhpx91r7/Household.lines_5.bin?rlkey=n8s0s9wqyzqfxdl82mmqui1z7&dl=0'),
    "CH5": ('CHEM_5', 'https://www.dropbox.com/scl/fi/f0n4o9mrbcfqhw54oa95i/CHEM_5.bin?rlkey=cji03kmi4e5tzmrdj36jsk062&dl=0'),
    "GL2": ('GeoLifeNoScale_2', 'https://www.dropbox.com/scl/fi/kckxpp3wcmpbkphikqer4/GeoLifeNoScale_2.bin?rlkey=ybmgdkymbq4i06fvsumk7w9ak&dl=0'),
    "GL5": ('GeoLifeNoScale_5', 'https://www.dropbox.com/scl/fi/kckxpp3wcmpbkphikqer4/GeoLifeNoScale_2.bin?rlkey=ybmgdkymbq4i06fvsumk7w9ak&dl=0'),
    "GL10": ('GeoLifeNoScale_10', 'https://www.dropbox.com/scl/fi/5fw3fhmaqekhihevxvj5e/GeoLifeNoScale_10.bin?rlkey=ed5qn87vdc9m0fjgey7bcopzx&dl=0'),
    "GL15": ('GeoLifeNoScale_15', 'https://www.dropbox.com/scl/fi/roc5tryp828wj3sdlzceu/GeoLifeNoScale_15.bin?rlkey=20tfzbv69xsh841sobh5tny05&dl=0'),
    "GL20": ('GeoLifeNoScale_20', 'https://www.dropbox.com/scl/fi/roc5tryp828wj3sdlzceu/GeoLifeNoScale_15.bin?rlkey=20tfzbv69xsh841sobh5tny05&dl=0'),
    "COS5": ('Cosmo50_5', 'https://www.dropbox.com/scl/fi/z6xpgzyb4vhhw42fy2xqt/Cosmo50_5.bin?rlkey=mcmlrx2ga8f6y5tfhu1wyduqi&dl=0'),

    # # Lattice Graphs
    "SQR": ('grid_4000_4000', 'https://www.dropbox.com/scl/fi/7jt1isj9oxejjroeailvk/grid_4000_4000.bin?rlkey=boftsrs25u9gngbmg7mfuaao0&dl=0'),
    "REC": ('grid_1000_10000', 'https://www.dropbox.com/scl/fi/wxc3cbdg3i3i6kuqh8ydf/grid_1000_10000.bin?rlkey=ouj3ulurgeomxoruc7g9f6tur&dl=0'),
    "SQR_s": ('grid_4000_4000_03', 'https://www.dropbox.com/scl/fi/jsyw9lqsfawpk7642sfcg/grid_4000_4000_03.bin?rlkey=rdjgqc8roqfh3spos858h4aon&dl=0'),
    "REC_s": ('grid_1000_10000_03', 'https://www.dropbox.com/scl/fi/cctseke6v0d5lgugmg89z/grid_1000_10000_03.bin?rlkey=hicjbj4z9y6sj9uogvo9em4s5&dl=0'),
}

sym_graphs = {
    # Social Graphs
    "OK": ('com-orkut_sym', 'https://www.dropbox.com/scl/fi/dlj3uguxml0rv50m5vtvc/com-orkut_sym.bin?rlkey=lo646ezb1tgpohqbhfx47jm5x&dl=0'),
    "LJ": ('soc-LiveJournal1_sym', 'https://www.dropbox.com/scl/fi/oe10d1vtlnc25ewmcn934/soc-LiveJournal1_sym.bin?rlkey=iobijg482q97lnc6zlisrrlow&dl=0'),
    "TW": ('twitter_sym', 'https://www.dropbox.com/scl/fi/5ysbs3apwcao1mr7m6hz3/twitter_sym.bin?rlkey=mo2wwcyykpur0ngd7mkauj734&dl=0'),
    "FT": ('friendster_sym', 'https://www.dropbox.com/scl/fi/viae36nowikb5s37g4tje/friendster_sym.bin?rlkey=50i0bxfcj4pidy6z894b83sam&dl=0'),

    # Web Graphs
    "SD": ('sd_arc_sym', 'https://www.dropbox.com/scl/fi/c6v55sz96mgkzwk0184rp/sd_arc_sym.bin?rlkey=pj4bxsp71q9c21rtobumes1gr&dl=0'),
    # "CW": ('clueweb_sym', ),
    # "HL14": ('hyperlink2014_sym',),
    # "HL12": ('hyperlink2012_sym', ),

    # Road Graphs
    "GE": ('Germany_sym', 'https://www.dropbox.com/scl/fi/plqdw97q6tia1x7sp2jfy/Germany_sym.bin?rlkey=vq0qnb88oxc8i3h2om85itm36&dl=0'),
    "USA": ('RoadUSA_sym', 'https://www.dropbox.com/scl/fi/z3ohfnkd3g5y3e42musrg/RoadUSA_sym.bin?rlkey=umazxwlin3jq91w5jw4x1j8n3&dl=0'),

    # KNN Graphs
    "HH5": ('Household.lines_5_sym', 'https://www.dropbox.com/s/zrvsznkhb677ykx/Household.lines_5_sym.bin?dl=0'),
    "CH5": ('CHEM_5_sym', 'https://www.dropbox.com/scl/fi/ztprqbch6ag6t29ksdm1b/CHEM_5_sym.bin?rlkey=v7szexxy41bpq90uyyy619skc&dl=0'),
    "GL2": ('GeoLifeNoScale_2_sym', 'https://www.dropbox.com/scl/fi/lpe1am6ctaf71xgp9akex/GeoLifeNoScale_2_sym.bin?rlkey=bwd6zl9ywgsmuc4pagdvl4o6o&dl=0'),
    "GL5": ('GeoLifeNoScale_5_sym', 'https://www.dropbox.com/scl/fi/xhgulpc1ylxzss9pnudvd/GeoLifeNoScale_5_sym.bin?rlkey=0lhv011twt46q58wri69py819&dl=0'),
    "GL10": ('GeoLifeNoScale_10_sym', 'https://www.dropbox.com/scl/fi/u93oeenes01qubypufupo/GeoLifeNoScale_10_sym.bin?rlkey=sydvff8m72dsupthrivi5t8un&dl=0'),
    "GL15": ('GeoLifeNoScale_15_sym', 'https://www.dropbox.com/scl/fi/o16640hz50ng0efll8vpq/GeoLifeNoScale_15_sym.bin?rlkey=u592t7ua8h0vhk635bibbzj5l&dl=0'),
    "GL20": ('GeoLifeNoScale_20_sym', 'https://www.dropbox.com/scl/fi/x8eatb17zrle5ow3pckvy/GeoLifeNoScale_20_sym.bin?rlkey=4dcoofekduds5dyxdtq8ryk8g&dl=0'),
    "COS5": ('Cosmo50_5_sym', 'https://www.dropbox.com/scl/fi/0i5kxloqla8tiv3627ldg/Cosmo50_5_sym.bin?rlkey=a0qmrs8jdls8e1cza3rztvbxx&dl=0'),

    # Lattice Graphs
    "SQR": ('grid_4000_4000_sym', 'https://www.dropbox.com/scl/fi/z59lvitzfyvzxlirzul93/grid_4000_4000_sym.bin?rlkey=4uq6oddrfjk22vz97qw0wmpre&dl=0'),
    "REC": ('grid_1000_10000_sym', 'https://www.dropbox.com/scl/fi/c3oj4o8gu6xs1i2zke7fb/grid_1000_10000_sym.bin?rlkey=1nyokx9en5hbyjvanhgyfcbjg&dl=0'),
    "SQR_s": ('grid_4000_4000_03_sym', 'https://www.dropbox.com/scl/fi/g3yq60g6782jb16v11u6a/grid_4000_4000_03_sym.bin?rlkey=9twes320fc4l74kkurquzkwim&dl=0'),
    "REC_s": ('grid_1000_10000_03_sym', 'https://www.dropbox.com/scl/fi/cf3oxv8cvjermz4lcx6v2/grid_1000_10000_03_sym.bin?rlkey=3u533rr5x1kdrztajebpb6w4a&dl=0'),
}
