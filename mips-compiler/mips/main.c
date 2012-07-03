#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arquivos.h"
#include "layout.h"
#include "mips.h"

/**
** Procedimento leInstrucoes
** Verifica se o arquivo foi lido corretamente
** Também faz a chamada para execução das instruções contidas no arquivo
**/
void leInstrucoes(char* nomeArq){
    /** Variáveis locais **/
    int qnt = leArquivoModo(nomeArq,"r+");  // Verifica se arquivo foi lido

    if(qnt == 0){   // Se não leu o arquivo corretamente
        printf("Corrija o arquivo!!\n");
    }else{          // Se leu o arquivo corretamente
        wait(0.5);

        int opc = execucaoMenu();   // Exibe o menu e recupera opção escolhida
        if(opc == 1 || opc == 2){   // Se opção é válida
            limpaPrompt();  // Limpa tela
            executaInstrucoes(qnt,opc); // Inicia execução em um dos modos válidos
        }else{
            printf("Opcao invalida!\n");
            wait(0.5);
        }
    }
}

/**
** Procedimento realizaLeitura
** Realiza a leitura e chama a execução de arquivos
**/
void realizaLeitura(){
    /** Variaveis locais **/
    char nomeArq[20];      // Armazena o nome do arquivo
    char testaArq[20];     // Variável para testar se é txt
    char continua;         // Variável para verificar se operador deseja continuar

    do{
        limpaPrompt();  // Limpa a tela
        printf("Digite o nome do arquivo a ser lido:\nArquivo: ");
        gets(nomeArq);  // Pega, via teclado, o nome do arquivo que se deseja gravar
        strcpy(testaArq,nomeArq);   // Copia nome do arquivo para testar se é txt

        if(!terminaCom(testaArq,".","txt")) //verifica se é txt
        {
            // Caso não seja, pede um novo arquivo que seja txt
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
** Realiza a gravação e chama a execução de arquivos
**/
void realizaGravacao(){
    /** Variáveis locais **/
    char nomeArq[20];      // Armazena o nome do arquivo
    char testaArq[20];     // Variável para testar se é txt
    char continua;         // Variável para verificar se operador deseja continuar

    do{
        limpaPrompt();  // Limpa a tela
        printf("Digite o nome do arquivo a ser gravado:\nArquivo: ");
        gets(nomeArq);  // Pega, via teclado, o nome do arquivo que se deseja gravar
        strcpy(testaArq,nomeArq);   // Copia nome do arquivo para testar se é txt

        if(!terminaCom(testaArq,".","txt")) //verifica se é txt
        {
            // Caso não seja, pede um novo arquivo que seja txt
            printf("\nNao eh um txt!!!");
            continua = 'y';
            wait(0.5);
            continue;
        }

        do{
            limpaPrompt();      // Limpa a tela
            char texto[100];    // Variável que pega o texto a ser gravado no arquivo

            printf("Digite o texto a ser gravado em %s:\nTexto: ",nomeArq);
            gets(texto);
            strcat(texto,"\n\0");

            gravaArquivoModo(nomeArq,"a+",texto);   // Grava o texto no arquivo

            // Verifica se deseja continuar gravando nesse arquivo
            printf("\nContinuar gravando?(y ou n)? ");
            scanf("%c%*c",&continua);
        }while(continua == 'y' || continua == 'Y' || continua == '1' || continua == 's' || continua =='S');

        leInstrucoes(nomeArq);  // chama o procedimento de execução do código presente no arquivo

        // Verifica se deseja gravar em outro arquivo
        printf("Gravar em outro arquivo?(y ou n)? ");
        scanf("%c%*c",&continua);
    }while(continua == 'y' || continua == 'Y' || continua == '1' || continua == 's' || continua =='S');
}

/**
** Função resetMem
** Chama procedimento de RESET dos registradores e memória
**/
void resetaMem(){
    resetaRegistradoresMemoria();   // função que realmente reseta (mips.c)
}

/**
** Função principal
** Inicia a execução chamando o menu
**/
int main(){
    /** Variaveis locais **/
    char continua;   // Variável para verificar se o operador deseja continuar
    int opc;         // Variável que salva a ação a ser executada

    do{
        opc = selecionaMenu();  // Chama o menu (layout.c)
        switch(opc) // Testa a variável de opção
        {
            case 1: // Ler um arquivo
                realizaLeitura();
                break;

            case 2: // Entrada via teclado
                realizaGravacao();
                break;

            case 3: // Resetar memória/registradores
            {
                resetaMem();
                printf("Memoria resetada!");
                wait(0.5);

                // Avança para pedir uma nova opção
                continua = 'y';
                continue;
            }break;

            case 4: // Sair
                return 0;
            default:// Opção selecionada não existe
            {
                printf("Opcao invalida!");
                wait(0.5);

                // Avança para pedir uma nova opção
                continua = 'y';
                continue;
            }
        }
        printf("\nDeseja continuar(y ou n)? ");
        scanf("%c%*c",&continua);
    }while(continua == 'y' || continua == 'Y' || continua == '1' || continua == 's' || continua =='S');

    return 0;
}
