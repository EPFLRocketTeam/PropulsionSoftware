#python

f = open("test.txt", "r")

deg = []

for l in f:
	deg.append(int(float(l)*10))

print(len(deg))
	
print(2**15)

f2 = open("res.c", "w")

f2.write("static const uint16_t temp_LUT[] = {\n")
cnt = 0
for i in range(100):
	for j in range(16):
		f2.write("{}, ".format(deg[cnt]))
		cnt += 1
	f2.write("\n")

f2.write("};")