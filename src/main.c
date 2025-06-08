#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "tokens.h"
#include "hashmap.h"
#include "vetor.h"

void remove_space(Vetor *str);

#ifdef DEBUG
#define PRINT_DEBUG(x) printf("%s\n", x)
#else
#define PRINT_DEBUG(x) (void) (x)
#endif

// futuramente o buffer tem que ficar na heap
#define BUFFER_SIZE 10240
#define ARRLEN(arr) ((int) (sizeof(arr) / sizeof(arr[0])))
// pensar em um nome
#define is_token_char(c) (isalnum(c) || (c) == '_')
// #define is_quote(str, i, c) ((i) && (c) == '"' && (str)[(i) - 1] != '\'' && (str)[(i) - 1] != '\\')
#define line_splicing(c1, c2) (c1) == '\\' && (c2) == '\n'

/* global variables */
bool is_string_ = false;
int numero_de_macros=0;
int tamanho_vetor_de_macros=0;
Macro *vetor_macro=NULL;

void inserir_macro(int numero_de_macros,int tamanho_vetor_de_macros,Macro* vet,Macro m);







void
die(const char *str)
{
        fprintf(stderr, "%s\n", str);
        exit(EXIT_FAILURE);
}

/* pensar em implementar is_quote e is_string
 * como macro novamente...
 */
bool
is_quote(Vetor *str, int i, char c)
{
        char ant = elemento(str, i);
        return i && c == '"' && ant != '\'' && ant != '\\';
}

bool
is_string(Vetor *str, int i)
{
        char c = elemento(str, i);
        if (is_quote(str, i, c))
                is_string_ = !is_string_;
        return is_string_;
}

void
print_line(FILE *f, Vetor *str)
{
        int len = tamanho_vetor(str);
        char c;
        for (int i = 0; i < len; i++) {
                c = elemento(str, i);
                putc(c, f);
        }
}

void
ignore_until_newline(Vetor *str, int *i)
{
        char c;
        while ((c = elemento(str, *i)) != '\n') {
                adiciona_elemento(str, '\0', *i);
                (*i)++;
        }
}

void
ignore_until_end_comment(Vetor *str, int *i)
{
        char ant = '\0';
        char c;
        while (true) {
                c = elemento(str, *i);
                if (c == '/' && ant == '*') {
                        adiciona_elemento(str, '\0', *i);
                        break;
                }
                ant = elemento(str, *i);
                adiciona_elemento(str, '\0', *i);
                (*i)++;
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
remove_comments(Vetor *str)
{
        int len = tamanho_vetor(str);
        for (int i = 1; i < len; i++) {
                if (is_string(str, i))
                        continue;
                char c = elemento(str, i);
                char ant = elemento(str, i - 1);
                if (ant == '/') {
                        if (c == '/') {
                                adiciona_elemento(str, ' ', i - 1);
                                ignore_until_newline(str, &i);
                        } else if (c == '*') {
                                adiciona_elemento(str, ' ', i - 1);
                                ignore_until_end_comment(str, &i);
                        }
                } 
        }
        organize_buffer(str);
}

void
read_file(FILE *fp, Vetor *str)
{
        char c;
        int i = 0;
        while ((c = getc(fp)) != EOF) {
                char ant = elemento(str, i - 1);
                if (i && line_splicing(ant, c)) {
                        i--;
                } else {
                        adiciona_elemento(str, c, i);
                        i++;
                }
        }
        adiciona_elemento(str, '\0', i);
}

void
remove_trailling_space(Vetor *str)
{
        int len = tamanho_vetor(str);
        char c, ant;
        for (int i = 1; i < len; i++) {
                c = elemento(str, i);
                ant = elemento(str, i - 1);
                if (is_string(str, i))
                        continue;
                if (isblank(c) && isblank(ant)) {
                        adiciona_elemento(str, '\0', i - 1);
                }
        }
        organize_buffer(str);
}

// quando ler um #, passa a posição do # no i, e a string
bool is_macro(int i,const char str[]){
        char aux[7]={'\0'};
        int k;
        if(i == 0){
                PRINT_DEBUG("ok is macro 1");
                for(k=0;k<6;k++){
                        PRINT_DEBUG("ok is macro 2");
                        aux[k]=str[(i+1)+k];
                }
                printf("%s",aux);
                if(strcmp(aux,"define")==0){
                        PRINT_DEBUG("ok is macro 3");
                        return true;
                }
        }
        if(i > 0 && str[i-1] == '\n'){
                PRINT_DEBUG("ok is macro 4");
                for(k=0;k<6;k++){
                        PRINT_DEBUG("ok is macro 5");
                        aux[k]=str[(i+1)+k];
                }
                if(strcmp(aux,"define") == 0){
                        PRINT_DEBUG("ok is macro 6");
                        return true;
                }
        }
        PRINT_DEBUG("ok is macro 7");
        return false;
}
// i é a posição do primeiro char do macro
bool is_simple_macro(int i,const char str[]){
        int k=0;
        while(str[i+k] != ' ' && str[i+k] != '('){
                k++;
        }
        if(str[i+k] == '('){
                return false;
        }
        return true;
}
// assume que i é a posição do primeiro char do macro
void save_macro(int i, const char str[]){
        int k=0;
        int j=0;
        Macro temp;
        temp.disponibilidade = OCUPADO;
        PRINT_DEBUG("save macro 1");
        if(is_simple_macro(i,str)){
                 PRINT_DEBUG("save macro 2");
                temp.simples=1;
                temp.qtd_param=0;
                while(str[i+k]!=' '){
                         PRINT_DEBUG("save macro loop 3");
                        temp.id[j]=str[i+k];
                        k++;j++;
                }
                k++; // pula o ' '
                temp.id[j]='\0';
                j=0;
                while (str[i+k] != ' ' && str[i+k] != '\n'){
                         PRINT_DEBUG("save macro 4");
                        temp.value[j]=str[i+k];
                        k++;j++;
                }
                 PRINT_DEBUG("save macro 5");
                temp.value[j]='\0';
                printf("%s\n",temp.value);        //teste
                PRINT_DEBUG("save macro 6");
                inserir_macro(numero_de_macros,tamanho_vetor_de_macros,vetor_macro,temp);
                 PRINT_DEBUG("save macro 7");
        }else{
                temp.simples=0;
                temp.qtd_param=1;
                while(str[i+k]!='(' ){
                        temp.id[j]=str[i+k];
                        k++;j++;
                }
                temp.id[j]='\0';
                k++;    // pula o "("
                int aux =k;
                while(str[i+k]!=')' && str[i + k] != '\0' ){
                        if(str[i+k] == ','){
                                temp.qtd_param++;
                                k++;
                        }
                }
                k=aux;
                temp.parametros = malloc(sizeof(char *) * temp.qtd_param);
                char parametrotemp[BUFFER_SIZE]={'\0'};
                j=0;            //pos em parametros
                int n=0;        //parametro n
                while(str[i+k]!=')' ){
                        if(str[i+k] != ','){
                                parametrotemp[j]=str[i+k];
                                k++;j++;
                        }else{
                                parametrotemp[j]='\0';
                                temp.parametros[n]= strdup(parametrotemp);
                                memset(parametrotemp, 0, sizeof(parametrotemp));
                        }
                }
                parametrotemp[j]='\0';
                temp.parametros[n]= strdup(parametrotemp);
                k++;    //pula ')'
                j=0;    //pos do corpo
                while (str[i + k] == ' ') k++;  //pula espaços
                while (str[i + k] != '\0' && str[i + k] != '\n' && str[i + k] != ' ') {
                        temp.value[j++] = str[i + k++];
                }
                temp.value[j] = '\0';

                inserir_macro(numero_de_macros,tamanho_vetor_de_macros,vetor_macro,temp);
        }
}
void inserir_macro(int numero_de_macros,int tamanho_vetor_de_macros,Macro* vet,Macro m){
        numero_de_macros++;
        PRINT_DEBUG("inserir macro 1");
        if(numero_de_macros == 1){
                PRINT_DEBUG("inserir macro 2");
                tamanho_vetor_de_macros = 1;
                vet = (Macro*)malloc(sizeof(Macro)*tamanho_vetor_de_macros);
                if (vet == NULL) {
                        perror("Erro ao alocar memória para vetor de structs");
                        return ;
                }
                vet[numero_de_macros-1] = m;
        }else if(numero_de_macros >= tamanho_vetor_de_macros){
                PRINT_DEBUG("inserir macro 3");
                tamanho_vetor_de_macros = tamanho_vetor_de_macros*2;
                vet = realloc(vet,sizeof(Macro)*tamanho_vetor_de_macros);
                if (vet == NULL) {
                        perror("Erro ao alocar memória para vetor de structs 2");
                        return ;
                }
                vet[numero_de_macros -1]= m;
        }else{
                PRINT_DEBUG("inserir macro 4");
                vet[numero_de_macros - 1] = m;
        }
        PRINT_DEBUG("inserir macro 5");
}
void
remove_space(Vetor *str)
{
        remove_trailling_space(str);
        int col = 0;
        int len = tamanho_vetor(str);
        char ant, c, prox;
        if (isspace(elemento(str, 0)))
                adiciona_elemento(str, '\0', 0);
        for (int i = 1; i < len - 1; i++) {
                if (is_string(str, i))
                        continue;

                ant = elemento(str, i - 1);
                c = elemento(str, i);
                prox = elemento(str, i + 1);
                bool is_space_now = isblank(c);
                // ignora espaco no comeco da linha
                if (col == 0 && is_space_now) {
                        adiciona_elemento(str, '\0', i);
                        continue;
                } else if (is_space_now) {
                        if (is_new_token(ant, prox))
                                continue;
                        else
                                adiciona_elemento(str, '\0', i);
                } 
                if (c == '\n') {
                        col = 0;
                } else {
                        col++;
                }
        }
        organize_buffer(str);
}

void
get_name(Vetor *str, int ini, int len)
{
        char c = elemento(str, 0);
        if (isdigit(c))
                return;
        Vetor *word = novo_vetor();
        vetor_memcpy(word, str, ini, len);
        print_line(stdout, word);
}

void
substituir_macros(Vetor *str)
{
        int word_len = 0, len = tamanho_vetor(str);
        char c;
        for (int i = 0; i < len; i++) {
                c = elemento(str, i);
                if (is_string(str, i))
                        continue;
                if (is_token_char(c)) {
                        word_len++;
                } else if (word_len > 0) {
                        get_name(str, i, word_len);
                        word_len = 0;
                }
        }
}
//acha macros, escreve eles num hash, e apaga da string substituindo os caracteres por \0
void find_macros(char str[]){
        PRINT_DEBUG("ok 1");
        int i=0;
        int length = (int)strlen(str);  
        //char aux[BUFFER_SIZE]={'\0'};
        for(i=0;i<length;i++){
                printf("ok loop %d\n",i);
                if(str[i] == '#'){
                        PRINT_DEBUG("ok 3");
                        if(is_macro(i,str)){
                                PRINT_DEBUG("ok 4");
                                save_macro(i,str);
                                PRINT_DEBUG("ok 5");
                                printf("%s",str);
                                // while(str[i]!='\n' || i>= length){
                                //         PRINT_DEBUG("ok ao");
                                //         str[i]='\0';
                                //         PRINT_DEBUG("ok 6");
                                //         printf("%c",str[i]);
                                //         PRINT_DEBUG("ok 7");
                                //         i++;                    //tira os defines do arquivo
                                // }
                        }
                }
        }
}

int
main(int argc, char *argv[])
{
        if (argc < 2)
                die("usage: ./main <input.c> [<output>]");

        FILE *fin = fopen(argv[1], "r");
        FILE *fout = stdout;
        Vetor *str = novo_vetor();

        if (argc >= 3)
                fout = fopen(argv[2], "w");
        if (!fin)
                die("Erro ao abrir o arquivo de entrada.");
        if (!fout)
                die("Erro ao criar o arquivo de saída.");
        PRINT_DEBUG("ok main 1");
        read_file(fin, str);
        PRINT_DEBUG("ok main 3");
        remove_comments(str);
        // remove_space(str);
        // substituir_macros(str);
        print_line(fout, str);
        return 0;
        PRINT_DEBUG("ok main 4");
        PRINT_DEBUG("ok main 5");
        // find_macros(str);
        PRINT_DEBUG("ok main 6");
        PRINT_DEBUG("ok main 7");
        fclose(fin);
        PRINT_DEBUG("ok main 8");
        // nao tem problema de fechar stdout aqui
        fclose(fout);
        PRINT_DEBUG("tudo rodando");
        return 0;
}
