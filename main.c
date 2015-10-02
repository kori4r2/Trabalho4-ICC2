#include <stdio.h>
#include <stdlib.h>
#include "schema.h"

int main(int argc, char *argv[]){

	int repeat, aux;
	char *input = NULL;

	// A lista schema é criada e lida da stdin
	SCHEMA *schema = create_schema();
	get_schema(schema);
	get_data(schema);

	do{
		// A cada repeticao le um comando da stdin
		repeat = 1;
		
		while(input == NULL){
			input = my_get_line_valid(stdin, &aux);
			if(aux == -2){
				fprintf(stderr, "End of file reached\n");
				exit(5);
			}
		}

		// Analisa qual o comando desejado e chama a(s) funcao(oes) responsavel(is) por realiza-lo
		if(strcmp(input, "dump_schema") == 0){
			dump_schema(schema);
		}else if(strcmp(input, "dump_data") == 0){
			dump_data(schema);
		}else if(strcmp(input, "dump_nn") == 0){
			scanf("%d", &aux);
			save_temporary_input(schema);
			update_distances(schema);
			get_index(schema);
			sort_index(schema);
			dump_nn(schema, aux);
		}else if(strcmp(input, "knn") == 0){
//			knn(schema);
		}else if(strcmp(input, "exit") == 0){
			// Caso seja digitado "exit", repeat recebe 0, saindo do loop
			repeat = 0;
		}

		// A cada repeticao input eh liberado caso tenha sido alocado adequadamente
		if(input != NULL){
			free(input);
			input = NULL;
		}
	}while(repeat);

	// Libera a memoria alocada
	delete_schema(&schema);

	return 0;
}
