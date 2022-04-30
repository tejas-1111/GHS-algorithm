file1 = open('output.txt', 'r')
Lines = file1.readlines()

count = 0
# Strips the newline character
lst = []
for line in Lines:
    lst.append(line.strip())

lst2 = []
lst3 = []
cnt = 0
for line in lst:
    tempL = line.split(' ')[0]
    if tempL[0] != '[':
        lst3.append(line)
    else:
        timeT = tempL[1:len(tempL)-1]
        lst2.append((cnt, timeT))
    cnt += 1

lst2.sort(key=lambda y: y[1])

file1 = open("new_output.txt", "w")
for tup in lst2:
    file1.write(lst[tup[0]])
    file1.write('\n')

for line in lst3:
    file1.write(line)
    file1.write('\n')
