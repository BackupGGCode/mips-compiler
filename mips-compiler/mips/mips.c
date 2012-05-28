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
void completaComEspacos(char* texto,int tam){
    int i,tamTxt = strlen(texto);
    if(tamTxt >= tam){
    return;
    }
    for(i= tamTxt;i<=tam;i++){
        strcat(texto," ");
    }
}

/**
** Função para imprimir o cabeçalho do arquivo de lo
**/
void imprimeCabecalho(){
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
    switch(posOffset){
        case -1:{
            if(strcmp(r0,"") > 0){
                strcat(retorno,r0);
            }
            if(strcmp(r1,"") > 0){
                strcat(retorno,", ");
                strcat(retorno,r1);
            }
            if(strcmp(r2,"") > 0){
                strcat(retorno,", ");
                strcat(retorno,r2);
            }
        }break;
        case 0:{
            if(strcmp(r0,"") > 0){
                strcat(retorno,r0);
                strcat(retorno,", ");
            }
            if(strcmp(r1,"") > 0){
                strcat(retorno,r1);
                strcat(retorno,", ");
            }
            strcat(retorno,recuperaOffset(offset));
        }break;
        case 1:{
            strcat(retorno,r0);
            strcat(retorno,", ");
            strcat(retorno,recuperaOffset(offset));
            strcat(retorno,"(");
            strcat(retorno,r1);
            strcat(retorno,")");
        }break;
        case 2:{
            strcat(retorno,recuperaOffset(offset));
        }break;
    }
    completaComEspacos(retorno,20);
    strcat(retorno,"\t");
    gravaArquivoModo(nomeArquivo,modoGravacao,retorno);
    imprimeRegistradores();
    printf("%s\n",retorno);
}

/**
** Função para avancar o PC
**/
void advance_pc (int offset)
{
    PC = PC + (offset)/4;
}



/***************************************************************************************/
/************************** Rotinas relativas às instruções ****************************/
/***************************************************************************************/

/**********************************************************************************/
/************************ INÍCIO DAS INSTRUÇÕES DO TIPO R *************************/
/**********************************************************************************/

/** Funcao add (com overflow)
** add $d, $s, $t
** opcode   rs       rt       rd    shamt    func
** 000000   sssss   ttttt   ddddd   00000   100000
** rs = rt + rd
**/
void add(int rs, int rt, int rd)
{
    recuperaOperacao("add",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),0,-1);
    reg[rs] = reg[rt] + reg[rd];
    advance_pc (4);
}

/** Funcao add unsigned (sem overflow)
** addu $d, $s, $t
** opcode   rs       rt       rd    shamt    func
** 000000   sssss   ttttt   ddddd   00000   100001
** rs = rt + rd
**/
void addu(int rs, int rt, int rd)
{
    recuperaOperacao("addu",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),0,-1);
    advance_pc (4);
}

/** Funcao Bitwise and
** and $d, $s, $t
** opcode   rs       rt       rd    shamt    func
** 000000   sssss   ttttt   ddddd   00000   100100
** $d = $s & $t;
**/
void and_(int rs, int rt, int rd)
{
    recuperaOperacao("and",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),0,-1);
    advance_pc (4);
}

/** Funcao Divide
** div $s, $t
** opcode   rs       rt       rd    shamt    func
** 000000   sssss   ttttt   00000   00000   011010
** ?
**/
void div_(int rs, int rt)
{
    //$LO = $s / $t; $HI = $s % $t;
   recuperaOperacao("div",recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),"",0,-1);
   advance_pc (4);
}

/** Funcao Divide unsigned
** divu $s, $t
** opcode   rs       rt       rd    shamt    func
** 000000   sssss   ttttt   00000   00000   011011
** ?
**/
void divu(int rs, int rt)
{
    //$LO = $s / $t; $HI = $s % $t;
   recuperaOperacao("divu",recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),"",0,-1);
   advance_pc (4);
}

/** Funcao Jump register
** jr $s
** opcode   rs       rt       rd    shamt    func
** 000000   sssss   00000   00000   00000   001000
** ?
**/
void jr(int rs)
{
    //PC = nPC; nPC = $s;
    recuperaOperacao("jr",recuperaNomeRegistrador(rs),"","",0,-1);
    advance_pc (4);
}

/** Funcao Move from HI
** mfhi $d
** opcode   rs       rt       rd    shamt    func
** 000000   00000   00000    ddddd  00000    010000
** ?
**/
void mfhi(int rd)
{
    //	$d = $HI;
    recuperaOperacao("mfhi",recuperaNomeRegistrador(rd),"","",0,-1);
    advance_pc (4);
}

/** Funcao Move from LO
** mflo $d
** opcode   rs       rt       rd    shamt    func
** 000000   00000   00000    ddddd  00000   010010
** ?
**/
void mflo(int rd)
{
    //	$d = $LO;
   recuperaOperacao("mflo",recuperaNomeRegistrador(rd),"","",0,-1);
   advance_pc (4);
}

/** Funcao Multiply
** mult $s, $t
** opcode   rs       rt       rd    shamt    func
** 000000   sssss   ttttt   00000   00000   011000
** ?
**/
void mult(int rs, int rt)
{
    //$LO = $s * $t; advance_pc (4);
    // reg[rs] = reg[rt] * reg[rd];
    recuperaOperacao("mult",recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),"",0,-1);
    advance_pc (4);
}

/** Funcao Multiply unsigned
** multu $s, $t
** opcode   rs       rt       rd    shamt    func
** 000000   sssss   ttttt   00000   00000   011001
** ?
**/
void multu(int rs, int rt)
{
    //$LO = $s * $t;
    recuperaOperacao("multu",recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),"",0,-1);
    advance_pc (4);
}

/** Funcao no operation
** noop
** opcode   rs       rt       rd    shamt    func
** 000000   00000    00000   00000  00000   000000
** ?
**/
void noop()
{
    //vazio mesmo, eu acho :P
    recuperaOperacao("noop","","","",0,-1);
    advance_pc (4);
}

/** Funcao Bitwise or
** or $d, $s, $t
** opcode   rs       rt       rd    shamt    func
** 000000   sssss   ttttt   ddddd   00000   100101
** $d = $s | $t;
**/
void or_(int rs, int rt, int rd)
{
    //$d = $s | $t;
    recuperaOperacao("or",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),0,-1);
    advance_pc (4);
}

/** Funcao Shift left logical
** sll $d, $t, h
** opcode   rs       rt       rd    shamt    func
** 000000   sssss   ttttt   ddddd   hhhhh   000000
** ?
**/
void sll(int rt, int rd, int shamt)
{
    //	$d = $t << h;
   recuperaOperacao("sll",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rt),"",shamt,0);
   advance_pc (4);
}

/** Funcao Shift left logical variable
** sllv $d, $t, $s
** opcode   rs       rt       rd    shamt    func
** 000000   sssss   ttttt   ddddd   -----   000100
** ?
**/
void sllv(int rs, int rt, int rd)
{
    //	$d = $t << $s;
    recuperaOperacao("sllv",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rt),recuperaNomeRegistrador(rs),0,-1);
    advance_pc (4);
}

/** Funcao Set on less than (signed)
** slt $d, $s, $t
** opcode   rs       rt       rd    shamt    func
** 000000   sssss   ttttt   ddddd   00000   101010
** ?
**/
void slt(int rs, int rt, int rd)
{
    //if $s < $t $d = 1; advance_pc (4); else $d = 0; advance_pc (4);
   recuperaOperacao("slt",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),0,-1);
   advance_pc (4);
}

/** Funcao Set on less than unsigned
** sltu $d, $s, $t
** opcode   rs       rt       rd    shamt    func
** 000000   sssss   ttttt   ddddd   00000   101011
** ?
**/
void sltu(int rs, int rt, int rd)
{
    //if $s < $t $d = 1; advance_pc (4); else $d = 0; advance_pc (4);
    recuperaOperacao("sltu",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),0,-1);
    advance_pc (4);
}

/** Funcao Shift right arithmetic
** sra $d, $t, h
** opcode   rs       rt       rd    shamt    func
** 000000   -----   ttttt   ddddd   hhhhh   000011
** ?
**/
void sra(int rt, int rd, int shamt)
{
    //	$d = $t >> h;
    recuperaOperacao("sra",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rt),"",shamt,0);
    advance_pc (4);
}

/** Funcao Shift right logical
** srl $d, $t, h
** opcode   rs       rt       rd    shamt    func
** 000000   -----   ttttt   ddddd   hhhhh   000010
** ?
**/
void srl(int rt, int rd, int shamt)
{
    //	$d = $t >> h;
   recuperaOperacao("srl",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rt),"",shamt,0);
   advance_pc (4);
}

/** Funcao Shift right logical variable
** srlv $d, $t, $s
** opcode   rs       rt       rd    shamt    func
** 000000   sssss   ttttt   ddddd   00000   000110
** ?
**/
void srlv(int rs, int rt, int rd)
{
    //	$d = $t >> $s;
    recuperaOperacao("srlv",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rt),recuperaNomeRegistrador(rs),0,-1);
    advance_pc (4);
}

/** Funcao Subtract
** sub $d, $s, $t
** opcode   rs       rt       rd    shamt    func
** 000000   sssss   ttttt   ddddd   00000   100010
** rs = rt - rd
**/
void sub(int rs, int rt, int rd)
{
    //reg[rs] = reg[rt] - reg[rd];
    recuperaOperacao("sub",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),0,-1);
    advance_pc (4);
}

/** Funcao Subtract unsigned
** subu $d, $s, $t
** opcode   rs       rt       rd    shamt    func
** 000000   sssss   ttttt   ddddd   00000   100011
** rs = rt - rd
**/
void subu(int rs, int rt, int rd)
{
    //reg[rs] = reg[rt] - reg[rd];
    recuperaOperacao("subu",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),0,-1);
    advance_pc (4);
}

/** Funcao System call
** syscall
** opcode   rs       rt       rd    shamt    func
** 000000   -----   -----   -----   -----   001100
** rs = rt - rd
**/
void syscall()
{
    recuperaOperacao("syscall","","","",0,-1);
    advance_pc (4);
}

/** Funcao Bitwise exclusive or
** xor $d, $s, $t
** opcode   rs       rt       rd    shamt    func
** 000000   sssss   ttttt   ddddd   -----   100110
** $d = $s ^ $t
**/
void xor_(int rs, int rt, int rd)
{
    //$d = $s ^ $t; advance_pc (4);
   recuperaOperacao("xor",recuperaNomeRegistrador(rd),recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),0,-1);
   advance_pc (4);
}

/**********************************************************************************/
/************************ INÍCIO DAS INSTRUÇÕES DO TIPO J *************************/
/**********************************************************************************/

/** Funcao Jump
** j target
** opcode         targetif(reg[rs] - reg[rt]!= 0)
    {
        advance_pc(4 + 4*offset);
    }
    else
        advance_pc(4);
** 000010   iiiiiiiiiiiiiiiiiiiiiiiiii
** ?
**/
void j(int target)
{
    //PC = nPC; nPC = (PC & 0xf0000000) | (target << 2);
    recuperaOperacao("j","","","",target,0);
    int v = (PC >> 28);
    PC = (v << 28) + (target << 2);
    advance_pc (4);
}

/** Funcao Jump and link
** jal target
** opcode         target
** 000011   iiiiiiiiiiiiiiiiiiiiiiiiii
** and stores the return address in $31
**/
void jal(int target)
{
    //$31 = PC + 8 (or nPC + 4); PC = nPC; nPC = (PC & 0xf0000000) | (target << 2);
    recuperaOperacao("jal","","","",target,0);
    reg[31] = PC;
    int v = (PC >> 28);
    PC = (v << 28) + (target << 2);
    advance_pc (4);
}

/**********************************************************************************/
/************************ INÍCIO DAS INSTRUÇÕES DO TIPO I *************************/
/**********************************************************************************/

/** Funcao add immediate(com overflow)
** addi $t, $s, imm
** opcode   rs       rt          imm
** 001000   sssss   ttttt   iiiiiiiiiiiiiiii
** rs = rt + imm
**/
void addi(int rs,int rt,int imm)
{
    recuperaOperacao("addi",recuperaNomeRegistrador(rt),recuperaNomeRegistrador(rs),"",imm,0);
    reg[rs] = reg[rt] + imm;
    advance_pc (4);
}

/** Funcao add immediate unsigned (sem overflow)
** addiu $t, $s, imm
** opcode   rs       rt          imm
** 001001   sssss   ttttt   iiiiiiiiiiiiiiii
** rs = rt + imm
**/
void addiu(int rs,int rt, int imm)
{
    recuperaOperacao("addiu",recuperaNomeRegistrador(rt),recuperaNomeRegistrador(rs),"",imm,0);
    advance_pc (4);
}

/** Funcao Bitwise and immediate
** andi $t, $s, imm
** opcode   rs       rt          imm
** 001100   sssss   ttttt   iiiiiiiiiiiiiiii
** $t = $s & imm
**/
void andi(int rs, int rt, int imm)
{
    recuperaOperacao("andi",recuperaNomeRegistrador(rt),recuperaNomeRegistrador(rs),"",imm,0);
    advance_pc (4);
}

/** Funcao Branch on equal
** beq $s, $t, offset
** opcode   rs       rt          offset
** 000100   sssss   ttttt   iiiiiiiiiiiiiiii
** ?
**/
void beq(int rs, int rt, int offset)
{
    //if $s == $t advance_pc (offset << 2)); else advance_pc (4);
    recuperaOperacao("beq",recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),"",offset,0);
    if(reg[rs] - reg[rt] == 0)
    {
        advance_pc(4 + 4*offset);
    }
    else
        advance_pc(4);
}

/** Funcao Branch on greater than or equal to zero
** bgez $s, offset
** opcode   rs       rt          offset
** 000001   sssss   00001   iiiiiiiiiiiiiiii
** ?
**/
void bgez(int rs, int offset)
{
    //if $s >= 0 advance_pc (offset << 2)); else advance_pc (4);
    recuperaOperacao("bgez",recuperaNomeRegistrador(rs),"","",offset,0);
    advance_pc (4);
}

/** Funcao Branch on greater than or equal to zero and link
** bgezal $s, offset
** opcode   rs       rt          offset
** 000001   sssss   10001   iiiiiiiiiiiiiiii
** and saves the return address in $31
**/
void bgezal(int rs, int offset)
{
    //if $s >= 0 $31 = PC + 8 (or nPC + 4); advance_pc (offset << 2)); else advance_pc (4);
    recuperaOperacao("bgezal",recuperaNomeRegistrador(rs),"","",offset,0);
    advance_pc (4);
}

/** Funcao Branch on greater than zero
** bgtz $s, offset
** opcode   rs       rt          offset
** 000111   sssss   00000   iiiiiiiiiiiiiiii
** ?
**/
void bgtz(int rs, int offset)
{
    //if $s > 0 advance_pc (offset << 2)); else advance_pc (4);
    recuperaOperacao("bgtz",recuperaNomeRegistrador(rs),"","",offset,0);
    advance_pc (4);
}

/** Funcao Branch on less than or equal to zero
** blez $s, offset
** opcode   rs       rt          offset
** 000110   sssss   00000   iiiiiiiiiiiiiiii
** ?
**/
void blez(int rs, int offset)
{
    //if $s <= 0 advance_pc (offset << 2)); else advance_pc (4);
    recuperaOperacao("blez",recuperaNomeRegistrador(rs),"","",offset,0);
    advance_pc (4);
}

/** Funcao Branch on less than zero
** bltz $s, offset
** opcode   rs       rt          offset
** 000001   sssss   00000   iiiiiiiiiiiiiiii
** ?
**/
void bltz(int rs, int offset)
{
    //if $s < 0 advance_pc (offset << 2)); else advance_pc (4);
    recuperaOperacao("bltz",recuperaNomeRegistrador(rs),"","",offset,0);
    advance_pc (4);
}

/** Funcao Branch on less than zero and link
** bltzal $s, offset
** opcode   rs       rt          offset
** 000001   sssss   10000   iiiiiiiiiiiiiiii
** and saves the return address in $31
**/
void bltzal(int rs, int offset)
{
    //if $s < 0 $31 = PC + 8 (or nPC + 4); advance_pc (offset << 2)); else advance_pc (4);
    recuperaOperacao("bltzal",recuperaNomeRegistrador(rs),"","",offset,0);
    advance_pc (4);
}

/** Funcao Branch on not equal
** bne $s, $t, offset
** opcode   rs       rt          offset
** 000101   sssss   ttttt   iiiiiiiiiiiiiiii
** ?
**/
void bne(int rs, int rt, int offset)
{
    //if $s != $t advance_pc (offset << 2)); else advance_pc (4);
    //printf("rs = %d\trt = %d\toff: %d\n",reg[rs],reg[rt],offset);
    recuperaOperacao("bne",recuperaNomeRegistrador(rs),recuperaNomeRegistrador(rt),"",offset,0);
    if(reg[rs] - reg[rt]!= 0)
    {
        advance_pc(4 + 4*offset);
    }
    else
        advance_pc(4);
}

/** Funcao Load byte
** lb $t, offset($s)
** opcode   rs       rt          offset
** 100000   sssss   ttttt   iiiiiiiiiiiiiiii
** ?
**/
void lb(int rs, int rt, int offset)
{
    //$t = MEM[$s + offset]; advance_pc (4);
    recuperaOperacao("lb",recuperaNomeRegistrador(rt),recuperaNomeRegistrador(rs),"",offset,1);
    advance_pc (4);
}

/** Funcao Load upper immediate
** lui $t, imm
** opcode   rs       rt          offset
** 001111   -----   ttttt   iiiiiiiiiiiiiiii
** ?
**/
void lui(int rt, int imm)
{
    //$t = (imm << 16); advance_pc (4);
     recuperaOperacao("lui",recuperaNomeRegistrador(rt),"","",imm,0);
     advance_pc (4);
}

/** Funcao Load word
** lw $t, offset($s)
** opcode   rs       rt          offset
** 100011   sssss   ttttt   iiiiiiiiiiiiiiii
** ?
**/
void lw(int rs, int rt, int offset)
{
    //$t = MEM[$s + offset]; advance_pc (4);
    recuperaOperacao("lw",recuperaNomeRegistrador(rt),recuperaNomeRegistrador(rs),"",offset,1);
    advance_pc (4);
}

/** Funcao Bitwise or immediate
** ori $t, $s, imm
** opcode   rs       rt          imm
** 001101   sssss   ttttt   iiiiiiiiiiiiiiii
** $t = $s | imm
**/
void ori(int rs, int rt, int imm)
{
    recuperaOperacao("ori",recuperaNomeRegistrador(rt),recuperaNomeRegistrador(rs),"",imm,0);
    advance_pc (4);
}

/** Funcao Store byte
** sb $t, offset($s)
** opcode   rs       rt          offset
** 101000   sssss   ttttt   iiiiiiiiiiiiiiii
** ?
**/
void sb(int rs, int rt, int offset)
{
    //MEM[$s + offset] = (0xff & $t); advance_pc (4);
    recuperaOperacao("sb",recuperaNomeRegistrador(rt),recuperaNomeRegistrador(rs),"",offset,1);
    advance_pc (4);
}

/** Funcao Set on less than immediate (signed)
** slti $t, $s, imm
** opcode   rs       rt          imm
** 001010   sssss   ttttt   iiiiiiiiiiiiiiii
** ?
**/
void slti(int rs, int rt, int imm)
{
    //if $s < imm $t = 1; advance_pc (4); else $t = 0; advance_pc (4);
   recuperaOperacao("slti",recuperaNomeRegistrador(rt),recuperaNomeRegistrador(rs),"",imm,0);
   advance_pc (4);
}

/** Funcao Set on less than immediate unsigned
** sltiu $t, $s, imm
** opcode   rs       rt          imm
** 001011   sssss   ttttt   iiiiiiiiiiiiiiii
** ?
**/
void sltiu(int rs, int rt, int imm)
{
    //if $s < imm $t = 1; advance_pc (4); else $t = 0; advance_pc (4);
    recuperaOperacao("sltiu",recuperaNomeRegistrador(rt),recuperaNomeRegistrador(rs),"",imm,0);
    advance_pc (4);
}

/** Funcao Store word
** sw $t, offset($s)
** opcode   rs       rt          offset
** 101011   sssss   ttttt   iiiiiiiiiiiiiiii
** ?
**/
void sw(int rs, int rt, int offset)
{
    //MEM[$s + offset] = $t; advance_pc (4);
    recuperaOperacao("sw",recuperaNomeRegistrador(rt),recuperaNomeRegistrador(rs),"",offset,1);
    advance_pc (4);
}

/** Funcao Bitwise exclusive or immediate
** xori $t, $s, imm
** opcode   rs       rt          imm
** 001110   sssss   ttttt   iiiiiiiiiiiiiiii
** $t = $s ^ imm
**/
void xori(int rs, int rt, int imm)
{
    recuperaOperacao("xori",recuperaNomeRegistrador(rt),recuperaNomeRegistrador(rs),"",imm,0);
    advance_pc (4);
}

/***************************************************************************************/
/************************************** Metodos ****************************************/
/***************************************************************************************/


/**
**
** funções de controle da ALU, em construção.
**
**/
/*
int ctrl_extend_signal(int var)
{
    int i=16;
    if(var >> 16 != 0)
    {
        while(i<32)
        {
            var+=1<<i;
            i++;
        }
    }
    return var;
}

controle zera_controle(controle ctrl)
{
    ctrl.regDst=ctrl.origAlu=ctrl.memReg=ctrl.escReg=ctrl.leMem=ctrl.escMem=ctrl.branch=ctrl.opAlu1=ctrl.opAlu2=0;
    return ctrl;
}

int alu(int rs, int rt, int rd, int *zeroAlu, int ctrlAlu)
{

    int resAlu;
    *zeroAlu = 0; //inativa
    switch(ctrlAlu)
    {
        case 0: resAlu = rs & rt; break; //and
        case 1: resAlu = rs | rt; break; //or
        case 2: resAlu = rs + rt; break; //add
        case 6:
        {
            resAlu = rs - rt;
            if(resAlu == 0) *zeroAlu = 1; //seta ativo
        }break; //sub
        case 7: ?????? ; break; //slt
        default: printf("\n\nErro de controle da ALU! \n\n"); //
    }
    return resAlu;
}

void regist(controle ctrl, int rs, int rt, int rd, shamt, funct)
{
    int zeroAlu=0, resAlu, ctrlAlu; //zeroAlu ativa em ALTO.

    if((ctrl.opAlu1 == 0 && ctrl.opAlu2 == 0)  //lw e sw == add
        ctrlAlu = 2;
    else if(ctrl.opAlu1 == 1 && ctrl.opAlu2 == 0) //beq == subtract
        ctrlAlu = 6;
    else //ver campo funct.
    {
        switch (funct)
        {
            case 32: ctrlAlu = 2; break; //add
            case 34: ctrlAlu = 6; break; //sub
            case 36: ctrlAlu = 0; break; //and
            case 37: ctrlAlu = 1; break; //or
            case 42: ctrlAlu = 7; break; //set on less than
            case ??: ; break;
            default: printf("\n\nErro no funct para controle da ALU! \n\n");
        }
    }
    resAlu = alu(rs, rt, rd, &zeroAlu, ctrlAlu);
    if(branch & zeroAlu) //habilita desvio
    {
        ;
    }
    else
    {
        PC+=4;
    }
}

void controle(int opcode, int rs, int rt, int rd, int shamt, int funct)
{
    int op5, op4, op3, op2, op1, op0, opatual;
    controle ctrl;
    ctrl = zera_controle(ctrl);
    int dadosLe1, dadosLe2, dadosEsc;
    op5 = opcode >> 5; opcode -= op5 << 5;
    op4 = opcode >> 4; opcode -= op4 << 4;
    op3 = opcode >> 3; opcode -= op3 << 3;
    op2 = opcode >> 2; opcode -= op2 << 2;
    op1 = opcode >> 1; opcode -= op1 << 1;
    op0 = opcode;
    if(op5 == 0) //R ou beq
    {
        if(op2 == 0) ctrl.regDst=ctrl.escReg=ctrl.opAlu1=1; //R
        else ctrl.branch=ctrl.opAlu2=1; //beq
    }
    else //lw ou sw
    {
        if(op3 == 0) ctrl.origAlu=ctrl.memReg=ctrl.escReg=ctrl.leMem=1; //lw
        else ctrl.origAlu=ctrl.escMem=1; //sw
    }
    regist(ctrl, rs, rt, rd);
}
*/
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

    int opcode, funct, rs, rt, rd, shamt,teste;
    short int aux,imm;
    unsigned int inst_dec;

    PC = 0;//PC seta o ponteiro como a 1° posicao do vetor
    reg[0] = 0;

    while(PC >= 0 && PC < qntInstrucoes)
   {
        inst_dec = mem[PC];
        opcode = inst_dec >> 26;                   //desloca 26 bit pra a direita
        inst_dec -= (opcode << 26);                //elimida o opcode
        switch(opcode)
        {
            //TIPO R: OPCODE 6 bits, RS 5 bits, RT 5 bits, RD 5 bits, SHAMT 5 bits, FUNCT 6 bits
        case 0:
        {
            rs = inst_dec >> 21;               //desloca 21 bit pra a direita
            inst_dec -= (rs << 21);            //elimina o rs
            rt = inst_dec >> 16;               //desloca 16 bit pra a direita
            inst_dec -= (rt << 16);            //elimina o rt
            rd = inst_dec >> 11;               //desloca 11 bit pra a direita
            inst_dec -= (rd << 11);            //elimina o rd
            shamt = inst_dec >> 6;             //desloca 6 bit pra a direita
            funct = inst_dec - (shamt << 6);   //elimina o shamt
            switch(funct)
            {
            case 0://noop == sll(0,0,0)
            {
                if(rd == 0 && rt == 0 && shamt == 0)
                    noop();
                else
                    sll(rs,rt,shamt);
            }
            break;
            case 2:
                srl(rt,rd,shamt);
                break;
            case 3:
                sra(rt,rd,shamt);
                break;
            case 4:
                sllv(rs,rt,rd);
                break;
            case 6:
                srlv(rs,rt,rd);
                break;
            case 8:
                jr(rs);
                break;
            case 12:
                syscall();
                break;
            case 16:
                mfhi(rd);
                break;
            case 18:
                mflo(rd);
                break;
            case 24:
                mult(rs,rt);
                break;
            case 25:
                multu(rs,rt);
                break;
            case 26:
                div_(rs,rt);
                break;
            case 27:
                divu(rs,rt);
                break;
            case 32:
                add(rs,rt,rd);
                break;
            case 33:
                addu(rs,rt,rd);
                break;
            case 34:
                sub(rs,rt,rd);
                break;
            case 35:
                subu(rs,rt,rd);
                break;
            case 36:
                and_(rs,rt,rd);
                break;
            case 37:
                or_(rs,rt,rd);
                break;
            case 38:
                xor_(rs,rt,rd);
                break;
            case 42:
                slt(rs,rt,rd);
                break;
            case 43:
                sltu(rs,rt,rd);
                break;
            default:
                printf("\nFuncao %d na posicao %d invalida\n",funct,(PC+1)); //funct inválida
                gravaArquivoModo(nomeArquivo,modoGravacao,"\n\nErro de funcao invalida\n\n");
            }
        }
        break;
        // TIPO I >> OPCODE 6 bits, RS 5 bits, RT 5 bits, ENDEREÇO/CONSTANTE 16 bits.
        case 1:
        {
            rs = inst_dec >> 21;               //desloca 21 bit pra a direita
            inst_dec -= (rs << 21);            //elimida o rs
            rt = inst_dec >> 16;               //desloca 16 bit pra a direita
            teste = inst_dec - (rt << 16);     //elimina o rt
            aux = teste >> 15;
            if(aux == 1){
                imm = 32768 - teste;
            }else{
                imm = teste;
            }
            switch(rt)
            {
            case 0:
                bltz(rs,imm);
                break;
            case 1:
                bgez(rs,imm);
                break;
            case 16:
                bltzal(rs,imm);
                break;
            case 17:
                bgezal(rs,imm);
                break;
            default:
                printf("\nRegistrado rt %d na posicao %d invalida\n",rt,(PC+1)); //funct inválida
                gravaArquivoModo(nomeArquivo,modoGravacao,"\n\nErro de Registrador invalido\n\n");
            }
        }
        break;
        // TIPO J >> OPCODE 6 bits, ENDEREÇO 26 bits
        case 2:
            j(inst_dec);
            break;
            // TIPO J
        case 3:
            jal(inst_dec);
            break;
            //TIPO I >>  OPCODE 6 bits, RS 5 bits, RT 5 bits, ENDEREÇO/CONSTANTE 16 bits.
        case 4 ... 43:
        {
            rs = inst_dec >> 21;               //desloca 21 bit pra a direita
            inst_dec -= (rs << 21);            //elimida o rs
            rt = inst_dec >> 16;               //desloca 16 bit pra a direita
            teste = inst_dec - (rt << 16);       //elimina o rt
            aux = teste >> 15;
            if(aux == 1){
                imm = 32768 - teste;
            }else{
                imm = teste;
            }
            switch(opcode)
            {
            case 4:
                beq(rs,rt,imm);
                break;
            case 5:
                bne(rs,rt,imm);
                break;
            case 6:
                blez(rs,imm);
                break;
            case 7:
                bgtz(rs,imm);
                break;
            case 8:
                addi(rs,rt,imm);
                break;
            case 9:
                addiu(rs,rt,imm);
                break;
            case 10:
                slti(rs,rt,imm);
                break;
            case 11:
                sltiu(rs,rt,imm);
                break;
            case 12:
                andi(rs,rt,imm);
                break;
            case 13:
                ori(rs,rt,imm);
                break;
            case 14:
                xori(rs,rt,imm);
                break;
            case 15:
                lui(rt,imm);
                break;
            case 32:
                lb(rs,rt,imm);
                break;
            case 35:
                lw(rs,rt,imm);
                break;
            case 40:
                sb(rs,rt,imm);
                break;
            case 43:
                sw(rs,rt,imm);
                break;
            default:
                printf("\nOPCODE %d na posicao %d invalido\n",opcode,(PC+1)); //opcode inválido.
                gravaArquivoModo(nomeArquivo,modoGravacao,"\n\nErro de opcode invalido\n\n");
            }
        }
        break;
        default:
            printf("\nOPCODE %d na posicao %d invalido\n",opcode,(PC+1)); //opcode inválido
            gravaArquivoModo(nomeArquivo,modoGravacao,"\n\nErro de opcode invalido\n\n");
        }
    }
    gravaArquivoModo(nomeArquivo,modoGravacao,"\n\n------------------------------Fim da gravacao------------------------------\n\n");
}
