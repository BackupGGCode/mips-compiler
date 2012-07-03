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
unsigned int HI,LO;                 // Variáveis HI e LO para tratar multiplicação e divisão

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

    /** Controla onde o Valor de “Dados para Escrita” é colocado    */
    /**     Se 0 -- > nenhum lugar                                  */
    /**     Se 1 -- > colocado em “Endereço”                        */
    int escreveMem;

    /** Controle de tomada de branch                                */
    /**     Se 0 -- > desvio não deve ser tomado                    */
    /**     Se 1 -- > desvio deve ser tomado                        */
    int branchEq;

    /** Controle de tomada de branch                                */
    /**     Se 0 -- > desvio não deve ser tomado                    */
    /**     Se 1 -- > desvio deve ser tomado                        */
    int branchNotEq;

    /** Controlam o tipo de operação deverá ser feita pela ALU      */
    /**     Se 00 -- > add                                          */
    /**     Se 01 -- > subtract                                     */
    /**     Se 10 -- > TIPO R, verificar campo funct                */
    int opAlu1;
    int opAlu0;

    /** Controle de tomada de jump                                  */
    /**     Se 0 -- > jump não deve ser efetuado                    */
    /**     Se 1 -- > jump deve ser efetuado                        */
    int jump;

    /** Controla a operação da ALU                                  */
    /**     Depende dos 'opAlu's e do campo funct                   */
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
** Função recuperaNomeRegistrador
** Recupera o 'nome' do registrador baseado no numero dele(num int)
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
** Função recuperaOffset
** Recupera o offset como char* a partir do parâmetro passado (offset int)
** Retorno
    off char*
**/
char* recuperaOffset(int offset)
{
    /** Variáveis locais **/
    char off[20];           // Vetor que guarda o valor de offset

    itoa(offset,off,10);    // Função do stdlib.h, converte int pra char*
    return off;
}

/**
** Procedimento completaComEspacos
** Completa o vetor de caracter texto(texto char*) ate tam(tam int) espaços
**/
void completaComEspacos(char* texto,int tam)
{
    /** Variáveis locais **/
    int i;  // contador de caracteres da string
    int tamTxt = strlen(texto);   // Variável que recupera o comprimento da string

    if(tamTxt >= tam)   // Se o tamanho da string for maior que o informado
        return; // Só retorna sem modificar a string

    for(i= tamTxt; i<=tam; i++) // Senão percorre a string adicionando espaços
        strcat(texto," ");
}

/**
** Procedimento imprimeCabecalho
** Imprime o cabeçalho no arquivo de log
**/
void imprimeCabecalho()
{
    gravaArquivoModo(nomeArquivo,modoGravacao,"\n------------------------------Inicio da execucao------------------------------\n\n");
    printf("Inicializando operacoes ...\n");

    /** Variáveis locais **/
    int i; // indexador para percorrer o banco de registradores

    gravaArquivoModo(nomeArquivo,modoGravacao,"Operacao//Registradores\tPC\t");
    printf("PC\tOperacao\t");

    //printf("Registradores\t");
    for(i=1; i<32; i++){
        gravaArquivoModo(nomeArquivo,modoGravacao,recuperaNomeRegistrador(i));
        gravaArquivoModo(nomeArquivo,modoGravacao,"\t");
        //printf("%s\t",recuperaNomeRegistrador(i));
    }
    gravaArquivoModo(nomeArquivo,modoGravacao,"RegDst\tOriAlu\tMemPReg\tEscReg\tLeMem\tEscMem\tBEq\tBNE\tOP1\tOP2\tCTR\tJUMP\t\n");
    printf("\n");
}

/**
** Procedimento imprimeRegistradores
** Imprime o estado dos registradores no arquivo de log
**/
void imprimeRegistradores()
{
    /** Variáveis locais **/
    int i;  // indexador para percorrer o banco de registradores

    /** Início da impressão do estado dos registradores no arquivo de log **/
    gravaArquivoModo(nomeArquivo,modoGravacao,recuperaOffset(PC));
    gravaArquivoModo(nomeArquivo,modoGravacao,"\t");
    for(i=1; i<32; i++)
    {
        gravaArquivoModo(nomeArquivo,modoGravacao,recuperaOffset(reg[i]));
        gravaArquivoModo(nomeArquivo,modoGravacao,"\t");
        //printf("%d\t",reg[i]);
    }
    /** Término da impressão do estado dos registradores no arquivo de log **/
}

/**
** Procedimento imprimeControle
** Imprime o estado das variáveis de controle(c Controle) no arquivo de log
**/
void imprimeControle(Controle c)
{
    gravaArquivoModo(nomeArquivo,modoGravacao,"  ");
    gravaArquivoModo(nomeArquivo,modoGravacao,recuperaOffset(c.regDst));
    gravaArquivoModo(nomeArquivo,modoGravacao,"\t  ");

    gravaArquivoModo(nomeArquivo,modoGravacao,recuperaOffset(c.origAlu));
    gravaArquivoModo(nomeArquivo,modoGravacao,"\t  ");

    gravaArquivoModo(nomeArquivo,modoGravacao,recuperaOffset(c.memParaReg));
    gravaArquivoModo(nomeArquivo,modoGravacao,"\t  ");

    gravaArquivoModo(nomeArquivo,modoGravacao,recuperaOffset(c.escreveReg));
    gravaArquivoModo(nomeArquivo,modoGravacao,"\t  ");

    gravaArquivoModo(nomeArquivo,modoGravacao,recuperaOffset(c.leMem));
    gravaArquivoModo(nomeArquivo,modoGravacao,"\t  ");

    gravaArquivoModo(nomeArquivo,modoGravacao,recuperaOffset(c.escreveMem));
    gravaArquivoModo(nomeArquivo,modoGravacao,"\t ");

    gravaArquivoModo(nomeArquivo,modoGravacao,recuperaOffset(c.branchEq));
    gravaArquivoModo(nomeArquivo,modoGravacao,"\t ");

    gravaArquivoModo(nomeArquivo,modoGravacao,recuperaOffset(c.branchNotEq));
    gravaArquivoModo(nomeArquivo,modoGravacao,"\t ");

    gravaArquivoModo(nomeArquivo,modoGravacao,recuperaOffset(c.opAlu1));
    gravaArquivoModo(nomeArquivo,modoGravacao,"\t ");

    gravaArquivoModo(nomeArquivo,modoGravacao,recuperaOffset(c.opAlu0));
    gravaArquivoModo(nomeArquivo,modoGravacao,"\t ");

    gravaArquivoModo(nomeArquivo,modoGravacao,recuperaOffset(c.ctrlAlu));
    gravaArquivoModo(nomeArquivo,modoGravacao,"\t ");

    gravaArquivoModo(nomeArquivo,modoGravacao,recuperaOffset(c.jump));
    gravaArquivoModo(nomeArquivo,modoGravacao,"\t ");

    gravaArquivoModo(nomeArquivo,modoGravacao,"\n");
}

/** Procedimento recuperaOperacao
** Imprime a instrução
** Parâmentros
   ope       *char   -- Nome da operacao
   r0        *char   -- Registrador
   r1        *char   -- Registrador
   r2        *char   -- Registrador
   offset    int     -- offset
   posOffset int     --
                      -1  > imprime sem offset
                       0  > imprime com o offset no final
                       1  > imprime com o offset no meio
                       2  > imprime com 1 registrador e o offset
    c       Controle -- Controle atual
**/
void recuperaOperacao(char* ope,char* r0,char* r1,char* r2,int offset,int posOffset,Controle c)
{
    /** Variáveis locais **/
    char retorno[20];   //

    strcpy(retorno,ope);    // Copia o nome da operação atual para a string retorno
    strcat(retorno," ");    // Adiciona um espaço ao final da string retorno

    /** Checa tipo de offset, para formatar corretamente a instrução atual **/
    switch(posOffset)
    {
    case -1:    // sem offset: Ex.: add $t1 $t2 $t3
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
    case 0:     // offset no final: Ex.: addi $t1 $t2 20
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
    case 1:     // offset no meio: Ex.: sw $t1 20($t2)
    {
        strcat(retorno,r0);
        strcat(retorno,", ");
        strcat(retorno,recuperaOffset(offset));
        strcat(retorno,"(");
        strcat(retorno,r1);
        strcat(retorno,")");
    }
    break;
    case 2:     // instruções de salto: Ex.: j 20
    {
        strcat(retorno,recuperaOffset(offset));
    }
    break;
    }

    /** Continua a formatação da string **/
    completaComEspacos(retorno,20);
    strcat(retorno,"\t");

    printf("%d\t%s\t",PC*4,retorno);                    // imprime "PC+4   retorno"

    gravaArquivoModo(nomeArquivo,modoGravacao,retorno); // grava a impressão no arquivo de log

    imprimeRegistradores();                             // grava o valor dos registradores no arquivo de log

    imprimeControle(c);                                 // grava o valor dos sinais de controle no arquivo de log

    printf("\n");                                       // melhora a formatação
}

/**
** Procedimento imprimeOperacaoAtual
** Recupera os dados da instrução que está sendo executada
** Parâmentros
   rs        int     -- Registrador
   rt        int     -- Registrador
   rd        int     -- Registrador
   offset    int     -- offset
   target    int     -- target de instrucoes do tipo j
    c       Controle -- Controle atual
**/
void imprimeOperacaoAtual(int rs,int rt,int rd,int shamt,int offset,int target,Controle c)
{
    switch(operacaoAtual)
    {
    case 0:
        recuperaOperacao("and",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),0,-1,c);
        break;
    case 1:
        recuperaOperacao("or",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),0,-1,c);
        break;
    case 2:
        recuperaOperacao("add",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),0,-1,c);
        break;
    case 3:
        recuperaOperacao("mult",recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),"",0,-1,c);
        break;
    case 4:
        recuperaOperacao("div",recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),"",0,-1,c);
        break;
    case 5:
        recuperaOperacao("sll",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),"",shamt,0,c);
        break;
    case 6:
        recuperaOperacao("sub",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),0,-1,c);
        break;
    case 7:
        recuperaOperacao("slt",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),0,-1,c);
        break;
    case 8:
        recuperaOperacao("jr",recuperaNomeRegistrador(rs),"","",0,-1,c);
        break;
    case 9:
        recuperaOperacao("j","","","",target,0,c);
        break;
    case 10:
        recuperaOperacao("jal","","","",target,0,c);
        break;
    case 11:
        recuperaOperacao("beq",recuperaNomeRegistrador(rt),recuperaNomeRegistrador(rs),"",offset,0,c);
        break;
    case 12:
        recuperaOperacao("bne",recuperaNomeRegistrador(rt),recuperaNomeRegistrador(rs),"",offset,0,c);
        break;
    case 13:
        recuperaOperacao("addi",recuperaNomeRegistrador(rt),recuperaNomeRegistrador(rs),"",offset,0,c);
        break;
    case 14:
        recuperaOperacao("lw",recuperaNomeRegistrador(rt),recuperaNomeRegistrador(rs),"",offset,1,c);
        break;
    case 15:
        recuperaOperacao("sw",recuperaNomeRegistrador(rt),recuperaNomeRegistrador(rs),"",offset,1,c);
        break;
    case 16:
        recuperaOperacao("mfhi",recuperaNomeRegistrador(rd),"","",0,-1,c);
        break;
    case 17:
        recuperaOperacao("mflo",recuperaNomeRegistrador(rd),"","",0,-1,c);
        break;
    }
}
/***************************************************************************************/
/************************************** Metodos ****************************************/
/***************************************************************************************/

/**
** Procedimento atualiza_PC
** Atualiza o valor de PC, de acordo com a instrução atual
** Parâmentros
   deslocamento        int     -- Caso aja deslocamento,o valor do deslocamento
   mutex               int     -- Validador se há deslocamento
                       1  > tem deslocamento
                       0  > não tem deslocamento
**/
void atualiza_PC(int deslocamento,int mutex)
{
    PC = PC + 4/4; // Incrementa o valor de PC
    if(mutex == 1){
        if(deslocamento > 32768){//Verifica se o numero era negativo
            deslocamento = (deslocamento >> 16);//retira a extensao de sinal
        }
        deslocamento = (deslocamento >> 2); // >> 2 adicional é porque tratamos do índice do vetor
        PC += deslocamento;
    }
}

/**
** Função extende_sinal
** Extensde o Sinal do Deslocamento
** Se for negativo, 16 bits superiores = 1
** Se for positivo, 16 bits superiores = 0
** Parâmetro:
      var   unsigned int var -- número a ser extendido
** Retorno
    unsigned int
**/
unsigned int extende_sinal(unsigned int var)
{
    /** Variáveis locais **/
    int i=16;   // Variável de indexação dos dígitos do número var

    if(var >> 15 != 0)  // Se o 16º dígito for diferente de 0 (ou seja, se ele for 1)
    {
        while(i<32) // percorremos os dígitos mais significativos a partir do 16º
        {
            var+=1<<i;  // e extendemos o sinal, dígito por dígito
            i++;
        }
    }   // Se o 16º dígito for 0, nada é feito, pois o número extendido já vem preenchido com zeros
    return var;
}

/** Função executaAlu
** Simula a execução da ALU
** Parâmetros:
    registrador1  int  -- primeira entrada da ALU
    registrador2  int  -- segunda entrada da ALU
    ctrlAlu       int  -- indica qual operacao a realizar
                        0000  AND
                        0001  OR
                        0010  ADD
                        0011  MULT
                        0100  DIV
                        0101  SLL
                        0110  SUB
                        0111  SLT
                        1100  NOR
** Retorna
    ALU
**/
ALU executaAlu(int registrador1, int registrador2, int ctrlAlu)
{
    /** Variáveis locais **/
    ALU aluControle;    // Struct que contém as variáveis de controle da ALU

    // Modifica o valor do sinal de controle zeroAlu, de acordo com o valor dos registradores em questão
    aluControle.zeroAlu = (registrador1 - registrador2 == 0)? 1 : 0;    // zeroAlu é ativo em ALTO

    switch(ctrlAlu) // Escolhe qual operação deve ser executada pela ALU
    {
    case 0:
        aluControle.retornoAlu = registrador1 & registrador2;
        break;  //and.
    case 1:
        aluControle.retornoAlu = registrador1 | registrador2;
        break;  //or.
    case 2:
        aluControle.retornoAlu = registrador1 + registrador2;
        break;  //add.
    case 3:{
        HI = registrador1 * registrador2;
        LO = registrador1 * registrador2;
    }
        break;  //mult
    case 4:{
        HI = registrador1 % registrador2;
        LO = registrador1 / registrador2;
    }
        break;  //div
    case 5:
        aluControle.retornoAlu = registrador1 << registrador2;
        break;  //sll.
    case 6:
        aluControle.retornoAlu = registrador1 - registrador2;
        break;  //sub.
    case 7:
        aluControle.retornoAlu = (registrador1 < registrador2)? 1 : 0;
        break;  //slt.
    case 12:
        aluControle.retornoAlu = ~(registrador1 | registrador2);
        break;  //nor.
    default:    //operação inválida
        printf("\nALU nao sabe realizar essa operacao\nCodigo operacao: %d\n\n",ctrlAlu);
    }
    return aluControle;
}

/**
** Função codigoControleAlu
** Gera os sinais de controle para que a ALU execute a operação correta
** Parâmetros:
    opAlu1  int  -- gerado pela unidade de controle
    opAlu0  int  -- gerado pela unidade de controle
    funct   int  -- caso seja instrucao do tipo R, usa o funct
**/
int codigoControleAlu(int opAlu1,int opAlu0,int funct)
{
    /** Variáveis locais **/
    int ctrlAlu = 0;    // Guarda o código da operação que deve ser realizada pela ALU

    if(opAlu1 == 0)
    {
        if(opAlu0 == 0) //add, para instruções como LW e SW
        {
            ctrlAlu = 2;
        }
        else            //sub, para instruções como BEQ e BNE
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
        else    // instruções do tipo R
        {
            /** Variáveis locais **/
            int op5, op4, op3, op2, op1, op0;
            // As variáveis op0, op1, op2, op3, op4, op5 pegam os dígitos separados do funct,
            // para descobrir qual das instruções do tipo R está sendo tratada

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
                ctrlAlu = 5;
                operacaoAtual = 5;
            }
            if((op5 == 0) && (op4 == 0) && (op3 == 1) &&
                    (op2 == 0) && (op1 == 0) && (op0 == 0))//jr
            {
                ctrlAlu = 8;
                operacaoAtual = 8;
            }
            if((op5 == 0) && (op4 == 1) && (op3 == 0) &&
                    (op2 == 0) && (op1 == 0) && (op0 == 0))//mfhi
            {
                ctrlAlu = 16;
                operacaoAtual = 16;
            }
            if((op5 == 0) && (op4 == 1) && (op3 == 0) &&
                    (op2 == 0) && (op1 == 1) && (op0 == 0))//mflo
            {
                ctrlAlu = 17;
                operacaoAtual = 17;
            }
            if((op5 == 0) && (op4 == 1) && (op3 == 1) &&
                    (op2 == 0) && (op1 == 0) && (op0 == 0))//mult
            {
                ctrlAlu = 3;
                operacaoAtual = 3;
            }
            if((op5 == 0) && (op4 == 1) && (op3 == 1) &&
                    (op2 == 0) && (op1 == 1) && (op0 == 0))//div
            {
                ctrlAlu = 4;
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

/**
** Função cria_controle
** Cria uma struct 'controle', com todos os sinais valendo 0
**/
Controle cria_controle()
{
    /** Variáveis locais **/
    Controle c; // Struct que contém todos os sinais de controle do caminho de dados

    /** Zerando todos os sinais de controle **/
    c.regDst        =   0;
    c.origAlu       =   0;
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

/**
** Função controleGeral
** Gerencia todas as operações relativas aos sinais de controle
**/
Controle controleGeral(int opcode,int funct)
{
    /** Variáveis locais **/
    Controle ctrl = cria_controle();    // Struct que contém as variáveis para todos os sinais de controle
    int op5, op4, op3, op2, op1, op0;   // Variáveis para desmembrar o opcode, para verificar qual a instrução atual

    /** Desmembramento do opcode em bits separados **/
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
        /** Setando os sinais de controle corretos para instruções do tipo R **/
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

        ctrl.ctrlAlu       = codigoControleAlu(1,0,funct);  // escolhe, de acordo com o funct, a instrução do tipo R a ser executada

        if(ctrl.ctrlAlu == 8)    // jr
        {
            ctrl.escreveReg = 0;    // jr não escreve no banco de registradores
            ctrl.jump = 1;          // jr, embora seja do tipo R, é um jump
        }
        else
            ctrl.jump = 0;   // outras instruções do tipo R não utilizam jump
    }
    else
    {
        // TIPO J >> OPCODE 6 bits, ENDEREÇO 26 bits
        if((op5 == 0) && (op4 == 0) && (op3 == 0) &&
                (op2 == 0) && (op1 == 1) && (op0 == 0))//j
        {
                    /** Setando os sinais de controle corretos para a instruçaõ j **/
                    ctrl.regDst        =   0;
                    ctrl.origAlu       =   0;
                    ctrl.memParaReg    =   0;
                    ctrl.escreveReg    =   0;
                    ctrl.leMem         =   0;
                    ctrl.escreveMem    =   0;
                    ctrl.branchEq      =   0;
                    ctrl.branchNotEq   =   0;
                    ctrl.opAlu1        =   0;
                    ctrl.opAlu0        =   0;
                    ctrl.jump          =   1;
                    operacaoAtual = 9;
        }
        else
        {
            // TIPO J
            if((op5 == 0) && (op4 == 0) && (op3 == 0) &&
                    (op2 == 0) && (op1 == 1) && (op0 == 1))//jal
            {
                    /** Setando os sinais de controle corretos para a instruçaõ jal **/
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
                    ctrl.jump          =   1;
                operacaoAtual = 10;
            }
            else
            {
                //TIPO I >>  OPCODE 6 bits, RS 5 bits, RT 5 bits, ENDEREÇO/CONSTANTE 16 bits.
                if((op5 == 0) && (op4 == 0) && (op3 == 0) &&
                        (op2 == 1) && (op1 == 0) && (op0 == 0))//beq
                {
                    /** Setando os sinais de controle corretos para beq **/
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

                    ctrl.ctrlAlu       = codigoControleAlu(0,1,funct);  // escolhe a instrução a ser executada

                    ctrl.jump          =   0;   // beq não realizam jump, realiza branch
                    operacaoAtual = 11;
                }
                else
                {
                    if((op5 == 0) && (op4 == 0) && (op3 == 0) &&
                            (op2 == 1) && (op1 == 0) && (op0 == 1))//bne
                    {
                        /** Setando os sinais de controle corretos para bne **/
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

                        ctrl.ctrlAlu       = codigoControleAlu(0,1,funct);  // escolhe a instrução a ser executada

                        ctrl.jump          =   0;   // bne não realiza jump, realiza branch

                        operacaoAtual = 12;
                    }
                    else
                    {
                        if((op5 == 0) && (op4 == 0) && (op3 == 1) &&
                                (op2 == 0) && (op1 == 0) && (op0 == 0))//addi
                        {
                            /** Setando os sinais de controle corretos para addi **/
                            ctrl.regDst        =   0;
                            ctrl.origAlu       =   1;
                            ctrl.memParaReg    =   0;
                            ctrl.escreveReg    =   1;
                            ctrl.leMem         =   0;
                            ctrl.escreveMem    =   0;
                            ctrl.branchEq      =   0;
                            ctrl.branchNotEq   =   0;
                            ctrl.opAlu1        =   0;
                            ctrl.opAlu0        =   0;

                            ctrl.ctrlAlu       = codigoControleAlu(0,0,funct);  // escolhe a instrução a ser executada

                            ctrl.jump          =   0;   // addi não realiza jump
                            operacaoAtual = 13;
                        }
                        else
                        {
                            if((op5 == 1) && (op4 == 0) && (op3 == 0) &&
                                    (op2 == 0) && (op1 == 1) && (op0 == 1))//lw
                            {
                                /** Setando os sinais de controle corretos para lw **/
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

                                ctrl.ctrlAlu       = codigoControleAlu(0,0,funct);  // escolhe a instrução a ser executada

                                ctrl.jump          =   0;   // lw não realiza jump

                                operacaoAtual = 14;
                            }
                            else
                            {
                                if((op5 == 1) && (op4 == 0) && (op3 == 1) &&
                                        (op2 == 0) && (op1 == 1) && (op0 == 1))//sw
                                {
                                    /** Setando os sinais de controle corretos para sw **/
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

                                    ctrl.ctrlAlu       = codigoControleAlu(0,0,funct);  // escolhe a instrução a ser executada

                                    ctrl.jump          =   0;   // sw não realiza jump

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

/** log de teste de funções **/
/**
Funcoes testadas:
addi    ----> ok
add     ----> ok
sub     ----> ok
mult    ----> ok
div     ----> ok
and     ----> ok
or      ----> ok
slt     ----> ok
sll     ----> ok
lw      ---->
sw      ---->
beq     ----> ok
bne     ----> ok
j       ----> ok
jr      ---->
jal     ---->
**/

/**
** Procedimento executaInstrucoes
** Executa uma quantidade de intruções MIPS(qntInstrucoes)
**/
void executaInstrucoes(int qntInstrucoes,int modoExecucao)
{
    imprimeCabecalho(); // Chama função de impressão de cabeçalho a fim de organização

    /** Variáveis locais **/
    unsigned int instrucao_decimal;     // guarda o valor da instrução atual convertida para decimal

    int parte_31_26;                    // guarda o valor do OPCODE
    int parte_25_21;                    // guarda o valor do RS
    int parte_20_16;                    // guarda o valor do RT
    int parte_15_11;                    // guarda o valor do RD
    int parte_10_6;                     // guarda o valor do SHAMT
    int parte_5_0;                      // guarda o valor do FUNCT
    int parte_25_0;                     // guarda o valor do endereço de jump
    short int parte_15_0;               // guarda o valor do endereço de branch
    unsigned int parte_15_0_extendido;  // guarda o valor do endereço de branch, após a extensão de sinal

    int deslocamento;  // guarda o valor do deslocamento a ser executado no PC, após uma instrução de desvio oou salto

    int entradaAlu1;                    // guarda o 1º valor a ser operado pela ALU
    int entradaAlu2;                    // guarda o 2º valor a ser operado pela ALU
    int valorGravarRegistradores;       // guarda o valor a ser gravado no banco de registradores

    ALU unidadeLogica;                  // struct que salva o resultado da operação feita pela ALU, assim como sinal de controle zeroAlu
    Controle controle;                  // struct que salva os sinais de controle do caminho de dados

    PC = 0;     // PC seta o ponteiro como a 1° posicao do vetor
    reg[0] = 0; // Seta o valor do registrador $ZERO

    while(PC >= 0 && PC < qntInstrucoes)    // Enquanto ainda existirem instruções a serem lidas
    {
        operacaoAtual = -1; // reseta o valor da instrução

        /** Carrega a instrucao atual a partir da memória **/
        instrucao_decimal = mem[PC];

        /** Divide a instrução em partes **/
        parte_31_26 = instrucao_decimal >> 26;              // guarda o OPCODE
        instrucao_decimal -= (parte_31_26 << 26);           // elimina o OPCODE da instrução (necessário para dividir com facilidade as partes)
        parte_25_21 = instrucao_decimal >> 21;              // guarda o RS
        parte_25_0 = instrucao_decimal;                     // guarda o endereço de jump (caso seja necessário futuramente)
        instrucao_decimal -= (parte_25_21 << 21);           // elimina o RS da instrução
        parte_20_16 = instrucao_decimal >> 16;              // guarda o RT
        instrucao_decimal -= (parte_20_16 << 16);           // elimina o RT da instrução
        deslocamento = parte_25_0;
        /** Essa parte é útil caso a instrução seja branch **/
        if(instrucao_decimal >> 15 == 1)
            parte_15_0 = 32768 - instrucao_decimal;         // Salva o endereço de branch
        else
            parte_15_0 = instrucao_decimal;                 // Salva o endereço de branch

        parte_15_11 = instrucao_decimal >> 11;              // guarda o RD
        instrucao_decimal -= (parte_15_11 << 11);           // elimina o RD da instrução
        parte_10_6 = instrucao_decimal >> 6;                // guarda o SHAMT
        parte_5_0 = instrucao_decimal - (parte_10_6 << 6);  // guarda o FUNCT

        parte_15_0_extendido = extende_sinal(parte_15_0);   // extende o sinal do endereço de branch

        /** Inicia o controle **/
        controle = controleGeral(parte_31_26,parte_5_0);    // Carrega os sinais de controle de acordo com a instrução atual

        /** Carrega os dados para a ALU **/
        entradaAlu1 = reg[parte_25_21];                     // Carrega o 1º valor a ser operado pela ALU

        /** Escolhe o 2º valor a ser operado pela ALU **/
        if(controle.origAlu == 0)
        {
            if(controle.ctrlAlu == 5) // sll
            {
                entradaAlu2 = parte_10_6;
            }
            else
            {
                if(controle.ctrlAlu == 16) // mfhi
                {
                    entradaAlu1 = 0;
                    entradaAlu2 = HI;
                    controle.ctrlAlu = 2;
                }
                else
                {
                    if(controle.ctrlAlu == 17) // mflo
                    {
                        entradaAlu1 = 0;
                        entradaAlu2 = LO;
                        controle.ctrlAlu = 2;
                    }
                    else
                    {
                        if(controle.ctrlAlu == 8) // jr
                        {
                            entradaAlu1 = 0;                    // zero, para não modificar o registrador
                            entradaAlu2 = parte_25_21;          // registrador no qual a operação será realizada
                            controle.ctrlAlu = 2;               // add
                            deslocamento = reg[parte_25_21];    // modifica o valor da variável de deslocamento
                        } else { // outras instruções que façam uso da ALU
                            entradaAlu2 = reg[parte_20_16];
                        }
                    }
                }
            }
        }
        else    // sw e lw
        {
            entradaAlu2 = parte_15_0;
        }

        /** Realiza os cálculos na ALU **/
        unidadeLogica = executaAlu(entradaAlu1,entradaAlu2,controle.ctrlAlu);

        if(controle.escreveMem == 1) // sw // mem_dados[15_0 + rs] = rt;
        {
            mem_dados[unidadeLogica.retornoAlu] = reg[parte_20_16];
        }

        /** Verifica o que é gravado no banco de registradores **/
        if(controle.memParaReg == 0) //resultado alu
        {
            valorGravarRegistradores = unidadeLogica.retornoAlu;
        }
        else
        {
            if(controle.leMem == 1){// lw // rt = mem_dados[15_0 + rs];
                valorGravarRegistradores = mem_dados[unidadeLogica.retornoAlu];
            }
        }

        /** Verifica se pode gravar no banco de registradores **/
        if(controle.escreveReg == 1)
        {
            if(controle.ctrlAlu != 3 && controle.ctrlAlu != 4){
                /** Define o registrador de destino a ser gravado **/
                if(controle.regDst == 0){
                    if(controle.jump == 0){//não é jal
                        reg[parte_20_16] = valorGravarRegistradores;
                    }
                }else
                    reg[parte_15_11] = valorGravarRegistradores;
            }
        }

        /** Imprime a operação atual **/
        imprimeOperacaoAtual(parte_25_21,parte_20_16,parte_15_11,parte_10_6,parte_15_0,parte_25_0,controle);

        /** Incrementa e/ou atualiza o valor de PC **/
        if (controle.jump == 1){    // Se for jump...
            if (controle.escreveReg == 1){ // Se for jal
                reg[31] = (PC + 4/4)<<2;   //atualiza valor do registrador 31 ($ra)
            }
            PC = PC >> 28;          // descarta os 28 bits menos significativos do PC
            PC = PC << 28;          // realinha o valor dos dígitos mais significativos de PC
            PC += (deslocamento);   // soma o valor de salto ao PC
            PC = PC >> 2;           // alinha o PC para casar com os indíces de vetor (estrutura utilizada pra simular a memória)
        } else {                    // não sendo jump...
            /** Verifica se é branch equal ou se é branch not equal*/
            if(((controle.branchEq == 1) && (unidadeLogica.zeroAlu == 1)) ||
                ((controle.branchNotEq == 1) && (unidadeLogica.zeroAlu == 0))){//toma desvio
                    atualiza_PC((parte_15_0_extendido<<2),1);   // atualiza_PC: beq e bne, utiliza parte_15_0
                }else{
                    atualiza_PC((parte_15_0_extendido<<2),0);   // atualiza_PC: PC += 4
                }
        }

        /** Caso o modo de execução seja PASSO A PASSO, pausa até que seja apertada alguma tecla **/
        if(modoExecucao == 1){
            getchar();
        }
    }
    gravaArquivoModo(nomeArquivo,modoGravacao,"\n\n------------------------------Fim da gravacao------------------------------\n\n");
    printf("Operacoes finalizadas com sucesso!\n");
}

/**
** Procedimento resetaRegistradoresMemoria
** Responsável pela função RESET
** Reseta os valores de todos os registradores, incluindo HI e LO
** a memória de instruções e de dados
**/
void resetaRegistradoresMemoria()
{
    /** Variáveis locais **/
    int i;  // indexador, para percorrer o vetor de registradores

    /** zera os registradores **/
    for(i=0;i<32;i++){  // Passa por todos os registradores, zerando cada um deles
        reg[i] = 0;
    }

    /** zera os registradores HI e LO **/
    HI=0;
    LO=0;

    /** zera a memória de instrução **/
    for(i=0;i<maxInstrucoes;i++){
        mem[i] = 0;         //noop
    }

    /** zera a memória de dados **/
    for(i=0;i<tamanhoMemoriaDados/4;i++){
        mem_dados[i] = 0;
    }
}
