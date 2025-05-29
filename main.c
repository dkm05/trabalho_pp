#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tokens.h"

#define MAX_LINE_LEN 256
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
        for (int i = 0; i < MAX_LINE_LEN; i++) {
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
                        organize_buffer(str);
                        break;
                } else {
                        str[i] = '\0';
                }
        }
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
print_line(char str[], FILE *f)
{
        for (int i = 0; str[i] != '\0'; i++)
                putc(str[i], f);
        putc('\n', f);
}

int
main(int argc, char *argv[])
{
        if (argc != 2)
                die("usage: ./main <file.c>");

        FILE *fin = fopen(argv[1], "r");
        char str[256] = {'\0'}, c;
        while (!feof(fin)) {
                fscanf(fin, " %255[^\n]%c", str, &c);
                remove_comments(str);
                print_line(str, stdout);
                memset(str, '\0', ARRLEN(str));
        }
        fclose(fin);
        return 0;
}
