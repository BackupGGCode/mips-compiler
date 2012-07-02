#ifndef MIPS_H_INCLUDED
#define MIPS_H_INCLUDED

#define maxInstrucoes 64


/***************************************************************************************/
/********************************* Variaveis Globais ***********************************/
/***************************************************************************************/

unsigned int mem[maxInstrucoes];
int PC;             // Program counter
int reg[32];        // Registradores

/**
** Procedimento executaInstrucoes
** Procedimento que starta a execu��o das instru��es de acordo com o modo de execu��o desejado
**/
void executaInstrucoes(int qntInstrucoes,int modoExecucao);

/**
** Procedimento resetaRegistradoresMemoria
** Reseta os valores contidos nos registradores
**/
void resetaRegistradoresMemoria();

#endif // MIPS_H_INCLUDED
