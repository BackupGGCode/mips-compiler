#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arquivos.h"
#include "mips.h"


/***************************************************************************************/
/********************************* Variaveis Globais ***********************************/
/***************************************************************************************/

char* nomeArquivo = "log.trojan";   // Nome do arquivo de log
char* modoGravacao = "a";           // Tipo de abertura do arquivo
int operacaoAtual;                  // Valor da operacao atual(serve para a gravacao)

/***************************************************************************************/
/********************************* Estruturas do MIPS **********************************/
/***************************************************************************************/

/**
** Estrutura do Controle
**/
typedef struct
{
    /** Controla o Registrador para Escrita                         */
    /**     Se 0 -- > vem de [20:16]                                */
    /**     Se 1 -- > vem de [15:11]                                */
    int regDst;

    /** Controla a segunda entradas para a ALU                      */
    /**     Se 0 -- > vem do banco de registradores                 */
    /**     Se 1 -- > vem do sinal estendido                        */
    int origAlu;

    /** Controla a origm do PC                                      */
    /**     Se 0 -- > PC = PC + 4                                   */
    /**     Se 1 -- > PC = desvio                                   */
    int origPC;

    /** Controla onde o Valor lido de “Endereço” é colocado         */
    /**     Se 0 -- > nenhum lugar                                  */
    /**     Se 1 -- > colocado em “Dados da Leitura”                */
    int leMem;

    /** Controla de onde o Valor de “Dados para Escrita”            */
    /**     Se 0 -- > vem da ALU                                    */
    /**     Se 1 -- > vem da memória de dados                       */
    int memParaReg;

    /** Controla qual o Valor do “Registrador para Escrita”         */
    /**     Se 0 -- > nenhum                                        */
    /**     Se 1 -- > valor de “Dados para Escrita”                 */
    int escreveReg;

    /** Controla onde o Valor de “Dados para Escrita” é colocado   */
    /**     Se 0 -- > nenhum lugar                                  */
    /**     Se 1 -- > colocado em “Endereço”                        */
    int escreveMem;

    int branchEq;
    int branchNotEq;
    int opAlu1;
    int opAlu0;
    int jump;

    int ctrlAlu;
} Controle;

/**
** Estrutura da ALU
**/
typedef struct
{
    /** Verifica se a diferença das entradas é 0                    */
    /**     Se 0 -- > a diferença dos registradores não é 0         */
    /**     Se 1 -- > a diferença dos registradores é 0             */
    int zeroAlu;

    /** Valor do retorno da ALU                                     */
    int retornoAlu;
} ALU;


/***************************************************************************************/
/******************************* Funções que geram log *********************************/
/***************************************************************************************/

/**
** Funcão que recupera o 'nome' do registrador
** Retorna(char*)
**/
char* recuperaNomeRegistrador(int num)
{
    switch(num)
    {
    case 0:
        return "$zero"; // constant
    case 1:
        return "$at";   // assembler temporary
    case 2:
        return "$v0";   // value for function return and expression evaluation
    case 3:
        return "$v1";   // value for function return and expression evaluation
    case 4:
        return "$a0";   // function argument
    case 5:
        return "$a1";   // function argument
    case 6:
        return "$a2";   // function argument
    case 7:
        return "$a3";   // function argument
    case 8:
        return "$t0";   // temporarie
    case 9:
        return "$t1";   // temporarie
    case 10:
        return "$t2";   // temporarie
    case 11:
        return "$t3";   // temporarie
    case 12:
        return "$t4";   // temporarie
    case 13:
        return "$t5";   // temporarie
    case 14:
        return "$t6";   // temporarie
    case 15:
        return "$t7";   // temporarie
    case 16:
        return "$s0";   // saved temporarie    must preserve
    case 17:
        return "$s1";   // saved temporarie    must preserve
    case 18:
        return "$s2";   // saved temporarie    must preserve
    case 19:
        return "$s3";   // saved temporarie    must preserve
    case 20:
        return "$s4";   // saved temporarie    must preserve
    case 21:
        return "$s5";   // saved temporarie    must preserve
    case 22:
        return "$s6";   // saved temporarie    must preserve
    case 23:
        return "$s7";   // saved temporarie    must preserve
    case 24:
        return "$t8";   // temporarie
    case 25:
        return "$t9";   // temporarie
    case 26:
        return "$k0";   // reserved for OS kernel
    case 27:
        return "$k1";   // reserved for OS kernel
    case 28:
        return "$gp";   // global pointer      must preserve
    case 29:
        return "$sp";   // stack pointer       must preserve
    case 30:
        return "$fp";   // frame pointer       must preserve
    case 31:
        return "$ra";   // return address
    default:
        return "invalido";
    }
    return NULL;
}

/**
** Função que recupera o offset como char* apartir do parametro passado(offset int)
** Retorno
    off char*
**/
char* recuperaOffset(int offset)
{
    char off[20];
    itoa(offset,off,10);
    return off;
}

/**
** Função que completa o vetor de caracter texto ate tam espacos
**/
void completaComEspacos(char* texto,int tam)
{
    int i,tamTxt = strlen(texto);
    if(tamTxt >= tam)
    {
        return;
    }
    for(i= tamTxt; i<=tam; i++)
    {
        strcat(texto," ");
    }
}

/**
** Função para imprimir o cabeçalho do arquivo de log
**/
void imprimeCabecalho()
{
    gravaArquivoModo(nomeArquivo,modoGravacao,"\n------------------------------Inicio da execucao------------------------------\n\n");
    int i;
    gravaArquivoModo(nomeArquivo,modoGravacao,"Operacao//Registradores\tPC\t");
    for(i=1; i<32; i++)
    {
        gravaArquivoModo(nomeArquivo,modoGravacao,recuperaNomeRegistrador(i));
        gravaArquivoModo(nomeArquivo,modoGravacao,"\t");
    }
    gravaArquivoModo(nomeArquivo,modoGravacao,"\n");
}

/**
** Função que imprime o estado dos registradores no arquivo de log
**/
void imprimeRegistradores()
{
    int i;
    gravaArquivoModo(nomeArquivo,modoGravacao,recuperaOffset(PC));
    gravaArquivoModo(nomeArquivo,modoGravacao,"\t");
    for(i=1; i<32; i++)
    {
        gravaArquivoModo(nomeArquivo,modoGravacao,recuperaOffset(reg[i]));
        gravaArquivoModo(nomeArquivo,modoGravacao,"\t");
    }
    gravaArquivoModo(nomeArquivo,modoGravacao,"\n");
}

/** Funcao que imprime a intrucao
** Paramentros
   ope       *char   -- Nome da operacao
   r0        *char   -- Registrador
   r1        *char   -- Registrador
   r2        *char   -- Registrador
   offset    int     -- offset
   posOffset int     --
                      -1  > imprime sem offset
                       0  > imprime com o offset no final
                       1  > imprime com o offset no meio
**/
void recuperaOperacao(char* ope,char* r0,char* r1,char* r2,int offset,int posOffset)
{
    char retorno[20];
    strcpy(retorno,ope);
    strcat(retorno," ");
    switch(posOffset)
    {
    case -1:
    {
        if(strcmp(r0,"") > 0)
        {
            strcat(retorno,r0);
        }
        if(strcmp(r1,"") > 0)
        {
            strcat(retorno,", ");
            strcat(retorno,r1);
        }
        if(strcmp(r2,"") > 0)
        {
            strcat(retorno,", ");
            strcat(retorno,r2);
        }
    }
    break;
    case 0:
    {
        if(strcmp(r0,"") > 0)
        {
            strcat(retorno,r0);
            strcat(retorno,", ");
        }
        if(strcmp(r1,"") > 0)
        {
            strcat(retorno,r1);
            strcat(retorno,", ");
        }
        strcat(retorno,recuperaOffset(offset));
    }
    break;
    case 1:
    {
        strcat(retorno,r0);
        strcat(retorno,", ");
        strcat(retorno,recuperaOffset(offset));
        strcat(retorno,"(");
        strcat(retorno,r1);
        strcat(retorno,")");
    }
    break;
    case 2:
    {
        strcat(retorno,recuperaOffset(offset));
    }
    break;
    }
    completaComEspacos(retorno,20);
    strcat(retorno,"\t");
    gravaArquivoModo(nomeArquivo,modoGravacao,retorno);
    imprimeRegistradores();
    printf("%s\n",retorno);
}

void imprimeOperacaoAtual(int rs,int rt,int rd,int shamt,int offset,int target){
    switch(operacaoAtual){
        case 0: recuperaOperacao("and",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),0,-1);   break;
        case 1: recuperaOperacao("or",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),0,-1);    break;
        case 2: recuperaOperacao("add",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),0,-1);   break;
        case 3: recuperaOperacao("mult",recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),"",0,-1);                           break;
        case 4: recuperaOperacao("div",recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),"",0,-1);                            break;
        case 5: recuperaOperacao("sll",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rt),"",shamt,0);                         break;
        case 6: recuperaOperacao("sub",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),0,-1);   break;
        case 7: recuperaOperacao("slt",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),0,-1);   break;
        case 8: recuperaOperacao("jr",recuperaNomeRegistrador(rs),"","",0,-1);                                                      break;
        case 9: recuperaOperacao("j","","","",target,0);                                                                            break;
        case 10: recuperaOperacao("jal","","","",target,0);                                                                         break;
        case 11: recuperaOperacao("beq",recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),"",offset,0);                       break;
        case 12: recuperaOperacao("bne",recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),"",offset,0);                       break;
        case 13: recuperaOperacao("addi",recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),"",offset,0);                         break;
        case 14: recuperaOperacao("lw",recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),"",offset,1);                        break;
        case 15: recuperaOperacao("sw",recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),"",offset,1);                        break;
    }
}
/***************************************************************************************/
/************************************** Metodos ****************************************/
/***************************************************************************************/

/**
** Função para avancar o PC
**/
void advance_pc (int offset)
{
    PC = PC + (offset)/4;
}

/** Funcao de Extensão de Sinal do Deslocamento
** Se for negativo, 16 bits superiores = 1
** Se for positivo, 16 bits superiores = 0
** Parametro:
      unsigned int var --> numero a ser extendido
** Retorno
    unsigned int
**/
unsigned int extende_sinal(unsigned int var)
{
    int i=16;
    if(var >> 15 != 0)
    {
        while(i<32)
        {
            var+=1<<i;
            i++;
        }
    }
    return var;
}

/** Função ALU
** Parametros:
    registrador1 --> primeira entrada da ALU
    registrador2 --> segunda entrada da ALU
    ctrlAlu --> indica qual operacao a realizar
        0000  AND
        0001  OR
        0010  ADD
        0110  SUB
        0111  SLT
        1100  NOR
** Retorna
    ALU
**/
ALU executaAlu(int registrador1, int registrador2, int ctrlAlu)
{
    ALU aluControle;
    aluControle.zeroAlu = (registrador1 - registrador2 == 0)? 1 : 0;
    switch(ctrlAlu)
    {
    case 0:
        aluControle.retornoAlu = registrador1 & registrador2;
        break; //and.
    case 1:
        aluControle.retornoAlu = registrador1 | registrador2;
        break; //or.
    case 2:
        aluControle.retornoAlu = registrador1 + registrador2;
        break; //add.
    case 6:
        aluControle.retornoAlu = registrador1 - registrador2;
        break; //sub.
    case 7:
        aluControle.retornoAlu = (registrador1 < registrador2)? 1 : 0;
        break; //slt.
    case 12:
        aluControle.retornoAlu = ~(registrador1 | registrador2);
        break; //nor.
    default:
        printf("\nALU não sabe realizar essa operacao\nCodigo operacao: %d\n\n",ctrlAlu);
    }
    return aluControle;
}

int codigoControleAlu(int opAlu1,int opAlu0,int funct)
{
    int ctrlAlu = 0;
    if(opAlu1 == 0)
    {
        if(opAlu0 == 0) //add
        {
            ctrlAlu = 2;
        }
        else  //sub
        {
            ctrlAlu = 6;
        }
    }
    else
    {
        if(opAlu0 == 1) //sub
        {
            ctrlAlu = 6;
        }
        else
        {
            int op5, op4, op3, op2, op1, op0;
            op5 = funct >> 5;
            funct -= op5 << 5;
            op4 = funct >> 4;
            funct -= op4 << 4;
            op3 = funct >> 3;
            funct -= op3 << 3;
            op2 = funct >> 2;
            funct -= op2 << 2;
            op1 = funct >> 1;
            funct -= op1 << 1;
            op0 = funct;
            if((op5 == 0) && (op4 == 0) && (op3 == 0) &&
                (op2 == 0) && (op1 == 0) && (op0 == 0))//sll
            {
                //ctrlAlu = 5;
                operacaoAtual = 5;
            }
            if((op5 == 0) && (op4 == 0) && (op3 == 0) &&
                (op2 == 0) && (op1 == 0) && (op0 == 0))//jr
            {
                //ctrlAlu = 8;
                operacaoAtual = 8;
            }
            if((op5 == 0) && (op4 == 1) && (op3 == 1) &&
                (op2 == 0) && (op1 == 0) && (op0 == 0))//mult
            {
                //ctrlAlu = 3;
                operacaoAtual = 3;
            }
            if((op5 == 0) && (op4 == 1) && (op3 == 1) &&
                (op2 == 0) && (op1 == 1) && (op0 == 0))//div
            {
                //ctrlAlu = 4;
                operacaoAtual = 4;
            }
            if((op5 == 1) && (op4 == 0) && (op3 == 0) &&
                (op2 == 0) && (op1 == 0) && (op0 == 0))//add
            {
                ctrlAlu = 2;
                operacaoAtual = 2;
            }
            if((op5 == 1) && (op4 == 0) && (op3 == 0) &&
                (op2 == 0) && (op1 == 1) && (op0 == 0))//sub
            {
                ctrlAlu = 6;
                operacaoAtual = 6;
            }
            if((op5 == 1) && (op4 == 0) && (op3 == 0) &&
                (op2 == 1) && (op1 == 0) && (op0 == 0))//and
            {
                ctrlAlu = 0;
                operacaoAtual = 0;
            }
            if((op5 == 1) && (op4 == 0) && (op3 == 0) &&
                (op2 == 1) && (op1 == 0) && (op0 == 1))//or
            {
                ctrlAlu = 1;
                operacaoAtual = 1;
            }
            if((op5 == 1) && (op4 == 0) && (op3 == 1) &&
                (op2 == 0) && (op1 == 1) && (op0 == 0))//slt
            {
                ctrlAlu = 7;
                operacaoAtual = 7;
            }
        }
    }
    return ctrlAlu;
}

Controle cria_controle()
{
    Controle c;
    c.regDst        =   0;
    c.origAlu       =   0;
    c.origPC        =   0;
    c.leMem         =   0;
    c.memParaReg    =   0;
    c.escreveReg    =   0;
    c.escreveMem    =   0;
    c.branchEq      =   0;
    c.branchNotEq   =   0;
    c.opAlu1        =   0;
    c.opAlu0        =   0;
    c.jump          =   0;
    c.ctrlAlu       =   0;
    return c;
}

Controle controleGeral(int opcode,int funct)
{
    Controle ctrl = cria_controle();
    int op5, op4, op3, op2, op1, op0;
    op5 = opcode >> 5;
    opcode -= op5 << 5;
    op4 = opcode >> 4;
    opcode -= op4 << 4;
    op3 = opcode >> 3;
    opcode -= op3 << 3;
    op2 = opcode >> 2;
    opcode -= op2 << 2;
    op1 = opcode >> 1;
    opcode -= op1 << 1;
    op0 = opcode;

    //TIPO R: OPCODE 6 bits, RS 5 bits, RT 5 bits, RD 5 bits, SHAMT 5 bits, FUNCT 6 bits
    //add, sub, mult, div, and, or, jr, slt, sll
    if((op5 == 0) && (op4 == 0) && (op3 == 0) &&
            (op2 == 0) && (op1 == 0) && (op0 == 0))
    {
        ctrl.regDst        =   1;
        ctrl.origAlu       =   0;
        ctrl.memParaReg    =   0;
        ctrl.escreveReg    =   1;
        ctrl.leMem         =   0;
        ctrl.escreveMem    =   0;
        ctrl.branchEq      =   0;
        ctrl.branchNotEq   =   0;
        ctrl.opAlu1        =   1;
        ctrl.opAlu0        =   0;
        ctrl.ctrlAlu       = codigoControleAlu(1,0,funct);

        ctrl.jump          =   0;
    }
    else
    {
        // TIPO J >> OPCODE 6 bits, ENDEREÇO 26 bits
        if((op5 == 0) && (op4 == 0) && (op3 == 0) &&
                (op2 == 0) && (op1 == 1) && (op0 == 0))//j
        {
            //a definir
            operacaoAtual = 9;
        }
        else
        {
            // TIPO J
            if((op5 == 0) && (op4 == 0) && (op3 == 0) &&
                    (op2 == 0) && (op1 == 1) && (op0 == 1))//jal
            {
                //a definir
                operacaoAtual = 10;
            }
            else
            {
                //TIPO I >>  OPCODE 6 bits, RS 5 bits, RT 5 bits, ENDEREÇO/CONSTANTE 16 bits.
                if((op5 == 0) && (op4 == 0) && (op3 == 0) &&
                        (op2 == 1) && (op1 == 0) && (op0 == 0))//beq
                {
                    ctrl.regDst        =   0;
                    ctrl.origAlu       =   0;
                    ctrl.memParaReg    =   0;
                    ctrl.escreveReg    =   0;
                    ctrl.leMem         =   0;
                    ctrl.escreveMem    =   0;
                    ctrl.branchEq      =   1;
                    ctrl.branchNotEq   =   0;
                    ctrl.opAlu1        =   0;
                    ctrl.opAlu0        =   1;
                    ctrl.ctrlAlu       = codigoControleAlu(0,1,funct);

                    ctrl.jump          =   0;
                    operacaoAtual = 11;
                }
                else
                {
                    if((op5 == 0) && (op4 == 0) && (op3 == 0) &&
                            (op2 == 1) && (op1 == 0) && (op0 == 1))//bne
                    {
                        ctrl.regDst        =   0;
                        ctrl.origAlu       =   0;
                        ctrl.memParaReg    =   0;
                        ctrl.escreveReg    =   0;
                        ctrl.leMem         =   0;
                        ctrl.escreveMem    =   0;
                        ctrl.branchEq      =   0;
                        ctrl.branchNotEq   =   1;
                        ctrl.opAlu1        =   0;
                        ctrl.opAlu0        =   1;
                        ctrl.ctrlAlu       = codigoControleAlu(0,1,funct);

                        ctrl.jump          =   0;

                        operacaoAtual = 12;
                    }
                    else
                    {
                        if((op5 == 0) && (op4 == 0) && (op3 == 1) &&
                                (op2 == 0) && (op1 == 0) && (op0 == 0))//addi
                        {
                            ctrl.regDst        =   0;
                            ctrl.origAlu       =   0;
                            ctrl.memParaReg    =   0;
                            ctrl.escreveReg    =   1;
                            ctrl.leMem         =   0;
                            ctrl.escreveMem    =   0;
                            ctrl.branchEq      =   0;
                            ctrl.branchNotEq   =   0;
                            ctrl.opAlu1        =   0;
                            ctrl.opAlu0        =   0;
                            ctrl.ctrlAlu       = codigoControleAlu(0,0,funct);

                            ctrl.jump          =   0;
                            operacaoAtual = 13;
                        }
                        else
                        {
                            if((op5 == 1) && (op4 == 0) && (op3 == 0) &&
                                    (op2 == 0) && (op1 == 1) && (op0 == 1))//lw
                            {
                                ctrl.regDst        =   0;
                                ctrl.origAlu       =   1;
                                ctrl.memParaReg    =   1;
                                ctrl.escreveReg    =   1;
                                ctrl.leMem         =   1;
                                ctrl.escreveMem    =   0;
                                ctrl.branchEq      =   0;
                                ctrl.branchNotEq   =   0;
                                ctrl.opAlu1        =   0;
                                ctrl.opAlu0        =   0;
                                ctrl.ctrlAlu       = codigoControleAlu(0,0,funct);

                                ctrl.jump          =   0;

                                operacaoAtual = 14;
                            }
                            else
                            {
                                if((op5 == 1) && (op4 == 0) && (op3 == 1) &&
                                        (op2 == 0) && (op1 == 1) && (op0 == 1))//sw
                                {
                                    ctrl.regDst        =   0;
                                    ctrl.origAlu       =   1;
                                    ctrl.memParaReg    =   0;
                                    ctrl.escreveReg    =   0;
                                    ctrl.leMem         =   0;
                                    ctrl.escreveMem    =   1;
                                    ctrl.branchEq      =   0;
                                    ctrl.branchNotEq   =   0;
                                    ctrl.opAlu1        =   0;
                                    ctrl.opAlu0        =   0;
                                    ctrl.ctrlAlu       = codigoControleAlu(0,0,funct);

                                    ctrl.jump          =   0;

                                    operacaoAtual = 15;
                                }
                                else
                                {
                                    printf("\nOPCODE %d na posicao %d invalido\n",opcode,(PC+1)); //opcode inválido
                                    gravaArquivoModo(nomeArquivo,modoGravacao,"\n\nErro de opcode invalido\n\n");
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return ctrl;
}


/*** essa eu nem sei se tá usando ***/


/*** concat == deusa gambi? xD ***/
/*
int concatena(int rs, int rt, int rd, int shamt, int funct, int qt)
{
    int conc=0;
    if(qt == 3) //concat rd, shamt, funct
        conc += (rd << 11) + (shamt << 6) + funct;
    else
        if(qt == 5)
            conc += (rs << 21) + (rt << 16) + (rd << 11) + (shamt << 6) + funct;
    return conc;
}

/*** desvio falta ajeitar as variáveis e os parâmetros ***/
/*
void desvio(int opcode. int rs, int rt, int rd, int shamt, int funct, int *PC)
{
if(!ctrl.jump)
if(ctrl.branch && zeroAlu) // beq, bne...
*PC += 4 + extende_sinal(concatena(rs, rt, rd, shamt, funct, 3));
else
if(funct == 8) //jr
*PC = rs;
else //sem desvio
*PC += 4;
else // jump
if(opcode == 2)
reg[31] = *PC + 4;
*PC = concatena(rs, rt, rd, shamt, funct, 3) << 2;
}

void mem_dados(controle ctrl, int resAlu, int *dadosLe2)
{

}
*/




/*** tá foda... ***/
/*
void regist(controle ctrl, int rs, int rt, int rd, int shamt, int funct, int *dadosLe1, int *dadosLe2, int *dadosEsc, int *pc)
{
    int zeroAlu=0, resAlu, ctrlAlu, dadosLeitura, dadosEscrita; //zeroAlu ativa em ALTO.

    /** FALTA VER ONDE FICA: addi, j, jr, jal
     /*   FALTA ARRUMAR: um monte ^^              **/
/*
    if((ctrl.opAlu1 == 0 && ctrl.opAlu2 == 0)  //lw e sw == add
        ctrlAlu = 2;
    else
        if(ctrl.opAlu1 == 1 && ctrl.opAlu2 == 0) //beq ou bne == subtract
            if() //??  >> COMO DIFERENCIA O BEQ DO BNE POR AQUI?
                ctrlAlu = 6;
    else //ver campo funct.
    {
        switch (funct)
        {
            case 0 : ??; //sll
            case 8 : ??; //jr
            case 24: ??; break; //mult
            case 26: ??; break; //div
            case 32: ctrlAlu = 2; break; //add
            case 34: ctrlAlu = 6; break; //sub
            case 36: ctrlAlu = 0; break; //and
            case 37: ctrlAlu = 1; break; //or
            case 42: ctrlAlu = 7; break; //set on less than
            case ??: ; break;
            default: printf("\n\nErro no funct para controle da ALU! \n\n");
        }
    }

    resAlu = alu(rs, rt, rd, &zeroAlu, ctrlAlu, shamt);

    /*** memória de dados, em construção. ***/
/*
    if(ctrl.escMem)
    {
        dadosEscrita = dadosLe2; //tem que ver onde e como vai usar isso aqui...
    }
    else
    {
        ; //anything here??
    }

    if(ctrl.leMem)
    {
        if(ctrl.memReg)
        {
            *dadosEsc = dadosLeitura; //tem que ver como faz pra botar os dados aqui...
        }
        else
        {
            *dadosEsc = resAlu; // :P
        }
    }
    else
    {
        ; //anything here??
    }

    //mem_dados(ctrl, resAlu, &dadosLe2);

    /** fim da memória de dados, em construção. ***/
/*
    desvio(rs, rt, rd, shamt, funct, &pc);
}

/*** tá mto foda... ***/
/*
/**
**
** FIM dasfunções de controle da ALU, em construção.
**
**/

/**
** Procedimento que executa uma quantidade de intruções MIPS(qntInstrucoes)
**/
void executaInstrucoes(int qntInstrucoes)
{
    imprimeCabecalho();

    unsigned int instrucao_decimal;
    int parte_31_26;
    int parte_25_21;
    int parte_20_16;
    int parte_15_11;
    int parte_10_6;
    int parte_5_0;
    int parte_25_0;
    short int parte_15_0;
    unsigned int parte_15_0_extendido;

    int entradaAlu1;
    int entradaAlu2;
    int valorGravarRegistradores;

    ALU unidadeLogica;
    Controle controle;

    PC = 0;//PC seta o ponteiro como a 1° posicao do vetor
    reg[0] = 0;

    while(PC >= 0 && PC < qntInstrucoes)
    {
        operacaoAtual = -1;
        /** Recupera a instrucao **/
        instrucao_decimal = mem[PC];

        /** Incrimenta PC **/
        advance_pc(4);

        /** Quebra a intrucao **/
        parte_31_26 = instrucao_decimal >> 26;                       //desloca 26 bit pra a direita
        instrucao_decimal -= (parte_31_26 << 26);                    //elimina o parte_31_26
        parte_25_21 = instrucao_decimal >> 21;                       //desloca 21 bit pra a direita
        parte_25_0 = instrucao_decimal;
        instrucao_decimal -= (parte_25_21 << 21);                    //elimina o parte_25_21
        parte_20_16 = instrucao_decimal >> 16;                       //desloca 16 bit pra a direita
        instrucao_decimal -= (parte_20_16 << 16);                    //elimina o parte_20_16
        if(instrucao_decimal >> 15 == 1)
        {
            parte_15_0 = 32768 - instrucao_decimal;
        }
        else
        {
            parte_15_0 = instrucao_decimal;
        }
        parte_15_11 = instrucao_decimal >> 11;                       //desloca 11 bit pra a direita
        instrucao_decimal -= (parte_15_11 << 11);                    //elimina o parte_15_11
        parte_10_6 = instrucao_decimal >> 6;                         //desloca 6 bit pra a direita
        parte_5_0 = instrucao_decimal - (parte_10_6 << 6);           //elimina o parte_10_6

        parte_15_0_extendido = extende_sinal(parte_15_0);

        /** Inicia o controle **/
        controle = controleGeral(parte_31_26,parte_5_0);

        /** Carrega os dados para a ALU **/
        entradaAlu1 = reg[parte_25_21];
        if(controle.origAlu == 1){
            entradaAlu2 = reg[parte_25_21];
        }else{
            entradaAlu2 = parte_15_0_extendido;
        }

        /** calcula os valores na ALU **/
        unidadeLogica = executaAlu(entradaAlu1,entradaAlu2,controle.ctrlAlu);


        if(controle.escreveMem == 1){//faz o sw
            //entradaAlu2 e unidadeLogica.retornoAlu
        }

        /** Verifica o que é gravado no banco de registradors **/
        if(controle.memParaReg == 0){//resultado alu
            valorGravarRegistradores = unidadeLogica.retornoAlu;
        }else{//lw

        }

         /** Verifica se pode gravar no registrador **/
        if(controle.escreveReg == 1){
            /** Define o registrador de destino a ser gravado **/
            if(controle.regDst == 0)
            {
                reg[parte_20_16] = valorGravarRegistradores;
            }
            else
            {
                reg[parte_15_11] = valorGravarRegistradores;
            }
        }

        /** Imprime a operacao **/
        imprimeOperacaoAtual(parte_25_21,parte_20_16,parte_15_11,parte_10_6,parte_15_0,parte_25_0);

    }
    gravaArquivoModo(nomeArquivo,modoGravacao,"\n\n------------------------------Fim da gravacao------------------------------\n\n");
}
