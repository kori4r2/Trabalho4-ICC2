#include "schema.h"

// Structs utilizadas-------------------------------------------------------------------------------------------------
typedef struct node{
	int order;
	int id;
	char *name;
	long int size;
	long int offset;
	struct node *next;
	struct node *previous;
}NODE;

struct schema{
	char *name;
	int n_elements;
	struct node *sentry;
	int size;
};

// Funcoes internas do TAD--------------------------------------------------------------------------------------------
NODE *create_node(void){
	// Aloca memoria
	NODE *node = (NODE*)malloc(sizeof(NODE));

	// Checa se foi criada adequadamente
	if(node != NULL){
		// Inicializa valores
		node->id = -1;
		node->name = NULL;
		node->size = 0;
		node->offset = 0;
		node->next = NULL;
		node->previous = NULL;
		node->order = 0;
	}

	return node;
}

void delete_node(NODE **node){
	// Checa se eh valido
	if(node != NULL && (*node) != NULL){
		// Aloca os campos que podem ter memoria alocada
		if((*node)->name != NULL) free((*node)->name);
		// Libera a memoria e coloca NULL no lugar do endereco liberado
		free(*node);
		(*node) = NULL;
	}
}

void get_node(NODE *node, char *line){
	char *aux;
	// Aux recebe o primeiro token da linha
	aux = strtok(line, DELIMITERS);
	// Esse token corresponde ao nome do elemento
	node->name = strdup(aux);
	// O segundo token corresponde ao tipo de dado do elemento
	aux = strtok(NULL, DELIMITERS);
	// Deve ser analizado qual o tipo para decidir o proximo passo
	if(strcmp(aux, STR_INT) == 0){
		// No caso de int e double, salva-se o id correspondente ao tipo e o tamanho
		node->id = INT_T;
		node->size = sizeof(int);
	}else if(strcmp(aux, STR_DOUBLE) == 0){
		node->id = DOUBLE_T;
		node->size = sizeof(double);
	}else if(strcmp(aux, STR_CHAR) == 0){
		// Caso seja um char[*], outro strtok deve ser chamado para identificar o numero de caracteres da string
		node->id = STRING_T;
		aux = strtok(NULL, DELIMITERS);
		// O numero de caracteres fica salvo no tamanho do elemento, pois sempre um produto do tamanho por sizeof(char)
		node->size = atoi(aux) * sizeof(char);
	}
/*
	// Mais uma chamada de strtok analisa se o elemento deve ser ordernado ou nao
	aux = strtok(NULL, DELIMITERS);

	if(aux != NULL && strcmp(aux, "order") == 0) node->order = 1;
*/
}

FILE *open_index(SCHEMA *schema, NODE *node, char *mode, int *size){
	FILE *fp_index;
	char *filename_index;
	long int end_file;
	

	filename_index = (char*)malloc(sizeof(char) * (strlen(schema->name) + 6 + strlen(node->name)));
	strcpy(filename_index, schema->name);
	strcat(filename_index, "-");
	strcat(filename_index, node->name);
	strcat(filename_index, ".idx");
	fp_index = fopen(filename_index, mode);
	if(fp_index == NULL){
		fprintf(stderr, "could not open file\n");
		exit(1);
	}
	fseek(fp_index, 0, SEEK_END);
	end_file = ftell(fp_index);
	fseek(fp_index, 0, SEEK_SET);
	(*size) = (int)(end_file/(node->size+sizeof(long int)));

	free(filename_index);
	return fp_index;
}

FILE *open_data(SCHEMA *schema, char *mode, int *size){
	FILE *fp_data;
	char *filename_data;
	long int end_file;

	filename_data = (char*)malloc(sizeof(char) * (strlen(schema->name)+6));
	strcpy(filename_data, schema->name);
	strcat(filename_data, ".data");
	fp_data = fopen(filename_data, mode);
	if(fp_data == NULL){
		fprintf(stderr, "could not open file\n");
		exit(1);
	}
	fseek(fp_data, 0, SEEK_END);
	end_file = ftell(fp_data);
	fseek(fp_data, 0, SEEK_SET);
	(*size) = (int)end_file/schema->size;

	free(filename_data);
	return fp_data;
}

// Recebe as informações de um elemento da stdin e salva em fp_data com dist == 0.0
void save_item(FILE *fp_data, SCHEMA *schema, int id){
	int i;
//	long int data_size = schema->size - schema->sentry->next->size - schema->sentry->previous->size;
	void *aux;
	NODE *node = schema->sentry->next;
	double dist = 0.0;

	fseek(fp_data, 0, SEEK_END);
	fwrite(&id, node->size, 1, fp_data);
//	aux = malloc(data_size);
	for(i = 0; i < schema->n_elements-2; i++){
		node = node->next;
		aux = malloc(node->size);

		if(node->id == INT_T){
			scanf("%d", (int*)(aux));
		}else if(node->id == DOUBLE_T){
			scanf("%lf", (double*)(aux));
		}else if(node->id == STRING_T){
			memset(aux, 0, node->size);
			copy_sized_string_input(stdin, aux, node->size);
//			fprintf(stdout, "string lida %s\n", (char*)(aux));
		}

		fwrite(aux, node->size, 1, fp_data);
		free(aux);
	}
	node = node->previous->previous;
//	fprintf(stdout, "hu3 : %.2lf\n", (*(double*)(aux+node->offset)) );
//	fwrite(aux, data_size, 1, fp_data);
	fwrite(&dist, sizeof(double), 1, fp_data);
}

void get_item(char **item, SCHEMA *schema, int cur_pos, int*n_elements){

	int aux_int, i;
	double aux_double;
	FILE *fp_data = open_data(schema, "rb", n_elements);
	NODE *node = schema->sentry;
	for(i = 0; i < schema->n_elements; i++){
		// Esse loop percorre todos os elementos salvos no schema e todas as linhas da tabela char**
		node = node->next;
		fseek(fp_data, (cur_pos*schema->size)+node->offset, SEEK_SET);

		// Analisa qual o tipo de dado do elemento atual
		if(node->id == INT_T){
			// O define LENGTH_ITEMS guarda o numero maximo de digitos a serem lidos por vez
			// O inteiro eh lido do arquivo e salvo numa variavel auxiliar
			fread(&aux_int, node->size, 1, fp_data);
			// E dpois eh convertido para uma string salva em item[i]
			snprintf(item[i], LENGTH_ITEMS-1, "%d", aux_int);
		}else if(node->id == STRING_T){
			// No caso de strings, a memoria eh copiada diretamente para item[i]
			fread(item[i], node->size, 1, fp_data);
		}else if(node->id == DOUBLE_T){
			// Para elementos do tipo double o procedimento eh semelhante ao de inteiros
			fread(&aux_double, node->size, 1, fp_data);
			snprintf(item[i], LENGTH_ITEMS-1, "%.2lf", aux_double);
		}
	}
	fclose(fp_data);
}

void print_item(SCHEMA *schema, char **item){

	// node armazena os detalhes do elemento sendo imprimido
	NODE *node = schema->sentry;
	int i;
	for(i = 0; i < schema->n_elements; i++){
		node = node->next;
		// Para cada elemento imprime o tipo e o que foi convertido para string com get_item
		printf("%s = %s\n", node->name, item[i]);
	}
}

void swap(FILE *fp, NODE *node, int i, int j){

	// Data_size guarda o tamanho do bloco de memoria que contem um item e um long int de offset para cada item no .idx
	long int data_size = (node->size + sizeof(long int));
	void *aux1 = malloc(data_size);
	void *aux2 = malloc(data_size);

	// A variavel aux1 armazena o elemento na "posicao i" dentro do arquivo
	fseek(fp, i * data_size, SEEK_SET);
	fread(aux1, data_size, 1, fp);
	// A variavel aux2 armazena o elemento na "posicao j"
	fseek(fp, j * data_size, SEEK_SET);
	fread(aux2, data_size, 1, fp);
	// Os conteudos de cada posicao sao reescritos com o conteudo anterior da outra posicao
	fseek(fp, j * data_size, SEEK_SET);
	fwrite(aux1, data_size, 1, fp);
	fseek(fp, i * data_size, SEEK_SET);
	fwrite(aux2, data_size, 1, fp);

	// A memoria alocada eh liberada
	free(aux1);
	free(aux2);
}

int compare_in_file(FILE *fp, NODE *node, int i, int j){

	// Assim como em swap(), data_size armazena o tamanho de cada elemento salvo no .idx
	long int data_size = (node->size + sizeof(long int));
	int result = 0;
	// As variaveis aux dessa vez armazenarao apenas o conteudo, ignorando o offset
	void *aux1 = malloc(node->size);
	void *aux2 = malloc(node->size);

	// O arquivo eh lido para obter o conteudo das "posicoes" i e j
	fseek(fp, i * data_size, SEEK_SET);
	fread(aux1, node->size, 1, fp);
	fseek(fp, j * data_size, SEEK_SET);
	fread(aux2, node->size, 1, fp);


	// De acordo com o tipo de dado sendo analisado, a comparacao eh feita de uma maneira diferente
	if(node->id == INT_T){
		if( (*((int*)aux1)) < (*((int*)aux2)) ) result = -1;
		else if( (*((int*)aux1)) > (*((int*)aux2)) ) result = 1;
	}else if(node->id == DOUBLE_T){
		if( (*((double*)aux1)) < (*((double*)aux2)) ) result = -1;
		else if( (*((double*)aux1)) > (*((double*)aux2)) ) result = 1;
	}else if(node->id == STRING_T){
		result = strcmp((char*)aux1, (char*)aux2);
	}
	// Independente do tipo, o resultado é semelhante em quesito de ordenacao ao retorno da funcao strcmp()
	// negativo quer dizer i e j estão crescentes, positivo decrescente e 0 igual

	// A memoria alocada eh liberada e o resultado da analise eh retornado
	free(aux1);
	free(aux2);

	return result;
}

int compare_outside(NODE *node, void *check, char *key){
	// valor de result eh inicializado para o caso de igualdade
	int result = 0;

	// Para cada tipo de dado desejado o char* e o void* sao convertidos de acordo para permitir a comparacao
	if(node->id == INT_T){
		if( (*((int*)check)) > atoi(key) ) result = -1;
		else if( (*((int*)check)) < atoi(key) ) result = 1;
	}else if(node->id == DOUBLE_T){
		if( (*((double*)check)) > atof(key) ) result = -1;
		else if( (*((double*)check)) < atof(key) ) result = 1;
	}else if(node->id == STRING_T){
		result = strcmp(key, (char*)check);
	}
	// O retorno da funcao se da de maneira semelhante a compare_inside_file()

	return result;
}

long int sequential_search(SCHEMA *schema, NODE *node, char *search_key, int *test_count){
	FILE *fp_data, *fp_index;
	int i, n_elements_data, n_elements_index, compare_result;
	void *aux;
	
	// O bloco de codigo abaixo abre o arquivo .data para leitura, checa se a abertura foi feita corretamente e analisa quantos elementos
	// estao contidos no arquivo (seria melhor implementar uma funcao para isso, mas n ha tempo agora)

	fp_data = open_data(schema, "rb", &n_elements_data);

	// O bloco de codigo abaixo faz o mesmo que o acima, mas dessa vez para o arquivo .idx

	fp_index = open_index(schema, node, "rb", &n_elements_index);

	// As variaveis auxiliares sao inicializadas
	aux = malloc(node->size);
	// O valor inicial de i faz com que a busca se inicie apos os elementos que ja haviam sido adicionados ao indice
	i = n_elements_index;
	compare_result = 1;

	// Enquanto n for encontrado ou sejam verificados todos, entra no loop
	while(i < n_elements_data && compare_result != 0){
		// O contador de iteracoes eh incrementado
		(*test_count)++;
		// Para cada repeticao procura a posicao do dado a ser analisado
		fseek(fp_data, (i*schema->size)+node->offset, SEEK_SET);
		// Guarda o resultado em uma posicao da memoria e compara com a chave procurada fora do arquivo
		fread(aux, node->size, 1, fp_data);
		compare_result = compare_outside(node, aux, search_key);
		// incrementa o passo
		i++;
	}
	// Liberacao da memoria alocada
	free(aux);
	fclose(fp_data);
	fclose(fp_index);

	// Caso haja sido encontrado o item buscado, retorna a posicao do registro que o contem dentro do .data
	if(compare_result == 0) return ((i-1)*schema->size);
	// Caso contrario, retorna -1
	else return -1;
}

long int binary_search(FILE *fp, SCHEMA *schema, NODE *node, char *search_key, int begin, int end, int *test_count){

	// A condicao de parada da busca eh estabelecida para depois que ela checa um bloco que contem apenas um item
	// E o retorno para quando o item n for encontrado eh -1
	if(begin > end) return -1;

	// Define-se o ponto medio
	int middle = (begin+end)/2, compare_result;
	long int result;
	// Aux recebe o conteudo do ponto medio
	void *aux = malloc(node->size);
	fseek(fp, middle*(node->size + sizeof(long int)), SEEK_SET);
	fread(aux, node->size, 1, fp);

	// Eh feita a comparacao com a chave buscada fora do arquivo
	compare_result = compare_outside(node, aux, search_key);
	// O contador de iteracoes eh incrementado
	(*test_count)++;
	// E a memoria alocada eh liberada
	free(aux);
	// De acordo com o resultado da comparacao, chama binary_search() recursivamente ou armazena a localizacao do item no
	// .data para ser retornado
	if(compare_result < 0){
		result = binary_search(fp, schema, node, search_key, begin, middle-1, test_count);
	}else if(compare_result > 0){
		result = binary_search(fp, schema, node, search_key, middle+1, end, test_count);
	}else{
		fread(&result, sizeof(long int), 1, fp);
	}
	return result;
}

// Funcoes utilizadas do TAD------------------------------------------------------------------------------------------

SCHEMA *create_schema(void){
	SCHEMA *schema = (SCHEMA*)malloc(sizeof(SCHEMA));
	// Verifica se a criacao foi bem sucedida
	if(schema != NULL){
		// Cria o "no cabeca"
		NODE *sentry = create_node();
		// Caso seja bem sucedida, inicializa os valores
		if(sentry != NULL){
			sentry->next = sentry;
			sentry->previous = sentry;
			schema->name = NULL;
			schema->n_elements = 0;
			schema->sentry = sentry;
			schema->size = 0;
		// Caso contrario, libera memoria e retorna NULL
		}else{
			free(schema);
			schema = NULL;
		}
	}else{
		fprintf(stderr, "error creating schema\n");
		exit(3);
	}

	return schema;
}

char **read_schema(int *n_elements){
	int ending, size = 0;
	FILE *fp;
	char *aux, *filename;
	char **table = NULL;

	// Le o nome do arquivo a ser aberto para leitura da stdin
	filename = my_get_line_valid(stdin, &ending);
	fp = fopen(filename, "r");
	// Checa se foi aberto de maneira correta
	if(fp == NULL){
		fprintf(stderr, "could not open file\n");
		free(filename);
		exit(1);
	}

	// Le o arquivo linha por linha
	do{
		aux = my_get_line_valid(fp, &ending);

		// Caso a linha seja lida corretamente, salva o que foi lido em uma tabela de char**
		if(aux != NULL){
			table = (char**)realloc(table, (size+1) * sizeof(char*));
			table[size++] = aux;
		}
	}while(ending >= 0); // Se ending eh positivo, ainda nao foi atingido o fim do arquivo

	// Registra quantos elementos o registro contem, tendo em vista que a primeira linha contem o nome do arquivo
	(*n_elements) = size+1;

	// Precaucao
	if(size == 0){
		fprintf(stderr, "error reading .schema\n");
		exit(2);
	}

	// Libera memoria alocara e retorna as linhas lidas
	fclose(fp);
	free(filename);
	return table;
}

void get_schema(SCHEMA *schema){
	// Verifica se o schema foi criado adequadamente
	if(schema != NULL){
		// Caso tenha sido, usa a funcao read_schema para ler as linhas do arquivo .schema indicado pela stdin e armazena quantos elementos
		// o schema contera
		NODE *new_node;
		int n_elements, i;
		long int offset;
		char *aux;
		char **table = read_schema(&n_elements);

		// A primeira linha é analizada para obter-se o nome do arquivo e o numero de elementos lido é armazenado
		aux = strtok(table[0], DELIMITERS);
		aux = strtok(NULL, DELIMITERS);
		schema->name = strdup(aux);
		schema->n_elements = n_elements;
		// O tamanho eh inicializado como 0
		schema->size = 0;

		// Cria um no correspondente ao id
		new_node = create_node();
		if(new_node == NULL){
			fprintf(stderr, "error creating node\n");
			exit(3);
		}
		new_node->name = strdup("id");
		new_node->id = INT_T;
		new_node->size = sizeof(int);
		// E insere no inicio da lista criada
		new_node->next = schema->sentry;
		new_node->previous = schema->sentry->previous;
		schema->sentry->previous->next = new_node;
		schema->sentry->previous = new_node;
		schema->size += new_node->size;


		// Cria um no para cada elemento de acordo com o numero de linhas lidas
		offset = new_node->size;
		for(i = 1; i < n_elements-1; i++){
			new_node = create_node();
			if(new_node == NULL){
				fprintf(stderr, "error creating node\n");
				exit(3);
			}

			// Obtem as informacoes do elemento a partir da linha atual
			get_node(new_node, table[i]);
			schema->size += new_node->size;
			new_node->offset = offset;
			offset += new_node->size;

			// Insere o elemento no final da lista criada
			new_node->next = schema->sentry;
			new_node->previous = schema->sentry->previous;
			schema->sentry->previous->next = new_node;
			schema->sentry->previous = new_node;
		}
		// Cria um no correspondente a distancia
		new_node = create_node();
		if(new_node == NULL){
			fprintf(stderr, "error creating node\n");
			exit(3);
		}
		new_node->name = strdup("dist");
		new_node->id = DOUBLE_T;
		new_node->size = sizeof(double);
		new_node->order = 1;
		new_node->offset = offset;
		// E insere no final da lista criada
		schema->size += new_node->size;
		new_node->next = schema->sentry;
		new_node->previous = schema->sentry->previous;
		schema->sentry->previous->next = new_node;
		schema->sentry->previous = new_node;

		// Libera a memoria alocada
		free_string_list(table, n_elements-1);

		// Cria os arquivos index necessarios de acordo com o .schema e o .data
//		get_index(schema);
//		sort_index(schema);
	}
}

void delete_schema(SCHEMA **schema){
	// Verifica se é um endereco valido
	if(schema != NULL && (*schema) != NULL){
		NODE *aux;
		// Deleta todos os nós criados para armazenar os elementos do registro
		while((*schema)->n_elements > 0){
			aux = (*schema)->sentry->previous;
			aux->next->previous = aux->previous;
			aux->previous->next = aux->next;
			delete_node(&aux);
			// Descresce o tamanho
			(*schema)->n_elements--;
		}
		// Deleta o no sentinela
		delete_node(&((*schema)->sentry));
		// Libera o nome se necessario
		if((*schema)->name != NULL) free((*schema)->name);
		// Libera o schema em si e seta NULL
		free(*schema);
		(*schema) = NULL;
	}
}

void dump_schema(SCHEMA *schema){
	if(schema != NULL){
		int i;
		NODE *node = schema->sentry;

		// Imprime as informacoes do registro como um todo
		printf("%s %s(%d bytes)\n", STR_TABLE, schema->name, schema->size);
		// Depois percorre todos os elementos com a variavel aux
		for(i = 0; i < schema->n_elements; i++){
			node = node->next;
			// Analisa o tipo do elemento e imprime as informacoes de acordo com o que foi lido
			if(node->id == INT_T){
				printf("%s %s", node->name, STR_INT);
			}else if(node->id == DOUBLE_T){
				printf("%s %s", node->name, STR_DOUBLE);
			}else if(node->id == STRING_T){
				printf("%s %s[%ld]", node->name, STR_CHAR, (node->size/(int)sizeof(char)));
			}
/*
			// Analisa se é necessario imprimir order e depois imprime o tamanho do elemento em bytes
			if(aux->order) printf(" %s", STR_ORDER);
*/
			printf("(%ld bytes)\n", node->size);
		}
	}
}

void get_data(SCHEMA *schema){
	int aux, id;
	FILE *fp_data = open_data(schema, "wb", &aux);

	scanf("%d", &id);
	while(id != -1){
		save_item(fp_data, schema, id);
		scanf("%d", &id);
	}
	fclose(fp_data);
}

void dump_data(SCHEMA *schema){

	int i, n_elements;
	// A variavel item armazenara as informacoes do elemento sendo lido
	char **item = (char**)malloc(schema->n_elements * sizeof(char*));
	// Mais uma vez o bloco responsavel por abrir o arquivo .data para leitura e obter o numero de elementos
	NODE *node = schema->sentry;

	// Todos os campos de item tem memoria alocada
	for(i = 0; i < schema->n_elements; i++){
		node = node->next;
		if(node->id == INT_T || node->id == DOUBLE_T){
			item[i] = (char*)malloc(LENGTH_ITEMS * sizeof(char));
		}else if(node->id == STRING_T){
			item[i] = (char*)malloc(node->size);
		}
	}

	// n_elements sao lidos do arquivo .data e imprimidos na stdout de acordo com o que foi definido na funcao print_item()
	i = 0;
	do{
		get_item(item, schema, i, &n_elements);
		print_item(schema, item);
		i++;
	}while(i < n_elements);

	// A memoria alocada é liberada
	free_string_list(item, schema->n_elements);
}

void copy_data_from_file(FILE *origin, FILE *destiny, SCHEMA *schema, NODE *node, int n_elements){

	long int location;
	int i;
	// Cada elemento a ser salvo no arquivo de destino (.idx) deve conter um elemento do registro e um long int contendo o offset do registro
	// dentro do arquivo de origem
	int data_size = (int)sizeof(long int) + node->size;
	void *aux = malloc(data_size);

	// Percorre o arquivo de origem lendo o elemento correspondente ao no e escrevendo-o no arquivo de destino
	// Juntamente com a posicao original do registro que contem o elemento lido
	for(i = 0; i < n_elements; i++){
		// cur_offset guarda o offset do elemento em relacao ao registro como um todo
		fseek(origin, (i * schema->size) + node->offset, SEEK_SET);
		fread(aux, node->size, 1, origin);
		location = (long int)(i * schema->size);
		// As duas informacoes sao salvas uma diretamente apos a outra
		memcpy(aux+(node->size), &location, sizeof(long int));
		fwrite(aux, data_size, 1, destiny);
	}

	// Liberacao de memoria alocada
	if(aux != NULL) free(aux);
}

void get_index(SCHEMA *schema){

	int i, n_elements;
	NODE *node = schema->sentry;
	FILE *fp_data, *fp_index;

	// O registro é percorrido analisando se o elemento deve ser ordenado
	for(i = 0; i < schema->n_elements; i++){
		node = node->next;
		if(node->order){
			// E o arquivo .idx é criado para escrita
			fp_index = open_index(schema, node, "wb", &n_elements);

			// Caso seja necessario, o arquivo .data é aberto para leitura
			fp_data = open_data(schema, "rb", &n_elements);

			// A funcao copy_data_from_file() é utilizada para copiar a memoria diretamente de um arquivo para o outro
			copy_data_from_file(fp_data, fp_index, schema, node, n_elements-1);

			// E toda a memoria alocada é liberada
			fclose(fp_data);
			fclose(fp_index);
		}
	}
}

void print_index(SCHEMA *schema){

	int i, j, n_elements;
	long int location;
	void *aux;
	NODE *node = schema->sentry;
	FILE *fp_index;

	// Percorre todos os elementos do registro checando se foram ordenados
	for(i = 0; i < schema->n_elements; i++){
		node = node->next;
		if(node->order){
			// Caso tenham sido ordenados, abre o arquivo .idx correspondente para leitura
			fp_index = open_index(schema, node, "rb", &n_elements);

			// O arquivo é lido um a um ate o final
			for(j = 0; j < n_elements; j++){
				// aux armazena o dado a ser impresso
				aux = malloc(node->size);
				fread(aux, node->size, 1, fp_index);
				// enquanto location armazena a localizacao salva logo apos
				fread(&location, sizeof(long int), 1, fp_index);

				// De acordo com o timpo de dado sendo analisado, a impressao é feita na stdout
				if(node->id == INT_T){
					printf("%d = %ld\n", *((int*)aux), location);
				}else if(node->id == DOUBLE_T){
					printf("%.2lf = %ld\n", *((double*)aux), location);
				}else if(node->id == STRING_T){
					printf("%s = %ld\n", (char*)aux, location);
				}
				// A memoria alocada é liberada a medida que é necessario
				free(aux);
			}
			fclose(fp_index);
		}
	}
}

void sort_index(SCHEMA *schema){

	int i, j, k, n_elements;
	long int location;
	NODE *node = schema->sentry;
	FILE *fp_index;

	// Percorre todos os elementos do registro checando se precisam ser ordenados
	for(i = 0; i < schema->n_elements; i++){
		node = node->next;
		if(node->order){
			// Caso seja o caso, o arquivo .idx correspondete é aberto para atualização
			fp_index = open_index(schema, node, "r+b", &n_elements);

			// Analisa-se qual o numero de elementos salvos dentro do arquivo .idx
			fseek(fp_index, 0, SEEK_END);
			location = ftell(fp_index);
			n_elements = (int)(location/(sizeof(long int) + node->size));
			fseek(fp_index, 0, SEEK_SET);

			// Utiliza-se um algoritmo de insertion sort e as funções compare_in_file() e swap() para aplicá-lo
			for(j = 1; j < n_elements; j++){

				for(k = j-1; (k >= 0) && (compare_in_file(fp_index, node, k, k+1) > 0); k--){
					swap(fp_index, node, k, k+1);
				}

			}
			// O arquivo é fechado
			fclose(fp_index);
		}
	}
}

void insert_data(SCHEMA *schema){

	int i, n_elements_data;
	double dist = 0;
	void *aux;
	NODE *node = schema->sentry;
	// O arquivo .data é aberto para atualizacao
	FILE *fp_data = open_data(schema, "r+b", &n_elements_data);
	// E o ponteiro é movido para o fim do arquivo
	fseek(fp_data, 0, SEEK_END);
	// O registro é percorrido analisando qual o tipo de dado a ser salvo na ordem desejada
	for(i = 0; i < schema->n_elements-1; i++){
		node = node->next;
		// aux armazena os dados a serem escritos
		aux = malloc(node->size);
		memset(aux, 0, node->size);

		// De acordo com o tipo sendo analisado, as informações são lidas da stdin e armazendas em aux
		if(node->id == INT_T){
			scanf("%d", (int*)aux);
		}else if(node->id == DOUBLE_T){
			scanf("%lf", (double*)aux);
		}else if(node->id == STRING_T){
			free(aux);
			aux = (void*)my_gets(stdin, (node->size/sizeof(char)));
			if(aux == NULL) fprintf(stderr, "error reading string\n");
		}
		// As informacoes sao entao escritas no final do arquivo .data
		fwrite(aux, node->size, 1, fp_data);
		// A memoria alocada é liberada
		if(aux != NULL) free(aux);
	}
	fwrite(&dist, node->size, 1, fp_data);
	// E o arquivo é fechado
	fclose(fp_data);
}

void search_index_data(SCHEMA *schema){

	int i, test_count, search_return, n_elements;
	long int location, offset;
	void *aux;
	char *filename_index, *search_term, *filename_data, *print_field, *search_key;
	NODE *node = schema->sentry;
	FILE *fp_index, *fp_data;

	// Sao lidos da stdin o campo a ser analisado na busca, a chave desejada e o campo a ser impresso apos a busca
	search_term = my_get_line_valid(stdin, &i);
	search_key = my_get_line_valid(stdin, &i);
	print_field = my_get_line_valid(stdin, &i);

	// test_count armazena o numero de iteracoes de busca e offset armazena o offset do tipo sendo analisado em relação á posição do elemento
	// como um todo
	test_count = 0;
	offset = 0;
	// Sao analizados todos os elementos ate ser encontrado o que deseja-se para a busca
	for(i = 0; i < schema->n_elements; i++){
		node = node->next;
		if(strcmp(search_term, node->name) == 0 && node->order){

			// O arquivo .idx é aberto para leitura
			filename_index = (char*)malloc(sizeof(char) * (strlen(schema->name) + 6 + strlen(node->name)));
			strcpy(filename_index, schema->name);
			strcat(filename_index, "-");
			strcat(filename_index, node->name);
			strcat(filename_index, ".idx");
			fp_index = fopen(filename_index, "rb");

			if(fp_index != NULL){

				// Analisa-se quantos elementos estão presentes no  .idx
				fseek(fp_index, 0, SEEK_END);
				location = ftell(fp_index);
				n_elements = (int)(location/(sizeof(long int) + node->size));
				fseek(fp_index, 0, SEEK_SET);

				// É realizada a busca binaria
				search_return = binary_search(fp_index, schema, node, search_key, 0, n_elements-1, &test_count);
				// E o arquivo é fechado
				fclose(fp_index);
				if(search_return == -1){
					// Caso não tenha sido encontrado o item desejado no indice, realiza-se a busca sequencial
					// nos itens que foram adicionados no fim do .data
					search_return = sequential_search(schema, node, search_key, &test_count);
				}
			}else{
				fprintf(stderr, "could not open file\n");
				exit(1);
			}
			// Liberacao da memoria alocada
			free(filename_index);

			// Imprime o numero de iteracoes da busca realizadas
			printf("%d\n", test_count);
			// Caso nao tenha sido encontrado o item, imprime a mensagem de erro
			if(search_return == -1){
				printf("value not found\n");
			}else{
				// Caso o item tenha sido encontrado, procura o offset do item a ser impresso em relação
				// ao elemento como um todo
				node = schema->sentry;
				offset = 0;
				for(i = 0; i < schema->n_elements; i++){
					node = node->next;
					if(strcmp(print_field, node->name) == 0){
						break;
					}
					offset += node->size;
				}

				// O arquivo .data é aberto para leitura
				filename_data = (char*)malloc(sizeof(char) * (strlen(schema->name)+6));
				strcpy(filename_data, schema->name);
				strcat(filename_data, ".data");
				fp_data = fopen(filename_data, "rb");
				if(fp_data == NULL){
					fprintf(stderr, "could not open file\n");
					exit(1);
				}

				// A variavel aux armazena o conteudo do item a ser impresso
				fseek(fp_data, search_return+offset, SEEK_SET);
				aux = malloc(node->size);
				fread(aux, node->size, 1, fp_data);
				// E o arquivo é fechado
				fclose(fp_data);

				// A impressao é feita de acordo com o tipo sendo analisado
				if(node->id == INT_T){
					printf("%d\n", *((int*)aux));
				}else if(node->id == DOUBLE_T){
					printf("%.2lf\n", *((double*)aux));
				}else if(node->id == STRING_T){
					printf("%s\n", (char*)aux);
				}
				// E a memoria alocada é liberada
				free(aux);
				free(filename_data);
			}
			// caso o item seja encontrado, o valor de i é alterado para que saia do loop
			i = schema->n_elements+1;
		}
		// O valor de offset é incrementado com o tamanho do item anterior analisado
		if(i <= schema->n_elements){
			offset += node->size;
		}
	}

	// Caso não tenha sido realizada nenhuma iteracao de busca, não foi encontrado nenhum arquivo .idx do campo buscado
	if(test_count == 0){
		printf("index not found\n");
	}
	// Libera a memoria alocada
	free(search_term);
	free(search_key);
	free(print_field);
}

void get_distance(FILE *fp_data, SCHEMA *schema, int n_elements, long int cur_offset){
	NODE *node = schema->sentry->next;
	int i;
	void *aux1 = malloc(sizeof(double));
	void *aux2 = malloc(sizeof(double));
	double distance = 0;

	for(i = 0; i < schema->n_elements-1; i++){
		node = node->next;
		if(node->id == INT_T || node->id == DOUBLE_T){
			fseek(fp_data, cur_offset+node->offset, SEEK_SET);
			fread(aux1, node->size, 1, fp_data);
			fseek(fp_data, ((n_elements-1)*schema->size)+node->offset, SEEK_SET);
			fread(aux2, node->size, 1, fp_data);
		}
		if(node->id == INT_T){
			distance += pow((double)( (*((int*)aux1)) - (*((int*)aux2)) ), 2);
		}else if(node->id == DOUBLE_T){
			distance += pow( (*((double*)aux1)) - (*((double*)aux2)), 2);
		}
	}
	free(aux1);
	free(aux2);
	distance = sqrt(distance);
	fseek(fp_data, cur_offset+node->offset, SEEK_SET);
	fwrite(&distance, node->size, 1, fp_data);
}

void update_distances(SCHEMA *schema){
	int i, n_elements;
	FILE *fp_data = open_data(schema, "r+b", &n_elements);

	for(i = 0; i < n_elements; i++){
		get_distance(fp_data, schema, n_elements, i*schema->size);
	}

	fclose(fp_data);
}

void dump_nn(SCHEMA *schema, int number){
	int i, n_elements;
	long int location;

	FILE *fp_index = open_index(schema, schema->sentry->previous, "rb", &n_elements);
	char **item = (char**)malloc(schema->n_elements * sizeof(char*));

	NODE *node = schema->sentry;
	for(i = 0; i < schema->n_elements; i++){
		node = node->next;
		if(node->id == INT_T || node->id == DOUBLE_T){
			item[i] = (char*)malloc(LENGTH_ITEMS * sizeof(char));
		}else if(node->id == STRING_T){
			item[i] = (char*)malloc(node->size);
		}
	}

	i = 0;
	do{
		// Para encontrar a localização do elemento mais proximo, ja se sabe que o tamano da informacao salva eh de double
		fseek(fp_index, (i*(sizeof(double)+sizeof(long int))) + sizeof(double), SEEK_SET);
		fread(&location, sizeof(long int), 1, fp_index);
		get_item(item, schema, location/schema->size, &n_elements);
		print_item(schema, item);
		i++;
	}while(i < n_elements && i < number);

	fclose(fp_index);
	free_string_list(item, schema->n_elements);
}
