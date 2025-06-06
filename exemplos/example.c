/* gcc é o um dos compiladores mais usados para C,
 * por isso usamos o comportamento do pré processador 
 * dele como referência
 *
 * comando para conseguir apenas o output do
 * pré processador do gcc:
 *
 * gcc -E <input.c>
 */

/* esse exemplo tem que ser compilado no gcc,
 * para confirmar que o codigo ta valido
 */
#define EX for(int i = 0; i < 10; i++)\
i += 2;

#include <stdio.h>




/* esse exemplo ta aqui para testar a continuação da linha com '\' */
/* fonte: https://git.suckless.org/st/file/arg.h.html */
#define ARGBEGIN	for (argv0 = *argv, argv++, argc--;\
			                argv[0] && argv[0][0] == '-'\
					&& argv[0][1];\
					argc--, argv++) {\
				char argc_;\
				char **argv_;\
				int brk_;\
				if (argv[0][1] == '-' && argv[0][2] == '\0') {\
					argv++;\
					argc--;\
					break;\
				}\
				for (brk_ = 0, argv[0]++, argv_ = argv;\
						argv[0][0] && !brk_;\
						argv[0]++) {\
					if (argv_ != argv)\
						break;\
					argc_ = argv[0][0];\
					switch (argc_)
#define ARGEND			}\
                        }
int main() {
        // esse é o primeiro comentário //
        printf ( " \n " ) ;
        int     a;
        //isso é outro comentário /**///
        int cc, dd = 0; 
        dd &= ~(1 << 24);

        int arg = 0;
        // essa string fica com muitos espaços no gcc também
        printf("um    exemplo    de quebra de linha\
                fora das macros\
                        %d\n", 10);

        /* isso é um
         * comentário multi-linha
         */
        /*****************************************
         * isso é outro comentário multi-linha   *
        *****************************************/

        char b = '\0';

        /* ... */ printf("1 + 1 = %d\n", 1/**/+1); // ...
        /**/ printf("/*..*/%d\n", arg ? /* isso */ 1 : /* ou aquilo */ 2)/* .. */;
        return 0;
}
