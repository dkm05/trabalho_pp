#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tokens.h"

#define BUFFER_SIZE 1024
#define ARRLEN(arr) (sizeof (arr) / sizeof (arr)[0])

/* global variables */
bool is_comment = false;

void
die(const char *str)
{
        fprintf(stderr, "%s\n", str);
        exit(EXIT_FAILURE);
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
        for (int i = ini; str[i] != '\0'; i++)
                str[i] = '\0';
}

void
remove_multi_line_comment(char str[], int ini)
{
        for (int i = ini; str[i] != '\0'; i++) {
                if (str[i] == '*' && str[i + 1] == '/') {
                        str[i] = '\0';
                        str[i + 1] = ' ';
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
        bool is_string = false;
        if (is_comment)
                remove_multi_line_comment(str, 0);

        for (int i = 0; str[i] != '\0'; i++) {
                if (i && str[i] == '"' && str[i - 1] != '\'')
                        is_string = !is_string;

                if (!is_string && str[i] == '/') {
                        if (str[i + 1] == '/')
                                remove_single_line_comment(str, i);
                        else if (str[i + 1] == '*') {
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
        putc('\n', f);
}

/* obs: dentro das definições das macros
 * tem q ter o menor número de espaços possivel
 */
void
read_line(FILE *fp, char str[])
{
        char c;
        int i;
        while ((c = getc(fp)) == ' ')
                ;
        for (i = 0; i < BUFFER_SIZE - 1; i++) {
                if (c == EOF)
                        break;
                str[i] = c;
                if (str[i] == '\n') {
                        if (i == 0 || str[i - 1] != '\\')
                                break;
                        else
                                i -= 2;
                }
                c = getc(fp);
        }

        str[i] = '\0';
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
                read_line(fin, str);
                remove_comments(str);
                print_line(fout, str);
                memset(str, '\0', strlen(str));
        }
        fclose(fin);
        // nao tem problema de fechar stdout aqui
        fclose(fout);
        return(0);
}
