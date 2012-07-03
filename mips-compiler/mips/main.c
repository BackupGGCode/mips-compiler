#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arquivos.h"
#include "layout.h"
#include "mips.h"

/**
** Procedimento leInstrucoes
** Verifica se o arquivo foi lido corretamente
** Tamb�m faz a chamada para execu��o das instru��es contidas no arquivo
**/
void leInstrucoes(char* nomeArq){
    /** Vari�veis locais **/
    int qnt = leArquivoModo(nomeArq,"r+");  // Verifica se arquivo foi lido

    if(qnt == 0){   // Se n�o leu o arquivo corretamente
        printf("Corrija o arquivo!!\n");
    }else{          // Se leu o arquivo corretamente
        wait(0.5);

        int opc = execucaoMenu();   // Exibe o menu e recupera op��o escolhida
        if(opc == 1 || opc == 2){   // Se op��o � v�lida
            limpaPrompt();  // Limpa tela
            executaInstrucoes(qnt,opc); // Inicia execu��o em um dos modos v�lidos
        }else{
            printf("Opcao invalida!\n");
            wait(0.5);
        }
    }
}

/**
** Procedimento realizaLeitura
** Realiza a leitura e chama a execu��o de arquivos
**/
void realizaLeitura(){
    /** Variaveis locais **/
    char nomeArq[20];      // Armazena o nome do arquivo
    char testaArq[20];     // Vari�vel para testar se � txt
    char continua;         // Vari�vel para verificar se operador deseja continuar

    do{
        limpaPrompt();  // Limpa a tela
        printf("Digite o nome do arquivo a ser lido:\nArquivo: ");
        gets(nomeArq);  // Pega, via teclado, o nome do arquivo que se deseja gravar
        strcpy(testaArq,nomeArq);   // Copia nome do arquivo para testar se � txt

        if(!terminaCom(testaArq,".","txt")) //verifica se � txt
        {
            // Caso n�o seja, pede um novo arquivo que seja txt
            printf("Nao eh um txt!!!\n");
            continua = 'y';
            wait(0.5);
            continue;
        }

        leInstrucoes(nomeArq);

        // Verifica se deseja ler outro arquivo
        printf("\nLer outro arquivo?(y ou n)? ");
        scanf("%c%*c",&continua);
    }while(continua == 'y' || continua == 'Y' || continua == '1' || continua == 's' || continua =='S');
}

/**
** Procedimento realizaGravacao
** Realiza a grava��o e chama a execu��o de arquivos
**/
void realizaGravacao(){
    /** Vari�veis locais **/
    char nomeArq[20];      // Armazena o nome do arquivo
    char testaArq[20];     // Vari�vel para testar se � txt
    char continua;         // Vari�vel para verificar se operador deseja continuar

    do{
        limpaPrompt();  // Limpa a tela
        printf("Digite o nome do arquivo a ser gravado:\nArquivo: ");
        gets(nomeArq);  // Pega, via teclado, o nome do arquivo que se deseja gravar
        strcpy(testaArq,nomeArq);   // Copia nome do arquivo para testar se � txt

        if(!terminaCom(testaArq,".","txt")) //verifica se � txt
        {
            // Caso n�o seja, pede um novo arquivo que seja txt
            printf("\nNao eh um txt!!!");
            continua = 'y';
            wait(0.5);
            continue;
        }

        do{
            limpaPrompt();      // Limpa a tela
            char texto[100];    // Vari�vel que pega o texto a ser gravado no arquivo

            printf("Digite o texto a ser gravado em %s:\nTexto: ",nomeArq);
            gets(texto);
            strcat(texto,"\n\0");

            gravaArquivoModo(nomeArq,"a+",texto);   // Grava o texto no arquivo

            // Verifica se deseja continuar gravando nesse arquivo
            printf("\nContinuar gravando?(y ou n)? ");
            scanf("%c%*c",&continua);
        }while(continua == 'y' || continua == 'Y' || continua == '1' || continua == 's' || continua =='S');

        leInstrucoes(nomeArq);  // chama o procedimento de execu��o do c�digo presente no arquivo

        // Verifica se deseja gravar em outro arquivo
        printf("Gravar em outro arquivo?(y ou n)? ");
        scanf("%c%*c",&continua);
    }while(continua == 'y' || continua == 'Y' || continua == '1' || continua == 's' || continua =='S');
}

/**
** Fun��o resetMem
** Chama procedimento de RESET dos registradores e mem�ria
**/
void resetaMem(){
    resetaRegistradoresMemoria();   // fun��o que realmente reseta (mips.c)
}

/**
** Fun��o principal
** Inicia a execu��o chamando o menu
**/
int main(){
    /** Variaveis locais **/
    char continua;   // Vari�vel para verificar se o operador deseja continuar
    int opc;         // Vari�vel que salva a a��o a ser executada

    do{
        opc = selecionaMenu();  // Chama o menu (layout.c)
        switch(opc) // Testa a vari�vel de op��o
        {
            case 1: // Ler um arquivo
                realizaLeitura();
                break;

            case 2: // Entrada via teclado
                realizaGravacao();
                break;

            case 3: // Resetar mem�ria/registradores
            {
                resetaMem();
                printf("Memoria resetada!");
                wait(0.5);

                // Avan�a para pedir uma nova op��o
                continua = 'y';
                continue;
            }break;

            case 4: // Sair
                return 0;
            default:// Op��o selecionada n�o existe
            {
                printf("Opcao invalida!");
                wait(0.5);

                // Avan�a para pedir uma nova op��o
                continua = 'y';
                continue;
            }
        }
        printf("\nDeseja continuar(y ou n)? ");
        scanf("%c%*c",&continua);
    }while(continua == 'y' || continua == 'Y' || continua == '1' || continua == 's' || continua =='S');

    return 0;
}
