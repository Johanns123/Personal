/*
 * File:   mapping.c
 * Author: johannsamorim
 *
 * Created on 4 de Fevereiro de 2022, 17:15
 */
#include "Running.h"
#include "dados.h"      // verificar depois o uso dessa biblioteca em running.c

/*Em desenvolvimento*/
void Running_Torricielli(void)
{
    #define trechos_totais 10
    extern bool flag_curva;
    static unsigned char trecho = 0;                            // indica qual trecho o robo está
    static unsigned char distancia_atual      = 0,              // dados encontrados pelos enconders
                         distancia_restante   = 0,
                         distancia_necessaria = 0;

    static unsigned char v_atual = 0, v_final = 0, a = 0;         // velocidades e acelerações
    
    extern unsigned char distancia_total[trechos_totais];
    extern unsigned char MaxSpeed[trechos_totais];
    extern unsigned char valor_de_tolerancia[trechos_totais];  // distancia de tolerancia para o robô começar a frear

    v_atual = dados_velocid_linear();   //velocidade atual
    v_final = MaxSpeed[trecho]; //guardada pelo mapeamento
    
    
    //distancia_atual = distancia do encoder;
    distancia_restante = distancia_total[trecho] - distancia_atual;
    distancia_necessaria = ((v_final * v_final) - (v_atual * v_atual)) / (2*a);
    
    
    if(distancia_necessaria < 0)    distancia_necessaria = 0;   //evita que seja negativo
    
    if(distancia_necessaria < (distancia_restante - valor_de_tolerancia[trecho]))
    //robô ainda não chegou no momento de reduzir a velocidade
    {
        //colocamos em velocidade máxima do trecho
        /*Necessario criar função de conversão de vel. para duty*/
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


