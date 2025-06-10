#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "tokens.h"
#include "funcoes.h"

/* global variables */
bool is_string_ = false;
int numero_de_macros=0;
int tamanho_vetor_de_macros=0;
Macro **vetor_macro=NULL;

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
        size_t i;
        for (i = 0; str[i] != '\n'; i++) {
                if (str[i] != '\0')
                        putc(str[i], f);
        }
        if (i > 0 && i < BUFFER_SIZE && is_new_token(str[i - 1], str[i + 1]))
                putc(' ', f);
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
        while(str[i+k] != ' ' && str[i+k] != '(' && (i+k)<length && str[i+k] != '\t'){
                k++;
        }
        puts("is simple 2");
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
        while((str[i] == ' ' || str[i] == '\t' )&& i<(int)strlen(str)){
                printf("%c\n",str[i]);
                i++;
        }
        if(is_simple_macro(i,str)){
                i++;
                temp->simples=1;
                temp->qtd_param=0;
                while(str[i+k]!=' ' && str[i+k]!='\t' ){
                        temp->id[j]=str[i+k];
                        k++;j++;
                }
                k++; // pula o ' '
                printf("id[%d]: %s\n",numero_de_macros,temp->id);
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
                remove_space(temp->value);
                remove_space(temp->id);
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
                remove_space(temp->id);
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
                                temp->parametros[n]= calloc(4096, sizeof(char));
                                strcpy(temp->parametros[n], parametrotemp);
                                printf("parametrotemp salvo no vetor: %s\n",temp->parametros[n]);
                                n++;
                                memset(parametrotemp, 0, sizeof(parametrotemp));
                                k++;    //pula a virgula
                                j=0;    //volta pro inicio
                        }
                }
                puts("macro com parametros 11");
                while (j<BUFFER_SIZE){
                        parametrotemp[j++]='\0';
                }
                puts("macro com parametros 12");
                temp->parametros[n]= calloc(4096, sizeof(char));
                strcpy(temp->parametros[n], parametrotemp);
                k++;    //pula ')'
                j=0;    //pos do corpo
                puts("macro com parametros 13");
                while (str[i + k] == ' ') k++;
                k--;  //pula espaços
                puts("macro com parametros 14");
                while (str[i + k] != '\0' && str[i + k] != '\n') {
                        temp->value[j++] = str[i + k++];
                }
                puts("macro com parametros 15");
                while(j<BUFFER_SIZE){
                       temp->value[j++] = '\0';
                }
                printf("valor lido antes: %s\n",temp->value); 
                remove_space(temp->value);
                printf("valor lido: %s\n",temp->value);        

                puts("macro com parametros 18");
                for(int h;h<temp->qtd_param;h++){
                        remove_space(temp->parametros[h]);
                }
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
get_name(char dest[], char str[], size_t len)
{
        if (isdigit(str[0])) {
                dest[0] = '\0';
                return;
        }
        // olhar se o tamanho é maior...
        memcpy(dest, str, len);
        dest[len] = '\0';
}

//acha macros,salva no vetor_macro, e apaga da string substituindo os caracteres por \0
void find_macros_leitura(char str[]){
        puts("ok 1");
        int i=0;
        int ret;
        int length = (int)strlen(str);  
        for(i=0;i<length;i++){
                if(str[i] == '#'){
                        if(is_macro(i,str,&ret)){
                                puts("ok 4");
                                save_macro(i+7+ret,str);//pos do # + 7 digitos(define) + ret(consequentes espaços pulados em is_macro)
                                puts("ok 5");
                                int aux =i;
                                while( str[aux]!='\n' && aux < length){
                                        str[aux]='\0';
                                        aux++;                    //tira os defines do arquivo
                                }
                        }
                }
        }
        organize_buffer(str);
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

void substituir_macros_final(char str[]){
        int i;// para procurar o vetor_macro[i] na str
        size_t k=0; //indice de str
        size_t pos =0; //indice de aux
        char aux[BUFFER_SIZE]={'\0'};
        size_t length = strlen(str);
        for(i=0;i<numero_de_macros;i++){
                printf("procura %d\n",i);
                while (k<length){
                        if(!is_token_char(str[k])){
                                //printf("achou um \"%c\"\n",str[k]);
                                memset(aux,0,sizeof(aux));
                                pos=0;
                                k++;    //pular caracter nao desejado
                        }
                        aux[pos]=str[k];
                        aux[pos+1]='\0';
                        if(strcmp(vetor_macro[i]->id,aux)){

                        }
                        k++;pos++;
                }
                memset(aux,0,sizeof(aux));
                k=0;
                pos=0;
        }
}

void get_system_include(char str[], FILE *fout)
{
        /* tamanho alto para garantir que não vai 
        * faltar espaço... isso seria resolvido se tivessemos usado 
        * vetor dinamico. tem um exemplo de implementação na branch
        * "vetor dinamico" (apenas remove espacos e comentarios)
        */
        char *buffer = calloc(1000000, sizeof(char));
        if (!buffer) {
                printf("NAO FOI POSSIVEL ALOCAR\n");
        }
        FILE *fin;
        char file[256] = {'\0'};
        char *db[4] = 
        {
                "/usr/include/",
                "/usr/local/include/",
                "/usr/lib/gcc/x86_64-pc-linux-gnu/15.1.1/include/",
                "/usr/lib/gcc/x86_64-pc-linux-gnu/15.1.1/include-fixed/",
        };

        for (int i = 0; i < 4; i++) {
                strcpy(file, db[i]);
                strcat(file, str);
                if ((fin = fopen(file, "r")))
                        break;
        }
        if (fin) {
                printf("FILE: %s\n", file);
                read_file(fin, buffer);
                remove_comments(buffer);
                remove_space(buffer);
                process_directives(buffer, fout);
                size_t len = strlen(buffer);
                for (size_t i = 0; i < len; i++) {
                        if (buffer[i] != '\0')
                                putc(buffer[i], fout);
                }
                putc('\n', fout);
        } else {
                printf("NAO ABRIU: %s\n", file);
        }
        free(buffer);
}

void get_user_include(char str[], FILE *fout)
{
        FILE *fin = fopen(str, "r");
        
        if (!fin) {
                get_system_include(str, fout);
                return;
        }
        
        process_file(fin, fout);
}

void get_include(const char str[], FILE *fout)
{
        char file[64] = {'\0'};
        sscanf(str, " %[^\n]", file);
        size_t len = strlen(file);
        if (file[0] == '<') {
                file[0] = file[len - 1] = '\0';
                get_system_include(file + 1, fout);
        } else if (file[0] == '\"') {
                file[0] = file[len - 1] = '\0';
                get_user_include(file + 1, fout);
        } else {
                fprintf(stderr, "ERRO: arquivo %s não encontrado.\n", file);
                exit(EXIT_FAILURE);
        }
}

void process_directives(char str[], FILE *f)
{
        size_t len = strlen(str), size = 1000;
        char *linha = calloc(size, sizeof(char));
        if (!linha) {
                printf("NAO FOI POSSIVEL ALOCAR\n");
                exit(EXIT_FAILURE);
        }
        char nome[128] = {'\0'};
        char c;
        size_t i = 0;
        size_t count = 0;
        while (count < len) {
                while ((c = str[count++]) != '\n') {
                        if (i >= size) {
                                size *= 2;
                                linha = realloc(linha, size);
                        }
                        if (c != '\0')
                                linha[i++] = c;
                }
                linha[i] = '\0';
                if (linha[0] == '#') {
                        sscanf(linha, "# %[A-Za-z_0-9]", nome);
                        count -= strlen(linha) + 1;
                        ignore_until_newline(str, &count);
                        if(!strcmp(nome, "define")){
                                ; // faz algo
                        } else if (!strcmp(nome, "include")) {
                                get_include(linha + 8, f);
                        } else if (!strcmp(nome, "ifdef")) {
                                ;
                                // exemplo...
                        }
                } else {
                        print_line(f, str + count - i - 1);
                }
                linha[0] = '\0';
                i = 0;
        }
        organize_buffer(str);
        free(linha);
}

void
process_file(FILE *fin, FILE *fout)
{
        char *buffer = calloc(1000000, sizeof(char));
        if (!buffer) {
                printf("NAO FOI POSSIVEL ALOCAR\n");
        }
        read_file(fin, buffer);
        remove_comments(buffer);
        remove_space(buffer);
        find_macros_leitura(buffer);
        substituir_macros(buffer);
        printf("%s",buffer);
        process_directives(buffer, fout);
        free(buffer);
}

// a: string a ser substituida, b: string que vai entrar no lugar
int substituir_isolado_simples( char* str, const char* a, const char* b) {
        if (!str || !a || !b)
        return 0;
        size_t len_str = strlen(str);
        int len_a = (int) strlen(a);
        int len_b = (int) strlen(b);
        
        if (!len_a) 
        return 0;
        // Primeiro, conta quantas vezes 'a' aparece em 'str'
        int count = 0;
        char* temp = str;
        while ((temp = strstr(temp, a)) != NULL) {
                // Verifica se está isolado
                char before;
                if(temp == str){
                        before = '\0';
                }else{
                        before = *(temp - 1);
                }
                char after = temp[len_a];
                if (!is_token_char(before) && !is_token_char(after)) {
                        count++;
                }
                temp += len_a;
        }
        
        size_t max_len = len_str + count * (len_b - len_a) + 1;
        char* result = malloc(max_len * sizeof(char));
        if (!result) 
        return 0;
        
        const char* src = str;
        char* dst = result;
        
        while (*src) {
                if (strncmp(src, a, len_a) == 0) {
                        char before = '\0';
                        char after  = src[len_a];
                        if (src != str)
                        before = *(src - 1);
                        
                        if (!is_token_char(before) && !is_token_char(after)) {
                                strcpy(dst, b);
                                dst += len_b;
                                src += len_a;
                                continue;
                        }
                }
                *dst++ = *src++;
        }
        
        *dst = '\0';
        strcpy(str,result);
        return 1;
}

void substituir_macro_parametrica(char *str, Macro *macro) {
    char *pos = str;
    char buffer[1000000] = {0};
    char *out = buffer;

    while (*pos) {
        if (strncmp(pos, macro->id, strlen(macro->id)) == 0 && pos[strlen(macro->id)] == '(') {
            pos += strlen(macro->id) + 1;

            // Coletar argumentos
            char *args[macro->qtd_param];
            for (int i = 0; i < macro->qtd_param; i++) {
                args[i] = calloc(256, sizeof(char));
                int j = 0;
                while (*pos && *pos != ',' && *pos != ')') {
                    args[i][j++] = *pos++;
                }
                args[i][j] = '\0';
                if (*pos == ',') pos++; // pula a vírgula
            }

            if (*pos == ')') pos++; // pula o ')'

            // Substituir no value
            char temp[4096];
            strcpy(temp, macro->value);
            for (int i = 0; i < macro->qtd_param; i++) {
                printf("%s\n", args[i]);
                substituir_isolado_simples(temp, macro->parametros[i], args[i]);
                free(args[i]);
            }

            // Copiar resultado expandido
            strcpy(out, temp);
            out += strlen(temp);
        } else {
            *out++ = *pos++;
        }
    }

    *out = '\0';
    strcpy(str, buffer);
}

void substituir_macros(char str[]){
        for(int i= 0;i<numero_de_macros;i++){
                if(vetor_macro[i]->simples == 1){
                        substituir_isolado_simples(str,vetor_macro[i]->id,vetor_macro[i]->value);
                }else{
                        substituir_macro_parametrica(str, vetor_macro[i]);
                }
                
        }
}

int
main(int argc, char *argv[])
{
        if (argc < 2)
        die("usage: ./main <input.c> [<output>]");
        
        printf("%s\n", argv[1]);
        FILE *fin = fopen(argv[1], "r");
        FILE *fout = stdout;

        if (argc >= 3)
                fout = fopen(argv[2], "w");
        if (!fin)
                die("Erro ao abrir o arquivo de entrada.");
        if (!fout)
                die("Erro ao criar o arquivo de saída.");

        process_file(fin, fout);

        fclose(fin);
        // nao tem problema de fechar stdout aqui
        fclose(fout);
        free_macro();

        return 0;
}
