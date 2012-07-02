#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arquivos.h"
#include "layout.h"
#include "mips.h"


void leInstrucoes(char* nomeArq){
    int qnt = leArquivoModo(nomeArq,"r+");
    //Se não leu o arquivo
    if(qnt == 0)
    {
        printf("Corrija o arquivo!!\n");
    }else{
        wait(0.5);

        int opc = execucaoMenu();
        if(opc == 1 || opc == 2){
            limpaPrompt();
            executaInstrucoes(qnt,opc);
        }else{
            printf("Opcao invalida!\n");
            wait(0.5);
        }
    }
}
/**
** Procedimento para realizar a leitura
**/
void realizaLeitura()
{
    /* Variaveis locais */
    char nomeArq[20];      //armazena o nome do arquivo
    char testaArq[20];     //testar se é txt
    char continua;         //deseja continuar
    do
    {
        limpaPrompt();
        printf("Digite o nome do arquivo a ser lido:\nArquivo: ");
        gets(nomeArq);

        strcpy(testaArq,nomeArq);
        //verifica se é txt
        if(!terminaCom(testaArq,".","txt"))
        {
            printf("Nao eh um txt!!!\n");
            continua = 'y';
            wait(0.5);
            continue;
        }

        leInstrucoes(nomeArq);
        //Verifica se deseja ler outro arquivo
        printf("\nLer outro arquivo?(y ou n)? ");
        scanf("%c%*c",&continua);
    }
    while(continua == 'y' || continua == 'Y' || continua == '1' || continua == 's' || continua =='S');
}


/**
** Procedimento para realizar a gravação
**/
void realizaGravacao()
{
    /* Variaveis locais */
    char nomeArq[20];      //armazena o nome do arquivo
    char testaArq[20];     //testar se é txt
    char continua;         //deseja continuar
    do
    {
        limpaPrompt();
        printf("Digite o nome do arquivo a ser gravado:\nArquivo: ");
        gets(nomeArq);
        strcpy(testaArq,nomeArq);
        //verifica se é txt
        if(!terminaCom(testaArq,".","txt"))
        {
            printf("\nNao eh um txt!!!");
            continua = 'y';
            wait(0.5);
            continue;
        }
        do
        {
            limpaPrompt();
            char texto[100];
            printf("Digite o texto a ser gravado em %s:\nTexto: ",nomeArq);
            gets(texto);
            strcat(texto,"\n\0");
            gravaArquivoModo(nomeArq,"a+",texto);

            //Verifica se deseja continuar gravando nesse arquivo
            printf("\nContinuar gravando?(y ou n)? ");
            scanf("%c%*c",&continua);

        }
        while(continua == 'y' || continua == 'Y' || continua == '1' || continua == 's' || continua =='S');

        leInstrucoes(nomeArq);
        //Verifica se deseja gravando em outro arquivo
        printf("Gravar em outro arquivo?(y ou n)? ");
        scanf("%c%*c",&continua);
    }
    while(continua == 'y' || continua == 'Y' || continua == '1' || continua == 's' || continua =='S');

}

void resetaMem(){
    resetaRegistradoresMemoria();
}
/**
** Função principal
**/
int main()
{
    /* Variaveis locais */
    char continua;   //deseja continuar
    int opc;         //acao a ser feita
    do
    {
        opc = selecionaMenu();
        switch(opc)
        {
        case 1:
            realizaLeitura();
            break;
        case 2:
            realizaGravacao();
            break;
        case 3:{
            resetaMem();
            printf("Memoria resetada!");
            wait(0.5);
            continua = 'y';
            continue;
        }
        break;
        case 4:
            return 0;
        default :
        {
            printf("Opcao invalida!");
            wait(0.5);
            continua = 'y';
            continue;
        }
        }

        printf("\nDeseja continuar(y ou n)? ");
        scanf("%c%*c",&continua);
    }
    while(continua == 'y' || continua == 'Y' || continua == '1' || continua == 's' || continua =='S');
    return 0;
}
