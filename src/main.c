#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "tokens.h"
#include "hashmap.h"


// futuramente o buffer pode ficar na heap
#define BUFFER_SIZE 10240
// pensar em um nome
#define ARRLEN(arr) ((int) (sizeof(arr) / sizeof(arr[0])))
#define is_token_char(c) (isalnum(c) || (c) == '_')


/* global variables */
bool is_string = false;


void
die(const char *str)
{
        fprintf(stderr, "%s\n", str);
        exit(EXIT_FAILURE);
}

// pode ser macro
bool
is_quote(char str[], int i, char c)
{
        return i && c == '"' && str[i - 1] != '\'' && str[i - 1] != '\\';
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

// da p implementar as proximas 3 funções como macros
bool
line_splicing(char str[], int i)
{
        return str[i - 1] == '\\';
}

void
ignore_until_newline(char str[], int *i)
{
        while (str[*i] != '\n')
                str[(*i)++] = '\0';
}

void
ignore_until_end_comment(char str[], int *i)
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

void
process_directive(char str[], int *i)
{
        /* pode pegar o diretorio ate achar um newline
         * (de preferencia fazendo line_splicing no caminho)
         * e colocar em uma string (ou ir processando enquanto le)
         * o ideal é encher de '\0' pra remover a linha do arquivo
         *
         * OU
         *
         * passa a string já pronta pra essa função
         */
        int j;
        for (j = *i; str[j] != '\n'; j++) {
                str[j] = '\0';
        }
        str[j] = '\0';
        *i = j;
}

void
process_file(char str[])
{
        int col = 0;
        for (int i = 0; str[i] != '\0'; i++) {
                if (col == 0) {
                        // ignora espaco no comeco da linha
                        if (isspace(str[i])) {
                                str[i] = '\0';
                                continue;
                        } else if (str[i] == '#') {
                                /* se passar a string do diretorio
                                 * pronta por essa função, talvez
                                 * não precise passar por referência
                                 */
                                process_directive(str, &i);
                                col = 0;
                                continue;
                        }
                }
                if (isspace(str[i]) && i) {
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

/* ideia: coloca todo o arquivo dentro de uma string
 * e remove os comentarios no processo
 * (bonus: nao é o ideal mas pode fazer o line_splicing aqui)
 */

void
remove_comments(char str[])
{
        for (int i = 0; str[i] != '\0'; i++) {
                if (is_quote(str, i, str[i]))
                        is_string = !is_string;
                if (i && str[i - 1] == '/' && !is_string) {
                        if (str[i] == '/') {
                                str[i - 1] = ' ';
                                ignore_until_newline(str, &i);
                                continue;
                        } else if (str[i] == '*') {
                                str[i - 1] = ' ';
                                ignore_until_end_comment(str, &i);
                                continue;
                        }
                } 
        }
}

void
read_file(FILE *fp, char str[])
{
        char c;
        int i = 0;
        while ((c = getc(fp)) != EOF) {
                if (line_splicing(str, i)) {
                        str[i - 1] = str[i] = 'A';
                        i--;
                        continue;
                }
                str[i++] = c;
        }
        str[i] = '\0';
}
// quando ler um #, passa a posição do # no i, e a string
bool is_macro(int i,const char str[]){
        char aux[7];
        int k;
        for(k=0;k<6;k++){
                aux[k]=str[(i+1)+k];
        }
        if(!strcmp(aux,"define")){
                return true;
        }
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
void save_macro(const char arq[], int i, const char str[]){
        int k=0;
        int j=0;
        Macro temp;
        temp.disponibilidade = OCUPADO;
        if(is_simple_macro(i,str)){
                temp.simples=1;
                temp.qtd_param=0;
                while(str[i+k]!=' '){
                        temp.id[j]=str[i+k];
                        k++;j++;
                }
                k++; // pula o ' '
                temp.id[j]='\0';
                j=0;
                while (str[i+k] != ' ' && str[i+k] != '\n'){
                        temp.value[j]=str[i+k];
                        k++;j++;
                }
                temp.value[j]='\0';
                inserir(arq,temp);
                
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
                                k++;n++;j=0;    //vai ler o novo parametro;
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

                inserir(arq, temp);
        }
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
        remove_comments(str);
        process_file(str);
        print_line(fout, str);
        fclose(fin);
        // nao tem problema de fechar stdout aqui
        fclose(fout);
}
