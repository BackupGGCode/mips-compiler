#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "layout.h"

/**
** Procedimento para pausar um tempo(segundos float)
**/
void wait ( float segundos )
{
    /* Variaveis locais */
    clock_t endwait  = clock () + segundos * CLOCKS_PER_SEC ;

    //Trava o prompt por alguns segundos
    while (clock() < endwait);
}


/**
** Procedimento para limpar o prompt
**/
void limpaPrompt()
{
    //Limpa o prompt
    system("cls");      // windows
    //system("clear");  //linus

}



/**
** Função que mostra o menu
** Retorna(int)
   opção selecionada
**/
int selecionaMenu()
{
    /* Variaveis locais */
    int opcao; // Opcao selecionada

    limpaPrompt();

    //Menu e leitura
    printf("Escolha uma opcao:\n");
    printf("1--> Ler um arquivo\n");
    printf("2--> Entrada via teclado\n");
    printf("3--> Reset memoria/registradores\n");
    printf("4--> Sair\n");
    printf("opcao: ");
    scanf("%d%*c",&opcao);
    return opcao;
}

/**
** Função que mostra o menu de modo de execucao
** Retorna(int)
   opção selecionada
**/
int execucaoMenu()
{
    /* Variaveis locais */
    int opcao; // Opcao selecionada
    //Menu e leitura
    printf("Modo de execucao:\n");
    printf("1--> Passo-a-Passo\n");
    printf("2--> Automatico\n");
    printf("opcao: ");
    scanf("%d%*c",&opcao);
    return opcao;
}
