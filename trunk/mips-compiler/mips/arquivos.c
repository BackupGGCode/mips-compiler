#include <stdio.h>
#include <string.h>
#include <math.h>

#include "arquivos.h"
#include "layout.h"
#include "mips.h"

/*
  Modos do fopen mod
   Modo 	Significado
   r 	   Abre o arquivo somente para leitura. O arquivo deve existir. (O r vem do inglês read, ler)
   r+ 	Abre o arquivo para leitura e escrita. O arquivo deve existir.
   w 	   Abre o arquivo somente para escrita no início do arquivo. Apagará o conteúdo do arquivo se ele já existir, criará um arquivo novo se não existir. (O w vem do inglês write, escrever)
   w+ 	Abre o arquivo para escrita e leitura, apagando o conteúdo pré-existente.
   a 	   Abre o arquivo para escrita no final do arquivo. Não apaga o conteúdo pré-existente. (O a vem do inglês append, adicionar, apender)
   a+ 	Abre o arquivo para escrita no final do arquivo e leitura.
*/

/** Funcao bin2dec
** Conver uma String binaria(bin char*) para decimal
** Retorna (unsigned int)
   sum
**/
unsigned int bin2dec(char *bin)
{
    int len, k;
    unsigned int sum = 0,des;
    int n;
    len =  strlen(bin) -1;
    for(k = 0; k <= len && bin[k] != '\n' && bin[k] != '\0'; k++)
    {
        n = (bin[k] - '0'); // converte char para numero
        if ((n > 1) || (n < 0))
        {
            return -1;
        }
        des = n << (len - k -1);
        sum += des;
    }
    return sum;
}


/**
** Funcao que le um arquivo(arq *FILE)
** Valida se o arquivo e nulo
** Arquivo deve estar no modo de leitura
** Esta funcao não fecha o arquivo lido
** Retorna (int)
   i
**/
int leArquivo(FILE* arq)
{
    //Verifi ca se o arquivo não é NULL
    if (arq == NULL)
    {
        printf("Arquivo invalido!\n");
        return -1;
    }

    /* Variaveis locais */
    char linha[100];    // Armazena a linha lida atual
    char* result;   // Armazena se foi possivel ler a linha
    int i =0;       // Variavel para mostrar o numero da linha
    unsigned int numAtual;
    int erro = 0;
    // Enquanto não esta no fim do arquivo
    while (!feof(arq))
    {
        // Lê uma linha (inclusive com o '\n')
        result = fgets(linha, 34, arq);  // o 'fgets' lê até 33 caracteres ou até o '\n'
        // Se foi possível ler
        if (result)
        {
           if(strlen(result) < 33){//32 bit + '\n'
               erro = 1;
               break;
           }
           if(strlen(result) > 33){
                erro = 2;
               break;
           }
            numAtual = bin2dec(result);
            if(numAtual == -1)
            {
                erro = 3;
                break;
            }
            if(i < maxInstrucoes){
               mem[i] = numAtual;
            }
            else{
               erro = 4;
               break;
            }
        }
        i++;
    }
    i--;
    switch(erro)
    {
       case 0:{
            limpaPrompt();
            printf("Arquivo lido com sucesso!\nForam lidos %d linhas!\n",i);
      }
      break;
      case 1: {printf("\n--- Erro na linha %d! ---\nNumero Binario com menos de 32 digitos!\n",i); i = -1;}
      break;
      case 2: {printf("\n--- Erro na linha %d! ---\nNumero Binario com mais de 32 digitos!\n",i); i = -1;}
      break;
      case 3: {printf("\n--- Erro na linha %d! ---\nNumero Binario so possui 1 ou 0!\n",i); i = -1;}
      break;
      case 4: {printf("\n--- Erro overflow ---\nMais instrucoes do que o maximo de instrucoes permitidas\n"); i = -1;}
      break;
      default: printf("\n--- Magia negra -----\n");
    }
    return i;
}

/**
** Procedimento que grava em um arquivo(arq *FILE) uma string(texto *char)
** Valida se o arquivo e nulo
** Arquivo deve estar no modo de gravação
** Esta funcao não fecha o arquivo gravado
**/
void gravaArquivo(FILE* arq,char* texto)
{
    /* Variaveis locais */
    int resultado = fputs(texto, arq); // Armazena se foi possivel grava na linha

    // Se deu erro na gravação
    if (resultado == EOF)
    {
        printf("Erro na Gravacao\n");
    }
}


/**
** Funcao que le um arquivo apartir do nome(arq *nomeArq)
** com um modo(modo *char) para leitura
** Valida se o modo é de leitura
** Valida se o arquivo existe
** Esta funcao fecha o arquivo lido
** Retorna(int)
   0 -> não conseguiu ler
   qndInstrucoes
**/
int leArquivoModo(char* nomeArq,char* modo)
{
    //Verifica se o modo é de leitura
    if(!strcmp(modo, "r") && !strcmp(modo,"r+") && !strcmp(modo,"a+") )
    {
        printf("Comando %s nao serve para leitura!\n",modo);
        return 0;
    }

    /* Variaveis locais */
    FILE *arq = fopen(nomeArq, modo);       //Variavel do arquivo

    //se o arquivo não existe ou teve rro na abertura
    if(arq == NULL)
    {
        printf("O arquivo %s nao existe!\n",nomeArq);
        return 0;
    }

    //Le o arquivo
    int qntInstrucoes = leArquivo(arq);

    //Fecha o arquivo
    fclose(arq);
    return (qntInstrucoes > 0)?qntInstrucoes : 0;
}


/**
** Funcao que grava em um arquivo apartir do nome(arq *nomeArq)
** com um modo(modo *char) para gravação
** o texto (texto char*)
** Valida se o modo é de gravação
** Esta funcao fecha o arquivo lido
** Retorna(int)
   0 -> comando invalido
   1-> conseguiu gravar
**/
int gravaArquivoModo(char* nomeArq,char* modo,char* texto)
{
    //Verifica se o modo é de gravação
    if(!strcmp(modo, "w") && !strcmp(modo,"w+") && !strcmp(modo,"a") && !strcmp(modo,"a+") )
    {
        printf("Comando %s nao serve para gravacao!\n",modo);
        return 0;
    }

    /* Variaveis locais */
    FILE *arq = fopen(nomeArq, modo); //Variavel do arquivo

    gravaArquivo(arq,texto);

    fclose(arq);
    return 1;
}

/**
** Função que verifica se o nome do arquivo(nomeArq *char)
** termina com separador(separador *char) +
** a extensao (extensaoArq char*)
** Valida se o arquivo é null
** Retorna(int)
   0 --> não é um documento de texto
   1 --> é um documento de texto
**/
int terminaCom(char *nomeArq,char *separador,char* extensaoArq)
{
    // Verifica se o arquivo é null
    if(nomeArq == NULL)
        return 0;

    /* Variaveis locais */
    char* parte;     // variavel para pegar a(s) parte(s) antes do separador

    //Percorre toda a string nomeArq
    for (parte = strtok(nomeArq, separador);
            parte;
            parte = strtok(NULL, separador))
    {
        // Verifica se a parte atual é igual a extensao
        if(!strcmp(extensaoArq,parte))
        {
            parte = strtok(NULL, separador); // pega proxima parte
            //Se for o final do nome
            if(parte == NULL)
            {
                return 1;
            }
        }
    }
    return 0;
}
