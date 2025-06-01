#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "tokens.h"

#define BUFFER_SIZE 1024
#define ARRLEN(arr) (sizeof (arr) / sizeof (arr)[0])
#define BETWEEN(a, b, c) (((a) >= (b)) && ((a) <= (c)))

/* global variables */
bool is_comment = false;
bool is_string = false;

void
die(const char *str)
{
        fprintf(stderr, "%s\n", str);
        exit(EXIT_FAILURE);
}

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
remove_single_line_comment(char str[], int ini)
{
        str[ini] = ' ';
        for (int i = ini + 1; str[i] != '\0'; i++)
                str[i] = '\0';
}

void
remove_multi_line_comment(char str[], int ini)
{
        for (int i = ini; str[i] != '\0'; i++) {
                if (str[i] == '*' && str[i + 1] == '/') {
                        str[i] = ' ';
                        str[i + 1] = '\0';
                        is_comment = false;
                        break;
                } else {
                        str[i] = '\0';
                }
        }
        organize_buffer(str);
}

void
remove_comments(char str[])
{
        if (is_comment)
                remove_multi_line_comment(str, 0);

        for (int i = 0; str[i] != '\0'; i++) {
                if (is_quote(str, i, str[i]))
                        is_string = !is_string;

                if (!is_string && str[i] == '/') {
                        if (str[i + 1] == '/')
                                remove_single_line_comment(str, i);
                        else if (str[i + 1] == '*') {
                                str[i] = ' ';
                                is_comment = true;
                                remove_multi_line_comment(str, i);
                        }
                }
        }
}

void
print_line(FILE *f, char str[])
{
        for (int i = 0; str[i] != '\0'; i++) {
                putc(str[i], f);
        }
        /* ficar atento com isso... por enquanto ta aqui para
         * o output ficar visivel, provavelmente tem q remover
         * essa linha no final
         */
       //putc('\n', f);
}

bool
line_splicing(char str[], int i)
{
        return str[i - 1] == '\\';
}

void
ignore_until_newline(FILE *fp)
{
        char c;
        while ((c = getc(fp)) != '\n')
                ;
}

void
ignore_until_end_comment(FILE *fp)
{
        char c1;
        while ((c1 = getc(fp))) {
                if (c1 == '*') {
                        if ((c1 = getc(fp)) == '/')
                                break;
                }
        }
}

void
read_directive_line(FILE *fp, char str[], int i)
{
        char c;
        while ((c = getc(fp))) {
                if (!isspace(c))
                        break;
        }
        str[i++] = c;
        while (((c = getc(fp)) != '\n') && i < BUFFER_SIZE) {
                if (str[i - 1] == '/') {
                        if (c == '/') {
                                str[i - 1] = ' ';
                                ignore_until_newline(fp);
                                continue;
                        } else if (c == '*') {
                                str[i - 1] = ' ';
                                ignore_until_end_comment(fp);
                                continue;
                        }
                }
                str[i++] = c;
        }
        str[i] = c;
        if (line_splicing(str, i)) {
                str[i - 1] = str[i] = '\0';
                read_directive_line(fp, str, i - 1);
        }
}

void
read_line_of_code(FILE *fp, char str[], int i)
{
        char c;
        for (; i < BUFFER_SIZE - 1; i++) {
                c = getc(fp);
                if (isblank(c)) {
                        c = ' ';
                } else if (is_quote(str, i, c)) {
                        is_string = !is_string;
                } else if (i && str[i - 1] == '/' && !is_string) {
                        if (c == '/') {
                                str[i - 1] = ' ';
                                ignore_until_newline(fp);
                                continue;
                        } else if (c == '*') {
                                str[i - 1] = ' ';
                                ignore_until_end_comment(fp);
                                continue;
                        }
                }
                str[i] = c;
        }
        str[i] = c;
        if (line_splicing(str, i)) {
                str[i - 1] = str[i] = '\0';
                read_line_of_code(fp, str, i - 1);
        }
}

/* obs: dentro das definições das macros
 * tem q ter o menor número de espaços possivel
 */
/*
void
read_line(FILE *fp, char str[])
{
        char c;
        while ((c = getc(fp))) {
                // talvez isspace
                if (!isspace(c))
                        break;
        }
        if (c == EOF)
                return;
        str[0] = c;
        if (c == '#')
                read_directive_line(fp, str, 1);
        else
                read_line_of_code(fp, str);
}
*/

void
remove_whitespace(char str[])
{
        for (int i = 0; str[i] != '\0'; i++) {
                if (is_quote(str, i, str[i]))
                        is_string = !is_string;
                // ' '
                bool is_space_char = i && str[i - 1] == '\'' && str[i + 1] == '\'';
                if (!is_space_char && str[i] == ' ' && !is_string) {
                        //printf("-----%c-----\n", str[i]);
                        bool before = i && (isalnum(str[i - 1]) ||
                                        str[i - 1] == '_');
                        bool next = isalnum(str[i + 1]) || 
                                    str[i + 1] == '_';
                        if (!(before && next))
                                str[i] = '\0';
                }
        }
        organize_buffer(str);
}

int
main(int argc, char *argv[])
{
        if (argc < 2)
                die("usage: ./main <input.c> [<output>]");

        FILE *fin = fopen(argv[1], "r");
        FILE *fout = stdout;

        if (argc == 3)
                fout = fopen(argv[2], "w");
        if (!fin)
                die("Erro ao abrir o arquivo de entrada.");
        if (!fout)
                die("Erro ao criar o arquivo de saída.");

        char str[BUFFER_SIZE] = {'\0'};
        while (!feof(fin)) {
                read_line_of_code(fin, str, 0);
                remove_comments(str);
                remove_whitespace(str);
                print_line(fout, str);
                memset(str, '\0', strlen(str));
        }
        fclose(fin);
        // nao tem problema de fechar stdout aqui
        fclose(fout);
}
