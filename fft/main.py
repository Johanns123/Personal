import matplotlib.pyplot as plt  
import numpy as np  
##Por enquanto código só funciona na escala dos KHz
##estabeleça um intervalo para a aplicação necessária.
#Como estamos trabalhando em THz o intervalo no eixo x precisa ser grande assim como o multiplicador v
n = 10000000 ##numero de pontos no gráfico - eixo x
m = 0.000001 ##passo - este aumenta a precisção de pontos no gráfico - eixo y
t = np.arange(0.0, 10, m) ##ponto de partida, ponto de chegada e o passo
A1 = int(input("Digite a amplitude do sinal 1: "))
w1 = int(input("Digite a frequencia do sinal 1: "))
A2 = int(input("Digite a amplitude do sinal 2: "))
w2 = int(input("Digite a frequencia do sinal 2: "))
sinal_gerado = A1*np.sin(w1*2*np.pi*t) + A2*np.sin(w2*2*np.pi*t)
plt.plot(t, sinal_gerado)
plt.xlabel('Tempo (s)')
plt.ylabel('Amplitude')
plt.title('Sinal original')
plt.grid(True)
##plt.savefig("plot-voltage-vs.-time.png")
v = 1000 ##valor que multiplica os valores no eixo x para representar a frequencia
freq = np.fft.fftfreq(n) *v
fft_calculo = np.fft.fft(sinal_gerado)  ##faz a fft para a soma dos sinais
fft_abs = 2.0*np.abs(fft_calculo/n)
plt.figure(2)
plt.title("Transformada")
plt.xlabel('kHz')
plt.ylabel('Amplitude')
plt.grid(True)
plt.plot(freq, fft_abs)

plt.show()