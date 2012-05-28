#ifndef ARQUIVOS_H_INCLUDED
#define ARQUIVOS_H_INCLUDED

/**
** Procedimento que le um arquivo(arq *FILE)
** Valida se o arquivo e nulo
** Arquivo deve estar no modo de leitura
** Esta funcao não fecha o arquivo lido
**/
int leArquivo(FILE* arq);

/**
** Procedimento que grava em um arquivo(arq *FILE) uma string(texto *char)
** Valida se o arquivo e nulo
** Arquivo deve estar no modo de gravação
** Esta funcao não fecha o arquivo gravado
**/
void gravaArquivo(FILE* arq,char* texto);

/**
** Funcao que le um arquivo apartir do nome(arq *nomeArq)
** com um modo(modo *char) para leitura
** Valida se o modo é de leitura
** Valida se o arquivo existe
** Esta funcao fecha o arquivo lido
** Retorna(int)
   0 -> não conseguiu ler
   1-> conseguiu ler
**/
int leArquivoModo(char* nomeArq,char* modo);

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
int gravaArquivoModo(char* nomeArq,char* modo,char* texto);

/**
** Função que verifica se o nome do arquivo(nomeArq *char)
** termina com separador(separador *char) +
** a extensao (extensaoArq char*)
** Valida se o arquivo é null
** Retorna(int)
   0 --> não é um documento de texto
   1 --> é um documento de texto
**/
int terminaCom(char *nomeArq,char *separador,char* extensaoArq);

#endif // ARQUIVOS_H_INCLUDED
