/*
 * File:   mapping.c
 * Author: johannsamorim
 *
 * Created on 4 de Fevereiro de 2022, 17:15
 */
#include "Running.h"

/*Em desenvolvimento*/
void Running_Torricielli(void)
{
    #define numero_de_trechos 10
    extern bool flag_curva;
    static unsigned char trecho = 0;    //indica qual trecho o robo está
    static unsigned char distancia_atual      = 0,                  //dados encontrados pelos enconders
                         distancia_total[numero_de_trechos],    //dados encontrados no mapeamento
                         distancia_restante   = 0,
                         distancia_necessaria = 0,
                         valor_de_tolerancia[numero_de_trechos];  //distancia de tolerancia para o robô começar a frear

    static unsigned char v_atual = 0, v_final = 0, a = 0;   //velocidades e acelerações
    
    static unsigned char MaxSpeed[numero_de_trechos];    //velocidade máxima de cada trecho
    
    //v_atual = velocidade medida pelo encoder
    v_final = MaxSpeed[trecho]; //guardada pelo mapeamento
    
    
    //distancia_atual = distancia do encoder;
    distancia_restante = distancia_total[trecho] - distancia_atual;
    distancia_necessaria = ((v_final * v_final) - (v_atual * v_atual)) / a;
    
    
    if(distancia_necessaria < 0)    distancia_necessaria = 0;   //evita que seja negativo
    
    if(distancia_necessaria < (distancia_restante - valor_de_tolerancia[trecho]))
    //robô ainda não chegou no momento de reduzir a velocidade
    {
        //colocamos em velocidade máxima do trecho
    }
    
    else
    {
        //velocidade no robô em redução ou em um valor menor que em reta
    }
    
    if(flag_curva)
    {
        trecho++;   //atualizo para o próximo trecho
    }
}


