#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "layout.h"

/**
** Procedimento wait
** Pausa a execu��o por um determinado tempo (segundos float)
**/
void wait ( float segundos )
{
    /* Vari�veis locais */
    clock_t endwait  = clock() + segundos * CLOCKS_PER_SEC ;

    //Trava o prompt por alguns segundos
    while (clock() < endwait);
}

/**
** Procedimento limpaPrompt
** Limpa o prompt
**/
void limpaPrompt()
{
    //Limpa o prompt
    system("cls");      // windows
    //system("clear");  //linux
}

/**
** Fun��o selecionaMenu
** Mostra o menu e recupera a op��o escolhida
** Retorna(int)
   op��o selecionada
**/
int selecionaMenu()
{
    /* Vari�veis locais */
    int opcao; // Vari�vel que guarda a op��o selecionada no menu

    limpaPrompt();

    //Menu
    printf("Escolha uma opcao:\n");
    printf("1--> Ler um arquivo\n");
    printf("2--> Entrada via teclado\n");
    printf("3--> Reset memoria/registradores\n");
    printf("4--> Sair\n");
    printf("opcao: ");

    scanf("%d%*c",&opcao);  // Leitura da op��o selecionada
    return opcao;
}

/**
** Fun��o execucaoMenu
** Mostra o menu de modo de execucao e recupera a op��o escolhida
** Retorna(int)
   op��o selecionada
**/
int execucaoMenu()
{
    /* Vari�veis locais */
    int opcao; // Vari�vel que guarda a op��o selecionada no menu

    //Menu
    printf("Modo de execucao:\n");
    printf("1--> Passo-a-Passo\n");
    printf("2--> Automatico\n");
    printf("opcao: ");

    scanf("%d%*c",&opcao);  // Leitura da op��o selecionada
    return opcao;
}
