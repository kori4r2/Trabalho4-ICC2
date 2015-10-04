/*
		Trabalho 3: Base de Dados

			Italo Tobler Silva - nUSP 8551910		17/09/2015

	OBS1: Recomendável para a leitura do codigo manter aberto uma copia dos defines e dos conteudos das structs
	para facilitar consulta

	OBS2: Nao houve tempo para ser feita a documentação das funções, mas elas foram devidamente comentadas internamente
*/
#ifndef _SCHEMA
#define _SCHEMA

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "my_strings.h"

// Length items é o numero maximo de digitos dos numeros lidos
#define LENGTH_ITEMS 50
// Esses sao os identificados usados para cada tipo de dado manipulado
#define INT_T 0
#define STRING_T 1
#define DOUBLE_T 2

/*
	Essas strings sao definidas para o caso do usuario desejar mudar o identificador dos tipos de dado ou do nome dos arquivos
   ou do indicador de ordenacao. A string DELIMITERS é foi criada para o uso da função strtok
*/
#define STR_TABLE "table"
#define STR_ORDER "order"
#define STR_CHAR "char"
#define STR_INT "int"
#define STR_DOUBLE "double"
#define DELIMITERS " []\t"

/*
	As informações do arquivo .schema são armazenadas como se fizessem parte de uma lista duplamente ligada onde cada nó contém
   as informações necessárias para serem feitas operações de comparação, impressão e análise de tamanho
*/
// Structs utilizadas-------------------------------------------------------------------------------------------------
typedef struct schema SCHEMA;

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
SCHEMA *create_schema(void);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void get_schema(SCHEMA*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void dump_schema(SCHEMA*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void delete_schema(SCHEMA**);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void get_data(SCHEMA *schema);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void dump_data(SCHEMA*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void get_index(SCHEMA*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void print_index(SCHEMA*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void sort_index(SCHEMA*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void insert_data(SCHEMA*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void search_index_data(SCHEMA*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void update_distances(SCHEMA*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void dump_nn(SCHEMA *schema, int number);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void save_temporary_input(SCHEMA *schema);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void get_class(SCHEMA *schema, int n);

#endif
