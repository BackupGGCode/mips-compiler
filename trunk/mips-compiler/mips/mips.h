#ifndef MIPS_H_INCLUDED
#define MIPS_H_INCLUDED

#define maxInstrucoes 64


/***************************************************************************************/
/********************************* Variaveis Globais ***********************************/
/***************************************************************************************/

unsigned int mem[maxInstrucoes];    // Memória de instruções
unsigned int mem_dados[64];         // Memória de dados     OBS.: 64 posições = 64 words = 4x64 bytes = 256 bytes

int PC;             // Program counter
int reg[32];        // Registradores

/**
** Procedimento executaInstrucoes
** Procedimento que starta a execução das instruções de acordo com o modo de execução desejado
**/
void executaInstrucoes(int qntInstrucoes,int modoExecucao);

/**
** Procedimento resetaRegistradoresMemoria
** Reseta os valores contidos nos registradores
**/
void resetaRegistradoresMemoria();

#endif // MIPS_H_INCLUDED
