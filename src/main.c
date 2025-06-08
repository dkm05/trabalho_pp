#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "tokens.h"
#include "hashmap.h"

void remove_space(char str[]);
// futuramente o buffer tem que ficar na heap
#define BUFFER_SIZE 4096
#define ARRLEN(arr) ((int) (sizeof(arr) / sizeof(arr[0])))
// pensar em um nome
#define is_token_char(c) (isalnum(c) || (c) == '_')
#define is_quote(str, i, c) ((i) && (c) == '"' && (str)[(i) - 1] != '\'' && (str)[(i) - 1] != '\\')
#define line_splicing(c1, c2) (c1) == '\\' && (c2) == '\n'

/* global variables */
bool is_string_ = false;
int numero_de_macros=0;
int tamanho_vetor_de_macros=0;
Macro **vetor_macro=NULL;

void inserir_macro(Macro* m);







void
die(const char *str)
{
        fprintf(stderr, "%s\n", str);
        exit(EXIT_FAILURE);
}

// pode ser macro
bool
is_string(char str[], size_t i)
{
        if (is_quote(str, i, str[i]))
                is_string_ = !is_string_;
        return is_string_;
}

void
organize_buffer(char *str)
{
        int pos = 0;
        for (int i = 0; i < BUFFER_SIZE; i++) {
                if (str[i] != '\0') {
                        str[pos++] = str[i];
                        if (i != pos - 1)
                                str[i] = '\0';
                }
        }
}

void
print_line(FILE *f, char str[])
{
        for (int i = 0; str[i] != '\0'; i++) {
                putc(str[i], f);
        }
}

void
ignore_until_newline(char str[], size_t *i)
{
        while (str[*i] != '\n')
                str[(*i)++] = '\0';
}

void
ignore_until_end_comment(char str[], size_t *i)
{
        char ant = '\0';
        while (true) {
                if (str[*i] == '/' && ant == '*') {
                        str[*i] = '\0';
                        break;
                }
                ant = str[*i];
                str[(*i)++] = '\0';
        }
}

/* retorna true se a junção de 2 chars
 * formam um novo token
 */
bool
is_new_token(char c1, char c2)
{
        if (is_token_char(c1) && is_token_char(c2))
                return true;
        
        char token[3] = {c1, c2, '\0'};
        // existem formas melhores de fazer isso
        // (hash table talvez?)
        for (int i = 0; i < ARRLEN(operators); i++) {
                if (!strcmp(token, operators[i]))
                        return true;
        }

        return false;
}

/* ideia: coloca todo o arquivo dentro de uma string
 * e remove os comentarios no processo
 * (bonus: nao é o ideal mas pode fazer o line_splicing aqui)
 */
void
remove_comments(char str[])
{
        size_t len = strlen(str);
        for (size_t i = 1; i < len; i++) {
                if (is_string(str, i)) {
                        continue;
                } else if (i && str[i - 1] == '/') {
                        if (str[i] == '/') {
                                str[i - 1] = ' ';
                                ignore_until_newline(str, &i);
                        } else if (str[i] == '*') {
                                str[i - 1] = ' ';
                                ignore_until_end_comment(str, &i);
                        }
                } 
        }
        organize_buffer(str);
}

void
read_file(FILE *fp, char str[])
{
        char c;
        int i = 0;
        while ((c = getc(fp)) != EOF) {
                if (i && line_splicing(str[i - 1], c))
                        i--;
                else
                        str[i++] = c;
        }
        str[i] = '\0';
}

void
remove_trailling_space(char str[])
{
        size_t len = strlen(str);
        for (size_t i = 1; i < len; i++) {
                if (is_string(str, i))
                        continue;
                if (isblank(str[i]) && isblank(str[i - 1]))
                        str[i - 1] = '\0';
        }
        organize_buffer(str);
}

// quando ler um #, passa a posição do # no i, e a string
bool is_macro(int i,const char str[],int *ret){
        char aux[7]={'\0'};
        int i_aux=i;    //guarda a posição do #
        i++;
        while(str[i] == ' '){ //pula espaços provenientes da remoção de comentários
                i++;
        }
        *ret=i-i_aux-1;
        int k;
        if(i_aux == 0){
                puts("ok is macro 1");
                for(k=0;k<6;k++){
                        printf("lido: %c\n",str[i]);
                        aux[k]=str[i++];
                        printf("escrito: %c\n",aux[k]);
                }
                printf("diretorio lido: %s\n",aux);
                if(strcmp(aux,"define")==0){
                        puts("ok is macro 3");
                        return true;
                }
        }else if(i_aux > 0 && str[i_aux-1] == '\n'){
                puts("ok is macro 4");
                for(k=0;k<6;k++){
                        printf("lido: %c\n",str[i]);
                        aux[k]=str[i++];
                        printf("escrito: %c\n",aux[k]);
                }
                printf("diretorio lido: %s\n",aux);
                if(strcmp(aux,"define") == 0){
                        puts("ok is macro 6");
                        return true;
                }
        }
        puts("ok is macro 7");
        return false;
}
// i é a posição do primeiro char do macro
bool is_simple_macro(int i,const char str[]){
        int length=(int)strlen(str);
        int k=0;
        puts("is simple 1");
        //printf("primeiro char: %c\n",str[i+k]);
        while(str[i+k] != ' ' && str[i+k] != '(' && (i+k)<length && str[i+k] != '\t'){
                //printf("%c\n",str[i+k]);
                k++;
        }
        puts("is simple 2");
        //printf("%c\n",str[i+k]);
        if(str[i+k] == '('){
                puts("is simple 2,5");
                return false;
        }
        puts("is simple 3");
        return true;
}
// assume que i é a posição do primeiro char depois de "define"
void save_macro(int i, const char str[]){
        int k=0;
        int j=0;
        Macro* temp = malloc(sizeof(Macro));
        
        if (!temp) {
        die("Falha ao alocar memória para Macro");
    }  
        temp->disponibilidade = OCUPADO;
        puts("save macro 1");
        while((str[i] == ' ' || str[i] == '\t' )&& i<(int)strlen(str)){
                printf("%c\n",str[i]);
                i++;
        }
        //printf("%c\n",str[i]);
        puts("save macro 1,5");
        if(is_simple_macro(i,str)){
                puts("save macro 2");
                temp->simples=1;
                temp->qtd_param=0;
                while(str[i+k]!=' ' && str[i+k]!='\t' ){
                        temp->id[j]=str[i+k];
                        k++;j++;
                }
                k++; // pula o ' '
                while(j<BUFFER_SIZE){
                        temp->id[j++]='\0';
                }
                printf("id[%d]: %s\n",numero_de_macros,temp->id);
                j=0;
                while (str[i+k] != '\n' && str[i+k] != '\0'){
                        temp->value[j]=str[i+k];
                        k++;j++;
                }
                puts("save macro 5");
                for(int z=j;z<BUFFER_SIZE;z++){
                        temp->value[z]='\0';
                }
                organize_buffer(temp->value);
                
                puts("save macro 5,5");
                printf("value[%d]: %s\n",numero_de_macros,temp->value);        //teste
                puts("save macro 6");
                //remove_space(temp->value);
                temp->id[sizeof(temp->id) - 1] = '\0';
                temp->value[sizeof(temp->value) - 1] = '\0';
                inserir_macro(temp);
                puts("save macro 7");
        }else{
                puts("macro com parametros");
                temp->simples=0;
                temp->qtd_param=1;
                while(str[i+k]!='(' ){
                        temp->id[j]=str[i+k];
                        k++;j++;
                }
                puts(" '(' achado, id lido");
                printf("'(' lido: %c\n",str[i+k]);
                printf("id lido: %s\n",temp->id);
                while(j<BUFFER_SIZE){
                        temp->id[j++]='\0';
                }
                printf("id lido apos adicionar '0': %s\n",temp->id);
                k++;    // pula o "("
                int aux =k;
                while(str[i+k] != ')' && str[i + k] != '\0' ){
                        if(str[i+k] == ','){
                                puts("',' achado");
                                temp->qtd_param++;
                        }
                        k++;

                }
                printf(" %d parametros contados\n",temp->qtd_param);
                k=aux;  //k volta para depois de '('
                temp->parametros =(char**) malloc(sizeof(char *) * temp->qtd_param);
                puts("parametros alocados");
                char parametrotemp[BUFFER_SIZE]={'\0'};
                j=0;            //pos em parametros
                int n=0;        //parametro n
                while(str[i+k-1]!=')' && str[i + k-1] != '\0' ){
                        if(str[i+k] != ',' && str[i+k] != ')' ){
                                printf("salvando %c como parametrotemp\n",str[i+k]);
                                parametrotemp[j]=str[i+k];
                                k++;j++;
                        }else{
                                printf("parametrotemp lido: %s\n", parametrotemp);
                                while(j<BUFFER_SIZE){
                                        parametrotemp[j++]='\0';
                                }
                                puts("macro com parametros 9");
                                temp->parametros[n]= strdup(parametrotemp);
                                printf("parametrotemp salvo no vetor: %s\n",temp->parametros[n]);
                                n++;
                                memset(parametrotemp, 0, sizeof(parametrotemp));
                                puts("parametrotemp copiado e limpo");
                                k++;    //pula a virgula
                                j=0;    //volta pro inicio
                        }
                }
                puts("macro com parametros 11");
                while (j<BUFFER_SIZE){
                        parametrotemp[j++]='\0';
                }
                puts("macro com parametros 12");
                temp->parametros[n]= strdup(parametrotemp);
                k++;    //pula ')'
                j=0;    //pos do corpo
                puts("macro com parametros 13");
                while (str[i + k] == ' ') k++;  //pula espaços
                puts("macro com parametros 14");
                while (str[i + k] != '\0' && str[i + k] != '\n') {
                        temp->value[j++] = str[i + k++];
                }
                printf("valor lido: %s\n",temp->value);        //parou aqui
                // while (j<BUFFER_SIZE){}{
                //         temp->value[j++]='\0';
                // }
                // puts("macro com parametros 16");
                //remove_space(temp->value);
                // for(int z=0;z<=n;z++){
                //         remove_space(temp->parametros[z]);
                // }
                temp->id[sizeof(temp->id) - 1] = '\0';
                puts("macro com parametros 17");
                temp->value[sizeof(temp->value) - 1] = '\0';
                puts("macro com parametros 18");
                inserir_macro(temp);
                puts("macro com parametros 19");
        }
}


void inserir_macro(Macro *m) {
    // Se for a primeira macro, aloca espaço
    if (numero_de_macros == 0) {
        tamanho_vetor_de_macros = 1;
        vetor_macro = malloc(sizeof(Macro *));
        if (!vetor_macro) {
            die("Falha ao alocar memória para vetor_macro");
        }
    }
    // Se o vetor estiver cheio, dobra o tamanho
    else if (numero_de_macros >= tamanho_vetor_de_macros) {
        tamanho_vetor_de_macros *= 2;
        Macro **temp = realloc(vetor_macro, sizeof(Macro *) * tamanho_vetor_de_macros);
        if (!temp) {
            die("Falha ao realocar vetor_macro");
        }
        vetor_macro = temp;
    }

    // Insere a macro no vetor
    vetor_macro[numero_de_macros] = m;
    numero_de_macros++;
}

void
remove_space(char str[])
{
        remove_trailling_space(str);
        int col = 0;
        size_t len = strlen(str);
        if (isspace(str[0]))
                str[0] = '\0';
        for (size_t i = 1; i < len - 1; i++) {
                if (is_string(str, i))
                        continue;

                bool is_space_now = isblank(str[i]);
                if (col == 0) {
                        // ignora espaco no comeco da linha
                        if (is_space_now) {
                                str[i] = '\0';
                                continue;
                        }
                } else if (is_space_now) {
                        if (is_new_token(str[i - 1], str[i + 1]))
                                continue;
                        else
                                str[i] = '\0';
                } 
                if (str[i] == '\n') {
                        col = 0;
                } else {
                        col++;
                }
        }
        organize_buffer(str);
}

void
get_name(char str[], size_t len)
{
        if (isdigit(str[0]))
                return;
        char word[len + 1];
        memcpy(word, str, len);
        word[len] = '\0';
        // talvez podemos usar o atoll
        // ja que o '.' não é um caractere valido
        // para nomear um token
        double d = atof(word);
        // if (!d)
        //         printf(" get name: \n%s\n", word);
}

void
substituir_macros(char str[])
{
        size_t word_len = 0, len = strlen(str);
        for (size_t i = 0; i < len; i++) {
                if (is_string(str, i))
                        continue;
                if (is_token_char(str[i])) {
                        word_len++;
                } else if (word_len > 0) {
                        get_name(str + i - word_len, word_len);
                        word_len = 0;
                }
        }
}
//acha macros, escreve eles num hash, e apaga da string substituindo os caracteres por \0
void find_macros(char str[]){
        puts("ok 1");
        int i=0;
        int ret;
        int length = (int)strlen(str);  
        //char aux[BUFFER_SIZE]={'\0'};
        for(i=0;i<length;i++){
                if(str[i] == '#'){
                        //printf("ok loop %d\n",i);
                        if(is_macro(i,str,&ret)){
                                puts("ok 4");
                                save_macro(i+7+ret,str);//pos do # + 7 digitos(define) + ret(consequentes espaços pulados em is_macro)
                                puts("ok 5");
                                int aux =i;
                                while( str[aux]!='\n' && aux < length){
                                        //printf("%c\n",str[aux]);
                                        str[aux]='\0';
                                        aux++;                    //tira os defines do arquivo
                                }
                                puts("ok 5,5");
                                organize_buffer(str);
                                puts("ok 6");
                                //printf("%s",str);
                        }//else if(is_include(i,str)){

                        // }
                }
        }
}
void free_macro(){
        for (int i = 0; i < numero_de_macros; i++) {
    if (vetor_macro[i]->simples == 0 && vetor_macro[i]->parametros) {
        for (int j = 0; j < vetor_macro[i]->qtd_param; j++) {
            free(vetor_macro[i]->parametros[j]);
        }
        free(vetor_macro[i]->parametros);
    }
    free(vetor_macro[i]);
}
free(vetor_macro);
}
int
main(int argc, char *argv[])
{
        if (argc < 2)
                die("usage: ./main <input.c> [<output>]");

        FILE *fin = fopen(argv[1], "r");
        FILE *fout = stdout;
        char str[BUFFER_SIZE] = {'\0'};

        if (argc >= 3)
                fout = fopen(argv[2], "w");
        if (!fin)
                die("Erro ao abrir o arquivo de entrada.");
        if (!fout)
                die("Erro ao criar o arquivo de saída.");
        read_file(fin, str);
        puts("ok main 1");
        
        remove_comments(str);
        puts("ok main 3");
        
        find_macros(str);
        puts("ok main 4");
        
        substituir_macros(str);
        puts("ok main 5");

        remove_space(str);
        puts("ok main 6");
        
        print_line(fout, str);
        puts("ok main 7");
        
        for (int i = 0; i < numero_de_macros; i++)
        {
                
                printf("id[%d]: %s\n",i,vetor_macro[i]->id);
                if(vetor_macro[i]->qtd_param>0){
                        
                        for(int j=0;j<vetor_macro[i]->qtd_param;j++){
                               
                                //remove_space(vetor_macro[i]->parametros[j]);
                                printf("parametro[%d]: %s\n",j,vetor_macro[i]->parametros[j]);
                }
                }
                remove_space(vetor_macro[i]->value);
                printf("valor[%d]: %s\n",i,vetor_macro[i]->value);
        }
        
        fclose(fin);
        puts("ok main 8");
        // nao tem problema de fechar stdout aqui
        fclose(fout);
        free_macro();
        puts("tudo rodando");
        
        return 0;
}
