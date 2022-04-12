import re
import numpy as np
import networkx as nx
import random
import sys
import subprocess

size = 1000

# When using wiki vote 
# size = 10000
filetarget = "wikivote"

def read():
    inputfile = open("raw_fw.out", mode='rb')
    fw = np.fromfile(inputfile, dtype=np.ushort)
    fw = fw.reshape(size, size)
    inputfile.close
    print(len(fw))

    inputfile = open("cc.out", mode='rb')
    ccc = np.fromfile(inputfile, dtype=np.single)
    inputfile.close
    print(len(ccc))

    # print(fw[3])
    # sum_3 = 0
    # count = 0
    # for i in range(len(fw[3])):
    #     if fw[3][i] != 65535:
    #         sum_3 += fw[3][i]
    #         count += 1
    # print(sum_3, count)
    # print(clo_cen[3])
    # print(ccc[3])

    # print("comp")
    # for i in range(1000):
    #     if G.has_node(i):
    #         print("{:.4f}".format(clo_cen[i]), "{:.4f}".format(ccc[i]))
    #     else:
    #         print("None", "{:.4f}".format(ccc[i]))

def genCfile(G, networkxfile, weighted = False):
    # 65535 is inf
    adj_mat = np.zeros((size,size), dtype=np.ushort)
    adj_mat.fill(65535)

    with open(networkxfile, 'r') as file:
        for line in file:
            # print(line)
            linedata = re.findall(r'(\d+)', line)

            row = int(linedata[0])
            col = int(linedata[1])
            if weighted:
                adj_mat[row][col] = linedata[2]
            else:            
                adj_mat[row][col] = 1
    file.close()

    # set diagonal to 0
    for i in range(len(adj_mat[0])):
        if G.has_node(i):
            adj_mat[i][i] = 0

    outfile = open("adjmat_short.bin", mode='wb')
    adj_mat.tofile(outfile)
    outfile.close

def main():
    val = ""
    while val == "":
        val = input("Generate Graph(G) or Read(R)\n")
        if((val == "G") or (val == "g")):
            # using a set seed
            G = nx.gnp_random_graph(size, 0.4, seed=42, directed=True)
            # weighted graph / networkx cc takes along time when weighted
            # nx.set_edge_attributes(G, {e: {'weight': random.randint(1, 3)} for e in G.edges})
            nx.set_edge_attributes(G, {e: {'weight': 1} for e in G.edges})
        elif((val == "R") or (val == "r")):
            # When reading from file
            G = nx.read_edgelist(filetarget, nodetype=int, create_using= nx.DiGraph())
            print(G.number_of_nodes())

    #dump generated graph to a file
    fp = open("gen_networkx", "wb")
    nx.write_edgelist(G, fp)
    fp.close

    #create matrix binary file for the C program
    genCfile(G, "gen_networkx", weighted = False)

    # clo_cen = nx.closeness_centrality(G.reverse(), distance='weight', wf_improved=False)
    clo_cen = nx.closeness_centrality(G.reverse(), wf_improved=False)
    max_idx = 0
    max_val = 0
    for item in clo_cen:
        if clo_cen[item] > max_val:
            max_idx = item
            max_val = clo_cen[item]
    print("python closest closest centrality: index: ", max_idx, ", val: ", max_val, "\n")

    # run C base line code
    val = ""
    while val == "":
        val = input("Run C program? Y/N \n")
        if ((val == "Y") or (val == "y")):
            subprocess.run(['gcc', '-O1', 'full_CC.c', '-o', 'CCout'])
            subprocess.run(['./CCout', str(size)])
        # elif ((val == "N") or (val == "n")):
        #     None    

if __name__ == '__main__':
    np.set_printoptions(threshold=sys.maxsize)
    main()