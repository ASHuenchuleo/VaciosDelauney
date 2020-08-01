import matplotlib.pyplot as plt

N = 		[100,    5000,  15000,  50000,    10**5,    3*10**5, 6*10**5, 10**6,  3 * 10**6, 6 * 10**6, 10**7, 2*10**7, 4*10**7, 6*10**7]
times_sec = [458,    6074,  18668,  39938,    85676,    242073,  502873,  952909, 3053038, 	 6151501,  9848988]
times_mix = [112264, 98990, 100291, 114350,   112500,   138813,  174301,  221730, 429256,  	 781597,  1370126]
times_par = [108653, 105354, 117187, 122628,  154598,   241792,  397305,  593274, 1582527, 	 3240351,  5458998]

plt.loglog(N, times_sec, '.-', label="Sequential")
plt.loglog(N, times_mix, '.-', label="Mixed")
plt.loglog(N, times_par, '.-', label="Parallel")
plt.title("Tiempos de ejecución para los distintos algoritmos")
plt.xlabel("N")
plt.ylabel("Tiempo [microsegundos]")
plt.legend()
plt.show()
