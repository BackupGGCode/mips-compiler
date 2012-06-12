#ifndef MIPS_H_INCLUDED
#define MIPS_H_INCLUDED

#define maxInstrucoes 64


/***************************************************************************************/
/********************************* Variaveis Globais ***********************************/
/***************************************************************************************/

unsigned int mem[maxInstrucoes];
int PC;
int reg[32];      //Registradores

double lo;
double hi;

void executaInstrucoes(int qntInstrucoes);

#endif // MIPS_H_INCLUDED
