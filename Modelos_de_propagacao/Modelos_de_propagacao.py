# @Author: johannamorim
# @Date:   2021-07-07T16:02:12-03:00
# @Last modified by:   johannamorim
# @Last modified time: 2021-07-29T13:02:50-03:00


## modelo espaço livre

'''
Dados utilizados no artigo "CAPACIDADE E PREDIÇÃO DE CANAIS SEM FIO
EM ONDAS MILIMÉTRICAS PARA REDES
CELULARES DE QUINTA GERAÇÃO":
Em 2.5GHz:

Potência de saída do amplificador do transmissor é igual a 46.02dbm, pois o rádio comercial utiliza
esse valor na prática. Pt
Os ganhos na transmissão são de 0 dB. Gt
As perdas na transmissão são de 0 dB.
As perdas por cabo no valor de 4 dB a cada 100 metros.
O comprimento do cabo de 10 metros, considerando que o rádio esteja próximo da antena
junto à torre de sustentação.
As perdas por conectores de 0.5 dB e a quantidade de conectores igual a 4, dois conectando o
rádio ao cabo e dois conectando o cabo à antena, tanto na transmissão quanto na recepção.
O ganho da antena ajustado manualmente em 15.8 dBd, tanto na recepção quanto na trans-
missão.
A potência de saída do receptor de 22.99dbm.
Os ganhos na recepção são de 3 dB.  Gr
Não há perdas na recepção.
A potência efetivamente radiada calculada em 59.4 dBm.
Raio de alcance: 10km
'''


import numpy as np
import matplotlib.pyplot as plt
import random


def espaco_livre():

    Pt_dBm = 46.02  ##Potência de transmissão
    '''dbi = dbm + 2.15
       dbi = 3 + 2.15 = 5.15'''

    Gt_dBi = 0            ##ganho de transmissão
    Gr_dBi = 5.15         ##ganho de recpeção
    f = 2.5 * (10 ** 9)   ##2.5GHz

    d = np.zeros(200)

    for i in range(200):
        d[i] = 41935000 * i

    L = 1
    n = 2

    # Pt_dBm = potência de transm.
    # Gt_dBi = Ganho da antena de transmissão em dBi
    # Gr_dBi = Ganho da antena de recepção em dBi
    # f = freq. em Hertz
    # d = vetor de distâncias onde a perda deverá ser calculada
    # L = outro sistema de perdas, caso sem perdas L = 1
    # n = path loss exponent (n = 2 para espaço livre)
    # Pr_dBm = Potência recebida em dBm
    # PL = path loss due to log normal shadowing

    Pr_dBm = calculo_modelo_sem_perdas(Pt_dBm, Gt_dBi, Gr_dBi, f, d, L, n)

    plt.plot(np.log10(d), Pr_dBm)
    plt.xlabel('log10(d)')
    plt.ylabel('P_r(dBm)')
    plt.title('Friis Path Loss Model')
    plt.grid(True)

    plt.show()


def calculo_modelo_sem_perdas(Pt_dBm, Gt_dBi, Gr_dBi, f, d, L, n):
    lamda = 3 * (10 ** 8) / f  # comprimento de onda
    K = (
        20 * np.log10(lamda / (4 * np.pi))
        - 10 * n * np.log10(d)
        - 10 * np.log10(L)
    )  # constant of path-loss factor

    PL = Gt_dBi + Gr_dBi + K
    Pr_dBm = Pt_dBm + PL

    return Pr_dBm


##função do cálclo do modelo Log_normal
def logNormalShadowing(Pt_dBm, Gt_dBi, Gr_dBi, f, d0, d, L, sigma, n):

    lambda1 = 3 * (10 ** 8) / f
    K = (
        20 * np.log10(lambda1 / (4 * np.pi))
        - 10 * n * np.log10(d0)
        - 10 * np.log10(L)
    )
    # constante
    X = sigma * np.random.normal(1, len(d))
    # normal random variable
    PL = Gt_dBi + Gr_dBi + K - 10 * n * np.log10(d / d0) - X
    Pr_dBm = Pt_dBm + PL

    return Pr_dBm


def log_normal():

    Pt_dBm = 46.02  ##Potência de transmissão
    '''dbi = dbm + 2.15
       dbi = 3 + 2.15 = 5.15'''

    Gt_dBi = 0            ##ganho de transmissão
    Gr_dBi = 5.15         ##ganho de recpeção
    f = 2.5 * (10 ** 9)   ##2.5GHz

    d0 = 1      ##distância de referência de 1m
       
    d = 100 * np.array(
        [np.arange(1.0, 200, 0.2)]
    )  ##ponto de partida, ponto de chegada e o passo

    d1 = 100 * np.arange(
        1.0, 200, 0.2
    )  ##ponto de partida, ponto de chegada e o passo

    L = 1       ##admitindo sistema sem perdas
    sigma = 6   ##desvio padrão normal do modelos
    n = 3.5     ##constante atribuída para ambiente com obstrução por construções

    Pr_friis = calculo_modelo_sem_perdas(Pt_dBm, Gt_dBi, Gr_dBi, f, d1, L, n)

    plt.plot(np.log10(d1), Pr_friis, 'b')

    for i in range(50):
        Pr_shadow = logNormalShadowing(
            Pt_dBm, Gt_dBi, Gr_dBi, f, d0, d, L, sigma, n
        )
        plt.plot(np.log10(d), Pr_shadow, 'r*')

    plt.xlabel('log10(d)')
    plt.ylabel('P_r (dBm)')
    plt.title('Log Normal Shadowing Model')
    plt.legend('no shadowing', 'log normal shadowing')
    plt.grid(True)

    plt.show()


def two_ray_model():
    f = 2.5 * (10 ** 9)         ##2.5GHz
    R = -1                      ##coeficiente de reflexão
    Pt = 39994.47               ##em mW
    Glos = 1                    ##ganho com linha de visada (adimensional)
    Gref = 1                    ##ganho de referência (adimensional)
    ht = 30                     ##altura em metros  
    hr = 30
    d = np.arange(1,10**5, 0.1) ##varredura de 100Km
    L = 1


    d_los = np.sqrt((ht - hr)**2+d**2)
    d_ref = np.sqrt((ht + hr)**2+d**2)
    lambda1 = 3*10**8/f
    phi = 2*np.pi*(d_ref-d_los)/lambda1
    s = lambda1/(4*np.pi)*(np.sqrt(Glos)/d_los + R*np.sqrt(Gref)/d_ref*np.exp(1j*phi))
    Pr = Pt*np.abs(s)**2
    ##Pr_norm = Pr/Pr(1)

    plt.semilogx(d,10*np.log10(Pr))
    ##ylim([-160 -55])
    plt.title('Two ray ground reflection model')
    plt.xlabel('log_{10} (d)')
    plt.ylabel('Normalized Received power (in dB)')


    dc = 4*ht*hr/lambda1
    d1 = np.arange(1,ht, 0.1)
    d2 = np.arange(ht,dc, 0.1)
    d3 =  np.arange(dc,10**5, 0.1)

    K_fps = Glos*Gref*lambda1**2/((4*np.pi)**2*L)
    K_2ray = Glos*Gref*ht**2*hr**2/L

    Pr1 = Pt*K_fps/(d1**2+ht**2)
    Pr2 = Pt*K_fps/d2**2
    Pr3 = Pt*K_2ray/d3**4

    plt.semilogx(d1,10*np.log10(Pr1), 'k-.')
    plt.semilogx(d2,10*np.log10(Pr2), 'r-.')
    plt.semilogx(d3,10*np.log10(Pr3), 'g-.')

    plt.axvline(x=ht, color='m')
    plt.axvline(x=dc, color='m')

    plt.grid()
    plt.show()

    #h = line([ht ht], [-160 -55]); set(h, 'Color', 'm')
    #h = line([dc dc], [-160 -55]); set(h, 'Color', 'm')


def modelo_Okumura_Hata():
    f = 2.5 * (10 ** 9)         ##2.5GHz
    d = np.arange(1,20, 0.1)    ##até 20Km, unidade em Km
    hb = 30                     ##Altura da estação de base em m
    hm = 30                     ##Altura da estação móvel em m
    aHm = 0
    C = 0

    Pt_dB = 16.02   ##dB = dBm - 30  ##Potência de transmissão
    Gt_dBi = 0                  ##ganho de transmissão

    ambiente = int(input("Digite 1 para metrô, 2 para cidade pequena, 3 para subúrbio, 4 para ambiente aberto: "))

    if ambiente == 1:
        C = 0
        if f <= 200:
            aHm = 8.29*(np.log10(1.54*hm)**2) - 1.11
        else:
            aHm = 3.2*(np.log10(11.75*hm)**2) - 4.97

    elif ambiente == 2:
        C = 0
        aHm = (1.1*np.log10(f) - 0.7)*hm - (1.56*np.log10(f) - 0.8)

    elif ambiente == 3:
        aHm = (1.1*np.log10(f) - 0.7)*hm - (1.56*np.log10(f) - 0.8)
        C = -2*(np.log10(f/28)**2) - 5.4

    elif ambiente == 4:
        aHm = (1.1*np.log10(f) - 0.7)*hm - (1.56*np.log10(f) - 0.8)
        C = -4.78*(np.log10(f)**2) + (18.33*np.log10(f)) - 40.98


    A = 69.55 + (26.16*np.log10(f)) - (13.82*np.log10(hb)) - aHm
    B = 44.9 - (6.55*np.log10(hb))
    L_50 = A+(B*np.log10(d))+C

    if L_50.all() < 0:
        PL = Gt_dBi + Pt_dB + L_50

    elif L_50.all() > 0:
        PL = Gt_dBi + Pt_dB - L_50

    plt.plot(d, PL)
    plt.xlabel('Distância em kilômetros')
    plt.ylabel('Sinal recebido (dB)')
    plt.title('Modelo Okumura Hata')
    plt.grid(True)

    plt.show()

def Hata_modified():
    f = 2.5 * (10 ** 9)         ##2.5GHz
    hb = 30                     ##Altura da estação de base em m
    hm = 30                     ##Altura da estação móvel em m
    d = np.arange(0.12,1, 0.001)    ##varredura de 20km
 
    Pt_dB = 16.02              ##Potência de transmissão
    Gt_dBi = 0                  ##ganho de transmissão
    
    alpha = 1                   ##valor qnd menor que 20km

    a_hm = (1.1*np.log10(f) - 0.7)*10 - (1.56*np.log10(f) - 0.8)
    + np.log10(hm/10)

    b_hb = (1.1*np.log10(f) - 0.7) * 10 - (1.56*np.log10(f) - 0.8) 
    + 20*np.log10(hb/10)

    regiao = int(input("Determine o Local:\n1 - urbano\n2 - Suburbano\n3 - Área aberta\n"))

    ##L_50 = 0
    X = (44.9 - 6.55*np.log10(30))*(np.log10(d)**alpha)

    L_50_urbano = 46.3 + 33.9*np.log10(2000) + 10*np.log10(f/2000) - 13.82*np.log10(30)
    - a_hm - b_hb

    if regiao == 1:
        L_50 = L_50_urbano

    if regiao == 2:
        L_50 = L_50_urbano - 2*(np.log10(2000/28)**2) - 5.4
    
    if regiao == 3:
        L_50=  L_50_urbano - 4.78*(np.log(2000)**2) + 18.33*np.log10(2000) - 40.94
    
    ##print(L_50)

    L_50 += X       ##para normalizar as dimensões dos vetores no gráfico

    if L_50.all() < 0:
        PL = Gt_dBi + Pt_dB + L_50

    elif L_50.all() > 0:
        PL = Gt_dBi + Pt_dB - L_50


    plt.plot(d, PL)
    plt.xlabel('Distância em Kilômetros')
    plt.ylabel('Sinal recebido (dB)')
    plt.title('Modelo Hata Modificado')
    plt.grid(True)

    plt.show()


def modelo_Okumura_Hata_Davidson():
    f = 2.5 * (10 ** 9)         ##2.5GHz
    d = np.arange(1,20, 0.1)    ##até 20, unidade em km
    hb = 30                     ##Altura da estação de base em m
    hm = 30                     ##Altura da estação móvel em m
   
    Pt_dB = 16.02              ##Potência de transmissão
    Gt_dBi = 0                  ##ganho de transmissão

    aHm = (1.1*np.log10(f) - 0.7)*hm - (1.56*np.log10(f) - 0.8)

    A_hb_d = 0          ##0 qnd d < 20

    S_1_d = 0             ##0 qnd d < 64,38

    S_2_hb_d = 0.00784*np.abs(np.log10(9.98/d))*(hb - 300)

    S_3_f = (f/250) * np.log10(1500/f)

    ##S_4_f_d = (0.112*np.log10(1500/f))*(d - 64.38)

    A = 69.55 + (26.16*np.log10(f)) - (13.82*np.log10(hb)) - aHm
    B = 44.9 - (6.55*np.log10(hb))
    L_50_urb = A+(B*np.log10(d))

    L_50 = L_50_urb + A_hb_d - S_1_d - S_2_hb_d - S_3_f

    if L_50.all() < 0:
        PL = Gt_dBi + Pt_dB + L_50

    elif L_50.all() > 0:
        PL = Gt_dBi + Pt_dB - L_50

    plt.plot(d, PL)
    plt.xlabel('Distância em kilômetros')
    plt.ylabel('Sinal recebido (dB)')
    plt.title('Modelo Okumura-Hata-Davidson')
    plt.grid(True)

    plt.show()

def modelo_Okumura_Hata_Cost_231():

    f = 2.5 * (10 ** 9)         ##2.5GHz
    d = np.arange(1,20, 0.1)    ##até 20, unidade em km
    hb = 30                     ##Altura da estação de base em m
    hm = 30                     ##Altura da estação móvel em m
   
    Pt_dB = 16.02               ##Potência de transmissão
    Gt_dBi = 0                  ##ganho de transmissão
    CM = 0

    L_50 = 0
    L_50_urbano = 0

    if hm <= 10 or hm >= 1:
        aHm = (1.1*np.log10(f) - 0.7)*hm - (1.56*np.log10(f) - 0.8)

    if f <= 200*10**6:
        aHm = (8.29*(np.log10(1.54*hm)**2) - 1.1)
    
    elif f >= 400*10**6:
        aHm = (3.2*(np.log10(11.75*hm)**2) - 4.97)


    L_50_urbano = 46.3 + 33.9*np.log10(f) - 13.82*np.log10(hb) - aHm 
    + CM
    
    option = int(input("Urbana - 1\nSuburbana - 2\nÁrea rural - 3\n"))


    if option == 1:
        CM = 3
        L_50 = L_50_urbano

    elif option == 2:
        L_50 = L_50_urbano - 2*(np.log10(f/28)**2) - 5.4

    elif option == 3:
        L_50 = L_50_urbano - 4.78*(np.log10(f)**2) + 18.33*np.log10(f) - 40.94


    L_50 += + (44.9 - 6.55*np.log10(hb))*np.log10(d)

    if L_50.all() < 0:
        PL = Gt_dBi + Pt_dB + L_50

    elif L_50.all() > 0:
        PL = Gt_dBi + Pt_dB - L_50

    plt.plot(d, PL)
    plt.xlabel('Distância em kilômetros')
    plt.ylabel('Sinal recebido (dB)')
    plt.title('Modelo Okumura-Hata COST 231')
    plt.grid(True)

    plt.show()


# função principal
acabou = 0

while acabou == 0:
    print("Digite qual modelo de propagação deseja calcular:")
    opcao = int(
        input(
            "Modelo espaço livre - 1\nModelo log-normal - 2\nTwo Ray model - 3\n"
            "Modelo Okumura Hata - 4\nModelo Hata Modificado - 5\nModelo Okumura/Hata-Davidson - 6\n"
            "Modelo_Okumura_Hata_Cost_231 - 7\n"            
        )
    )

    print("\n\n")

    if opcao == 1:
        espaco_livre()

    elif opcao == 2:
        log_normal()

    elif opcao == 3:
        two_ray_model()

    elif opcao == 4:
        modelo_Okumura_Hata()

    elif opcao == 5:
        Hata_modified() 

    elif opcao == 6 :
        modelo_Okumura_Hata_Davidson()

    elif opcao == 7:
        modelo_Okumura_Hata_Cost_231()

    acabou = int(input("Tecle 0 para calcular novamente ou 1 para encerrar: "))
    # fim da função principal
