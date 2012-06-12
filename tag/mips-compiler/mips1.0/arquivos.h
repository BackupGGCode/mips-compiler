#ifndef ARQUIVOS_H_INCLUDED
#define ARQUIVOS_H_INCLUDED

/**
** Procedimento que le um arquivo(arq *FILE)
** Valida se o arquivo e nulo
** Arquivo deve estar no modo de leitura
** Esta funcao n�o fecha o arquivo lido
**/
int leArquivo(FILE* arq);

/**
** Procedimento que grava em um arquivo(arq *FILE) uma string(texto *char)
** Valida se o arquivo e nulo
** Arquivo deve estar no modo de grava��o
** Esta funcao n�o fecha o arquivo gravado
**/
void gravaArquivo(FILE* arq,char* texto);

/**
** Funcao que le um arquivo apartir do nome(arq *nomeArq)
** com um modo(modo *char) para leitura
** Valida se o modo � de leitura
** Valida se o arquivo existe
** Esta funcao fecha o arquivo lido
** Retorna(int)
   0 -> n�o conseguiu ler
   1-> conseguiu ler
**/
int leArquivoModo(char* nomeArq,char* modo);

/**
** Funcao que grava em um arquivo apartir do nome(arq *nomeArq)
** com um modo(modo *char) para grava��o
** o texto (texto char*)
** Valida se o modo � de grava��o
** Esta funcao fecha o arquivo lido
** Retorna(int)
   0 -> comando invalido
   1-> conseguiu gravar
**/
int gravaArquivoModo(char* nomeArq,char* modo,char* texto);

/**
** Fun��o que verifica se o nome do arquivo(nomeArq *char)
** termina com separador(separador *char) +
** a extensao (extensaoArq char*)
** Valida se o arquivo � null
** Retorna(int)
   0 --> n�o � um documento de texto
   1 --> � um documento de texto
**/
int terminaCom(char *nomeArq,char *separador,char* extensaoArq);

#endif // ARQUIVOS_H_INCLUDED
