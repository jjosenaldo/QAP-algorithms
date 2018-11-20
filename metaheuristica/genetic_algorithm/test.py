a = dict()
with open("hue.out", "r") as f:
	mylist = f.read().splitlines()
	for line in mylist:
		line = [i for i in line.split(" ")]
		try:
			a[int(line[0])] = max(a[int(line[0])],int(line[2]))
		except KeyError:
			a[int(line[0])] = int(line[2])
print(a)