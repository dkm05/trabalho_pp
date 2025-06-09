#pragma once
#include <stdlib.h>
#include <stdio.h>

#define MACRO_SIZE 4096
#define BUFFER_SIZE 4096

#define ARRLEN(arr) ((int) (sizeof(arr) / sizeof(arr[0])))
#define is_token_char(c) (isalnum(c) || (c) == '_')
#define is_quote(str, i, c) ((i) && (c) == '"' && (str)[(i) - 1] != '\'' && (str)[(i) - 1] != '\\')
#define line_splicing(c1, c2) (c1) == '\\' && (c2) == '\n'

typedef struct Macro{
        char id[MACRO_SIZE];
        char value[MACRO_SIZE];
        int disponibilidade;
        int simples;
        char **parametros;
        int qtd_param;
}Macro;



enum {
    OCUPADO = 0,
    LIVRE = 1,
};

void read_file(FILE *fp, char str[]);
void get_system_include(char str[], FILE *fout);
void get_include(const char str[], FILE *fout);
void get_user_include(char str[], FILE *fout);
void process_directives(char str[], FILE *f);
void process_file(FILE *fin, FILE *fout);
void print_line(FILE *f, char str[]);

void inserir_macro(Macro* m);
bool is_macro(int i,const char str[],int *ret);
void save_macro(int i, const char str[]);
bool is_simple_macro(int i,const char str[]);
void find_macros_leitura(char str[]);
void substituir_macros_final(char str[]);
void substituir_macros(char str[]);
void free_macro();

void get_name(char dest[], char str[], size_t len);
bool is_new_token(char c1, char c2);
bool is_string(char str[], size_t i);
void ignore_until_newline(char str[], size_t *i);
void organize_buffer(char *str);
void ignore_until_end_comment(char str[], size_t *i);
void remove_comments(char str[]);
void remove_trailling_space(char str[]);
void remove_space(char str[]);
void die(const char *str);