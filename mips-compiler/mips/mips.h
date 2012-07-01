#ifndef MIPS_H_INCLUDED
#define MIPS_H_INCLUDED

#define maxInstrucoes 64


/***************************************************************************************/
/********************************* Variaveis Globais ***********************************/
/***************************************************************************************/

unsigned int mem[maxInstrucoes];
int PC;
int reg[32];      //Registradores


void executaInstrucoes(int qntInstrucoes,int modoExecucao);

void resetaRegistradoresMemoria();

#endif // MIPS_H_INCLUDED
