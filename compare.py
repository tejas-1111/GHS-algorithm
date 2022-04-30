import os
import sys
f1 = sys.argv[1]
f2 = sys.argv[2]
ghs_node = []
ghs_weight = []
orig_node = []
orig_weight = []
with open(f1, 'r') as f:
    lines = f.readlines()
    for line in lines:
        token = line.split()
        if token[0] == "MST:":
            ghs_node.append((int(token[1]), int(token[2])))
            ghs_weight.append(int(token[3]))
with open(f2, 'r') as f:
    lines = f.readlines()
    for line in lines:
        token = line.split()
        orig_node.append((int(token[0]), int(token[1])))
        orig_weight.append(int(token[2]))

sorted_ghs = [(y, x) for y, x in sorted(zip(ghs_weight, ghs_node))]
sorted_orig = [(y, x) for y, x in sorted(zip(orig_weight, orig_node))]

print("----------------- Values from GHS ---------------")
for val in sorted_ghs:
    print(f"{val[1][0]} {val[1][1]} {val[0]}")
print("-----------------      True       ---------------")
for val in sorted_orig:
    print(f"{val[1][0]} {val[1][1]} {val[0]}")
print("-------------------------------------------------")
print(f"Are they the same?? : {sorted_orig == sorted_ghs}")