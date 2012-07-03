#ifndef LAYOUT_H_INCLUDED
#define LAYOUT_H_INCLUDED

/**
** Procedimento wait
** Pausa a execu��o por um determinado tempo(segundos float)
**/
void wait ( float segundos );

/**
** Procedimento limpaPrompt
** Limpa o prompt
**/
void limpaPrompt();

/**
** Fun��o selecionaMenu
** Mostra o menu e recupera a op��o escolhida
** Retorna(int)
   op��o selecionada
**/
int selecionaMenu();

/**
** Fun��o execucaoMenu
** Mostra o menu de modo de execu��o e recupera a op��o escolhida
** Retorna(int)
   op��o selecionada
**/
int execucaoMenu();

#endif // LAYOUT_H_INCLUDED
