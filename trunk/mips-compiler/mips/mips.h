#ifndef MIPS_H_INCLUDED
#define MIPS_H_INCLUDED

#define maxInstrucoes 64
#define tamanhoMemoriaDados 256     //em bytes

/***************************************************************************************/
/********************************* Variaveis Globais ***********************************/
/***************************************************************************************/

unsigned int mem[maxInstrucoes];                        // Memória de instruções
unsigned int mem_dados[tamanhoMemoriaDados/4];          // Memória de dados     OBS.: 256/4 posições = 64 words = 4x64 bytes = 256 bytes

int PC;             // Program counter
int reg[32];        // Registradores

/**
** Procedimento executaInstrucoes
** Procedimento que starta a execução das instruções(no total de qntInstrucoes int)
** de acordo com o modo de execução desejado(int modoExecucao)
**/
void executaInstrucoes(int qntInstrucoes,int modoExecucao);

/**
** Procedimento resetaRegistradoresMemoria
** Reseta os valores contidos nos registradores
**/
void resetaRegistradoresMemoria();

#endif // MIPS_H_INCLUDED
