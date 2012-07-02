#ifndef ARQUIVOS_H_INCLUDED
#define ARQUIVOS_H_INCLUDED

/**
** Procedimento leArquivo
** L� um arquivo(arq *FILE)
** Valida se o arquivo � nulo
** Arquivo deve estar no modo de leitura
** Esta fun��o n�o fecha o arquivo lido
**/
int leArquivo(FILE* arq);

/**
** Procedimento gravaArquivo
** Grava uma string(texto *char) em um arquivo(arq *FILE)
** Valida se o arquivo � nulo
** Arquivo deve estar no modo de grava��o
** Esta fun��o n�o fecha o arquivo gravado
**/
void gravaArquivo(FILE* arq,char* texto);

/**
** Fun��o leArquivoModo
** L� um arquivo a partir do nome(arq *nomeArq)
** com um modo(modo *char) para leitura
** Valida se o modo � de leitura
** Valida se o arquivo existe
** Esta fun��o fecha o arquivo lido
** Retorna(int)
   0 -> n�o conseguiu ler
   1-> conseguiu ler
**/
int leArquivoModo(char* nomeArq,char* modo);

/**
** Funcao gravaArquivoModo
** Grava em um arquivo a partir do nome(arq *nomeArq)
** com um modo(modo *char) para grava��o
** o texto (texto char*)
** Valida se o modo � de grava��o
** Esta fun��o fecha o arquivo lido
** Retorna(int)
   0 -> comando invalido
   1-> conseguiu gravar
**/
int gravaArquivoModo(char* nomeArq,char* modo,char* texto);

/**
** Fun��o terminaCom
** Verifica se o nome do arquivo(nomeArq *char)
** termina com separador(separador *char) +
** a extensao (extensaoArq char*)
** Valida se o arquivo � null
** Retorna(int)
   0 --> n�o � um documento de texto
   1 --> � um documento de texto
**/
int terminaCom(char *nomeArq,char *separador,char* extensaoArq);

#endif // ARQUIVOS_H_INCLUDED
