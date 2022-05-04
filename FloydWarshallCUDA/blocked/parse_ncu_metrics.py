#! /usr/bin/python3

import sys

metrics_file_name = sys.argv[1]

with open(metrics_file_name, "rt") as f:
    lines = f.readlines()
    lines = [line.split(" ")[0].strip() for line in lines[4:]]
    lines_combined = ""
    for i in range(len(lines)):
        if "smsp__sass_inst_executed_op_memory" in lines[i] or "smsp__sass_inst_executed_op_shared" in lines[i]:
            lines_combined += lines[i]
            lines_combined += ","
    lines_combined = lines_combined.strip()[:-2]
    print(lines_combined)
