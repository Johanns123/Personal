/**
 * @Author: johannamorim
 * @Date:   2021-07-01T20:56:35-03:00
 * @Last modified by:   johannamorim
 * @Last modified time: 2022-04-01-16:49
 */

// Versão 0.0.2

/*Bug fixes:
  -Não há mais repetições de nomes na mesma semana*/


//melhorias futuras: Deixar o código mais organizado e reduzir o número de V. globais


/*Bibliotecas*/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
//este código destina-se a gerar uma tabela com o nome da pessoa e os dias em que o seu nome está associado utilizando
//uma aleatoriedade dos números

/*Variáveis globais*/
int dia, mes, ano, mes2, ano2;
unsigned int sabado = 0, quarta = 0;
int dias_desde_domingo;
int correcao = 0;
int selecao = 0;
FILE *file;   //crio um ponteiro para a variável FILE
char *nomes [10] = {"Gabriel",
                    "Josef",
                    "Custodio",
                    "Leonardo",
                    "Jonathan",
                    "Lucas",
                    "Alexsandro",
                    "Natha",
                    "Rodrigo",
                    "Johann"}; //matriz de strings/caracateres
//Outra forma de representar é por: char nomes[10] [30] em que as linhas são os nomes e as colunas o conjunto de caracteres contando com o caractere '\0'

/*Protótipo das funções*/
int numero(int num);
void numeracao_dos_dias(int i);
int sorteio_de_nomes(int j, int num);
void escreve_nomes(int num, char opt);
void nao_repete_por_numero_de_semanas(int numero_aleatorio, int j, int num);

int main(){
    setlocale(LC_ALL,"Portuguese_Brazil");
    setlocale(LC_NUMERIC,"C");


    srand((unsigned) time(NULL));

    printf("********************************************************\n");
    printf("************Gerador de tabelas v. 1.0.1*****************\n");
    printf("Olá, este é o programa gerador de designações do zoom!\n");

    printf("Digite o número de semanas para montar a escala e pressione enter:\n");
    scanf("%d", &selecao);


    file = fopen("tabela_zoom.csv", "w");   //ponteiro file apontando para uma função que manipula arquivos
    //Determino o diretório onde vai ser gerado o arquivo e o que eu desejo fazer com ele, que é escrita.


    time_t data_tempo;
    time(&data_tempo);
    struct tm *data = localtime(&data_tempo);
    //tempo do computador no momento em que o programa for executado

    //printf("Dias desde Janeiro -> %d\n", data->tm_yday);
    //configuração da data em que o programa está sendo executado

    dia = data->tm_mday;                        //dia do mês
    mes = data->tm_mon + 1;                     //mês atual
    mes2 = data->tm_mon + 1;                     //mês atual
    ano = data->tm_year + 1900;                 //ano atual
    ano2 = data->tm_year +1900;                 //ano atual
    dias_desde_domingo = data->tm_wday;     //dias desde domingo

    fprintf(file, "      Dias,Indicador,Áudio,Vídeo,Suporte              \n");

    for(int i = 0; i < (selecao); i++)
    {

        static int retorno = 0;
        static char contador = 0;

        numeracao_dos_dias(i);

        if(quarta < sabado || (mes < mes2 || (mes2 != 1  || mes != 1)))
        {
            fprintf(file, "%2d/%2d/%d,", quarta, mes, ano);
            retorno = 0;
            escreve_nomes(10,correcao);
            //contador++;
            correcao = 1;
        }

        else
        {
            correcao = 1;
            fprintf(file, "%2d/%2d/%d,", sabado, mes2, ano2);
            retorno = 1;
            escreve_nomes(10,correcao);
            contador++;
        }


        fprintf(file, "\n");

        if(!retorno)
        {
            fprintf(file, "%2d/%2d/%d,", sabado, mes2, ano2);
            escreve_nomes(10,correcao);
            //contador++;
        }
        else
        {
            fprintf(file, "%2d/%2d/%d,", quarta, mes, ano);
            escreve_nomes(10,correcao);
            //contador++;
        }

        fprintf(file, "\n");
    }


  fclose(file);  //fecha/salva o arquivo.txt

  printf("Tabela gerada com sucesso!!!\nVocê pode acessá-la na mesma pasta deste programa num documento \"csv\".\n");       //versão linux
  //printf("Tabela gerada com sucesso!!!\nVocê pode acess\xA0-la na mesma pasta deste programa num documento \"txt\".\n");    //versão windows


  getchar();
  return 0;

}//end main


/*Desenvolvimento das funções*/
int numero(int num){
    int random = rand() % num;  //numero vai de 0 a 5

    return random;
}


void numeracao_dos_dias(int i)
{
    switch (dias_desde_domingo)
    {   /*Sequência de ifs e elses nos cases para determinar os dias da semana*/
        case 0:
            //domigo
            if(i < 1)
            {
                quarta += (dia + (3 * (i+1)));
            }
            else if(i >= 1)
            {
                quarta += 7;
            }
            if(i < 1)
            {
                sabado += (dia + (6 * (i+1)));
            }
            else
            {
                sabado += 7;
            }
            break;

        case 1:
            //segunda
            if(i < 1)
            {
                quarta += (dia + 2);
            }
            else if(i >= 1)
            {
                quarta += 7;
            }
            if(i < 1)
            {
                sabado += (dia + 5);
            }
            else
            {
                sabado += 7;
            }
            break;

        case 2:
            //terça
            if(i < 1)
            {
                quarta += (dia + 1);
            }
            else if(i >= 1)
            {
                quarta += 7;
            }
            if(i < 1)
            {
                sabado += (dia + 4);
            }
            else
            {
                sabado += 7;
            }
            break;

        case 3:
            //quarta
            if(i < 1)
            {
                quarta += (dia);
            }
            else if(i >= 1)
            {
                quarta += 7;
            }
            if(i < 1)
            {
                sabado += (dia + 3);
            }
            else
            {
                sabado += 7;
            }
            break;

        case 4:
            //quinta
            if(i < 1)
            {
                quarta += (dia + 6);
            }
            else if(i >= 1)
            {
                quarta += 7;
            }
            if(i < 1)
            {
                sabado += (dia + 2);
            }
            else
            {
                sabado += 7;
            }
            break;

        case 5:
            //sexta
            if(i < 1)
            {
                quarta += (dia + 5);
            }
            else if(i >= 1)
            {
                quarta += 7;
            }
            if(i < 1)
            {
                sabado += (dia + 1);
            }
            else
            {
                sabado += 7;
            }
            break;

        case 6:
            //sábado*/
            if(i < 1)
            {
                quarta += (dia + 4);
            }
            else if(i >= 1)
            {
                quarta += 7;
            }
            if(i < 1)
            {
                sabado += (dia);
            }
            else
            {
                sabado += 7;
            }
            break;
    }//end switch

    if((mes == 1 || mes == 3 || mes == 5 || mes == 7 || mes == 8 || mes == 10 || mes == 12) && (quarta > 31 || sabado > 31))                                //lógica para não exceder 31 dias
        {
            if(quarta > 31)
            {

                quarta -= 31;
                mes++;
            }
            if(mes > 12)
            {
                mes = 1;
                ano++;

            }
            else if(quarta == 31 || sabado == 31);
        }

    else if((mes == 4 || mes == 6 || mes == 9 || mes == 11)  && (quarta > 30 || sabado > 30))    //lógica para não exceder 30 dias
    {
        if(quarta > 30)
        {

            quarta -= 30;
            mes++;
        }
        else if(quarta == 30 || sabado == 30);
    }

    else if(mes == 2 && (quarta > 28 || sabado > 28))                                           //lógica par não exceder 28 dias
    {
        if(quarta > 28)
        {

            quarta -= 28;
            mes++;
        }
        else if(quarta == 28 || sabado == 28);
    }

    if((mes2 == 1 || mes2 == 3 || mes2 == 5 || mes2 == 7 || mes2 == 8 || mes2 == 10 || mes2 == 12) && (quarta > 31 || sabado > 31))                                //lógica para não exceder 31 dias
        {
            if(sabado > 31)
            {
                sabado -= 31;
                mes2++;
            }
            if(mes2 > 12)
            {
                mes2 = 1;
                ano2++;

            }

            else if(quarta == 31 || sabado == 31);

            //counter++;
        }

    else if((mes2 == 4 || mes2 == 6 || mes2 == 9 || mes2 ==11)  && (quarta > 30 || sabado > 30))    //lógica para não exceder 30 dias
    {
        if(sabado > 30)
        {
            sabado -= 30;
            mes2++;
        }
        else if(quarta == 30 || sabado == 30);
    }

    else if(mes2 == 2 && (quarta > 28 || sabado > 28))                                           //lógica par não exceder 28 dias
    {
        if(sabado > 28)
        {
            sabado -= 28;
            mes2++;
        }
        else if(quarta == 28 || sabado == 28);
    }

}//end numeracao_dos_dias

int sorteio_de_nomes(int j, int num)
{
    static int numero1  = 0;
    static int numero2  = 0;
    static int numero3  = 0;
    static int numero4  = 0;

    int numero_aleatorio;
    numero_aleatorio = numero(num);
    while(numero_aleatorio == numero1 || numero_aleatorio == numero2
    || numero_aleatorio == numero3 || numero_aleatorio == numero4) numero_aleatorio = numero(num);

    if(j == 0) numero1 = numero_aleatorio;
    if(j == 1) numero2 = numero_aleatorio;
    if(j == 2) numero3 = numero_aleatorio;
    if(j == 3) numero4 = numero_aleatorio;

    return numero_aleatorio;

}

/*Fazer a leitura do arquivo e verificar as strings procurando uma forma de mapear o arquivo em uma matriz*/
void escreve_nomes(int num, char opt)
{
    static char *linha0 [10] = {0};     //vetores de string para comparação entre as designações de dias da semana iguais mas em semanas diferentes
    static char *linha2 [10] = {0};
    static char *linha4 [10] = {0};
    static char *linha1 [10] = {0};     //vetores de string para comparação entre as designações de dias da semana iguais mas em semanas diferentes
    static char *linha3 [10] = {0};
    static char *linha5 [10] = {0};

    static int indice = 0;

    for(int j = 0; j < 4; j++)
    {

        static int numero_aleatorio = 0;
        

        numero_aleatorio = sorteio_de_nomes(j, num);


        nao_repete_por_numero_de_semanas(numero_aleatorio, j, num);

        if(!indice)
        {

            linha0 [j] = nomes[numero_aleatorio];
            
        }

        if(indice == 2)
        {
            linha2 [j] = nomes[numero_aleatorio];
            if(linha2 [j] == linha0[j])
            {
                printf("linha0: %s\tlinha2: %s\n", linha0[j], linha2[j]);
                numero_aleatorio = sorteio_de_nomes(j, num);
                linha2 [j] = nomes[numero_aleatorio];
            }
        }

        if(indice == 4)
        {
            linha4 [j] = nomes[numero_aleatorio];
            if(linha4 [j] == linha2[j])
            {
                printf("linha4: %s\tlinha2:%s\n", linha4[j], linha2[j]);
                numero_aleatorio = sorteio_de_nomes(j, num);
                linha4 [j] = nomes[numero_aleatorio];
            }
        }

        if(indice == 1)
        {
            linha0 [j] = nomes[numero_aleatorio];
        }


        if(indice == 3)
        {
            linha3 [j] = nomes[numero_aleatorio];
            if(linha3 [j] == linha1[j])
            {
                printf("linha3: %s\tlinha1: %s\n", linha3[j], linha1[j]);
                numero_aleatorio = sorteio_de_nomes(j, num);
                linha3 [j] = nomes[numero_aleatorio];
            }
        }

        if(indice == 5)
        {
            linha5 [j] = nomes[numero_aleatorio];
            if(linha5 [j] == linha3[j])
            {
                printf("linha5: %s\tlinha3: %s\n", linha5[j], linha3[j]);
                numero_aleatorio = sorteio_de_nomes(j, num);
                linha5 [j] = nomes[numero_aleatorio];
            }
        }

        fprintf(file, "%10s,", nomes[numero_aleatorio]);
    
    }

    indice++;   //depois que é feito a impressão de uma linha

    if(indice > 6)  indice = 0; //reinicio a análise dos ídices

}

void nao_repete_por_numero_de_semanas(int numero_aleatorio, int j, int num)
{
    static int counter, counter1, counter2, counter3, counter4, counter5, counter6, counter7, counter8, counter9;
    switch(numero_aleatorio)
        {
            case 0:
            if(counter > selecao) numero_aleatorio = sorteio_de_nomes(j, num);
            counter++;
            break;

            case 1:
            if(counter1 > selecao) numero_aleatorio = sorteio_de_nomes(j, num);
            counter1++;
            break;

            case 2:
            if(counter2 > selecao) numero_aleatorio = sorteio_de_nomes(j, num);
            counter2++;
            break;

            case 3:
            if(counter3 > selecao) numero_aleatorio = sorteio_de_nomes(j, num);
            counter3++;
            break;

            case 4:
            if(counter4 > selecao) numero_aleatorio = sorteio_de_nomes(j, num);
            counter4++;
            break;

            case 5:
            if(counter5 > selecao) numero_aleatorio = sorteio_de_nomes(j, num);
            counter5++;
            break;

            case 6:
            if(counter6 > selecao) numero_aleatorio = sorteio_de_nomes(j, num);
            counter6++;
            break;

            case 7:
            if(counter7 > selecao) numero_aleatorio = sorteio_de_nomes(j, num);
            counter7++;
            break;

            case 8:
            if(counter8 > selecao) numero_aleatorio = sorteio_de_nomes(j, num);
            counter8++;
            break;

            case 9:
            if(counter9 > selecao) numero_aleatorio = sorteio_de_nomes(j, num);
            counter9++;
            break;

        }
}