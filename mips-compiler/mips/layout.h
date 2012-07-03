#ifndef LAYOUT_H_INCLUDED
#define LAYOUT_H_INCLUDED

/**
** Procedimento wait
** Pausa a execução por um determinado tempo(segundos float)
**/
void wait ( float segundos );

/**
** Procedimento limpaPrompt
** Limpa o prompt
**/
void limpaPrompt();

/**
** Função selecionaMenu
** Mostra o menu e recupera a opção escolhida
** Retorna(int)
   opção selecionada
**/
int selecionaMenu();

/**
** Função execucaoMenu
** Mostra o menu de modo de execução e recupera a opção escolhida
** Retorna(int)
   opção selecionada
**/
int execucaoMenu();

#endif // LAYOUT_H_INCLUDED
