## modelo espaço livre
import numpy as np

Gt = float(input("digite um valor para o ganho de transmissor no valor de 0 a 1: "))
Gr = float(input("digite um valor para o ganho de receptor no valor de 0 a 1: "))
lambda1 = float(input("digite um valor para o comprimento de onda do sinal de onda a ser transmitido em metros: "))
pi = np.pi
d = float(input("digite a distancia entre o emissor e o receptor em metros: "))

argumento_do_log = (Gt*Gr*(lambda1 ** 2)) / (4* (pi ** 2) * (d **2 ))
logaritmo = np.log(argumento_do_log)

Loss = -10 * logaritmo

print(Loss)

##(Gt*Gr*(lambda1 ** 2)) / (4* (pi ** 2) * (d **2 ))