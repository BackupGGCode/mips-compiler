#ifndef ARQUIVOS_H_INCLUDED
#define ARQUIVOS_H_INCLUDED

/**
** Procedimento leArquivo
** Lê um arquivo(arq *FILE)
** Valida se o arquivo é nulo
** Arquivo deve estar no modo de leitura
** Esta função não fecha o arquivo lido
**/
int leArquivo(FILE* arq);

/**
** Procedimento gravaArquivo
** Grava uma string(texto *char) em um arquivo(arq *FILE)
** Valida se o arquivo é nulo
** Arquivo deve estar no modo de gravação
** Esta função não fecha o arquivo gravado
**/
void gravaArquivo(FILE* arq,char* texto);

/**
** Função leArquivoModo
** Lê um arquivo a partir do nome(arq *nomeArq)
** com um modo(modo *char) para leitura
** Valida se o modo é de leitura
** Valida se o arquivo existe
** Esta função fecha o arquivo lido
** Retorna(int)
   0 -> não conseguiu ler
   1-> conseguiu ler
**/
int leArquivoModo(char* nomeArq,char* modo);

/**
** Funcao gravaArquivoModo
** Grava em um arquivo a partir do nome(arq *nomeArq)
** com um modo(modo *char) para gravação
** o texto (texto char*)
** Valida se o modo é de gravação
** Esta função fecha o arquivo lido
** Retorna(int)
   0 -> comando invalido
   1-> conseguiu gravar
**/
int gravaArquivoModo(char* nomeArq,char* modo,char* texto);

/**
** Função terminaCom
** Verifica se o nome do arquivo(nomeArq *char)
** termina com separador(separador *char) +
** a extensao (extensaoArq char*)
** Valida se o arquivo é null
** Retorna(int)
   0 --> não é um documento de texto
   1 --> é um documento de texto
**/
int terminaCom(char *nomeArq,char *separador,char* extensaoArq);

#endif // ARQUIVOS_H_INCLUDED
