import numpy as np
import pandas as pd
import random

def pd_to_file(df, filename):
    with open(filename, "w") as out:
        for index, row in df.iterrows():
            non_zero = np.nonzero(row.to_numpy())
            out.write(f"{len(non_zero[0])} ")
            for index in non_zero[0]:
                out.write(f"{index} ")
            out.write("\n")

num_nodes = 51

for num_nodes in [90, 128, 180, 256, 362, 2048]:
    out = np.zeros((num_nodes, num_nodes))
    for n in range(num_nodes):
        x = [random.randint(0, num_nodes - 1) for _ in range(0, 5)]
        for neighbor in x:
            out[n][neighbor] = 1
            out[neighbor][n] = 1

    pd_to_file(pd.DataFrame(out), f"size{num_nodes}.adj")
