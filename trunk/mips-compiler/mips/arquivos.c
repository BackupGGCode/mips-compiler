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

/** Função bin2dec
** Converte uma String binária (bin char*) para decimal
** Retorna (unsigned int)
   sum
**/
unsigned int bin2dec(char *bin)
{
    /* Variáveis locais */
    int len;    // Armazena o comprimento da string
    int k;      // indexador da string

    // Variável 'des' salva o bit deslocado para a posição correta no número
    unsigned int sum = 0, des;  // Variável 'sum' salva o número convertido

    int n;  // Salva temporariamente o dígito convertido a ser adicionado a 'sum'

    len =  strlen(bin) -1;  // Armazena o comprimento da string

    for(k = 0; k <= len && bin[k] != '\n' && bin[k] != '\0'; k++)   // Percorre a string
    {
        n = (bin[k] - '0'); // Converte char para número

        if ((n > 1) || (n < 0)) // Se o número for inválido
            return -1;  // Erro

        des = n << (len - k -1);    // Recupera o bit já deslocado para a posição correta
        sum += des; // Adiciona o bit na posição correta do número
    }
    return sum;
}

/**
** Função leArquivo
** Lê um arquivo(arq *FILE)
** Verifica se o arquivo é nulo
** Arquivo deve estar no modo de leitura
** Esta função não fecha o arquivo lido
** Retorna (int)
   i
**/
int leArquivo(FILE* arq)
{
    if (arq == NULL)    //Verifica se o arquivo é NULL
    {
        printf("Arquivo invalido!\n");
        return -1;
    }

    /* Variaveis locais */
    char linha[100];    // Armazena a linha lida atual
    char* result;   // Variável para armazenar se foi possível ler a linha
    int i = 0;      // Variável para mostrar o número da linha
    unsigned int numAtual;  // Variável que armazena a instrução convertida para decimal
    int erro = 0;   // Variável para verificação de erros no código binário

    while (!feof(arq))  // Enquanto não está no fim do arquivo
    {
        // Lê uma linha (inclusive com o '\n')
        result = fgets(linha, 34, arq);  // o 'fgets' lê até 33 caracteres ou até o '\n'

        if (result) // Se foi possível ler
        {
           if(strlen(result) < 33){ //32 bit + '\n'
               erro = 1;
               break;
           }
           if(strlen(result) > 33){ // Se a string é maior do que deveria
                erro = 2;
               break;
           }
            numAtual = bin2dec(result); // Converte a string binária em decimal
            if(numAtual == -1) // Se encontrou algum dígito inválido
            {
                erro = 3;
                break;
            }
            if(i < maxInstrucoes){  // Se ainda não leu todas as instruções
               mem[i] = numAtual;   // Salva a instrução na Memória de Instruções
            }
            else{
               erro = 4;
               break;
            }
        }
        i++; // Incrementa o valor da linha
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
** Procedimento gravaArquivo
** Grava uma string(texto *char) em um arquivo(arq *FILE)
** Verifica se o arquivo é nulo
** Arquivo deve estar no modo de gravação
** Esta função não fecha o arquivo gravado
**/
void gravaArquivo(FILE* arq,char* texto)
{
    /* Variaveis locais */
    int resultado = fputs(texto, arq); // Tenta gravar na linha e verifica se foi possível efetuar a gravação

    if (resultado == EOF)   // Se deu erro na gravação
        printf("Erro na Gravacao\n");
}

/**
** Função leArquivoModo
** Lê um arquivo a partir do nome(arq *nomeArq)
** com um modo(modo *char) para leitura
** Verifica se o modo é de leitura
** Verifica se o arquivo existe
** Esta função fecha o arquivo lido
** Retorna(int)
   0 -> não conseguiu ler
   qndInstrucoes
**/
int leArquivoModo(char* nomeArq,char* modo)
{
    if(!strcmp(modo, "r") && !strcmp(modo,"r+") && !strcmp(modo,"a+"))  //Verifica se o modo é de leitura
    {
        printf("Comando %s nao serve para leitura!\n",modo);
        return 0;
    }

    /* Variáveis locais */
    FILE *arq = fopen(nomeArq, modo);       //Variável do arquivo

    if(arq == NULL) //Se o arquivo não existe ou teve erro na abertura
    {
        printf("O arquivo %s nao existe!\n",nomeArq);
        return 0;
    }

    int qntInstrucoes = leArquivo(arq); //Lê o arquivo

    fclose(arq);    //Fecha o arquivo
    return (qntInstrucoes > 0)?qntInstrucoes : 0;
}

/**
** Função gravaArquivoModo
** Grava em um arquivo a partir do nome(arq *nomeArq)
** com um modo(modo *char) para gravação
** o texto (texto char*)
** Verifica se o modo é de gravação
** Esta função fecha o arquivo lido
** Retorna(int)
   0 -> comando inválido
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
    FILE *arq = fopen(nomeArq, modo); //Variável do arquivo

    gravaArquivo(arq,texto);

    fclose(arq);
    return 1;
}

/**
** Função terminaCom
** Verifica se o nome do arquivo(nomeArq *char)
** termina com separador(separador *char) +
** a extensao (extensaoArq char*)
** Verifica se o arquivo é null
** Retorna(int)
   0 --> não é um documento de texto
   1 --> é um documento de texto
**/
int terminaCom(char *nomeArq,char *separador,char* extensaoArq)
{
    // Verifica se o arquivo é null
    if(nomeArq == NULL)
        return 0;

    /* Variáveis locais */
    char* parte;     // variável para pegar a(s) parte(s) antes do separador

    //Percorre toda a string nomeArq
    for (parte = strtok(nomeArq, separador); parte; parte = strtok(NULL, separador))
    {
        if(!strcmp(extensaoArq,parte))  // Verifica se a parte atual é igual à extensão
        {
            parte = strtok(NULL, separador); // pega próxima parte
            if(parte == NULL)   //Se for o final do nome
                return 1;
        }
    }
    return 0;
}
