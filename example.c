#include <stdio.h>

int main() {
        // esse é o primeiro comentário //
        printf("\n");
        //isso é outro comentário /**///
        printf("lolol");
        /* isso é um
         * comentário multi-linha
         */
        int/*
              aa
        */arg = 0;
        /*****************************************
         * isso é outro comentário multi-linha   *
        *****************************************/

        /* ... */ printf("%d\n", arg); // ...
        /**/ printf("/*..*/%d\n", arg ? /* isso */ 1 : /* ou aquilo */ 2)/* .. */; /**/
        return 0;
}
