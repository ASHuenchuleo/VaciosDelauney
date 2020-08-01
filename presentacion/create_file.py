import random
sizes = [2*10**7, 4*10**7, 6*10**7]
for size in sizes:
	file = open(f"data_{size}.dat", "w+")
	file.write(f"{str(2)}\n")
	file.write(f"{size}\n")
	print(size, "done")
	for i in range(size):
		x = random.uniform(-1000, 1000)
		y = random.uniform(-1000, 1000)
		file.write(f"{'{:.5f}'.format(x)} {'{:.5f}'.format(y)}\n")