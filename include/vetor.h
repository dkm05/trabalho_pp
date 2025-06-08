#ifndef VETOR_H
#define VETOR_H

typedef struct Vetor Vetor;

Vetor* novo_vetor(void);
void finaliza_vetor(Vetor* V);
int tamanho_vetor(const Vetor* V);
void adiciona_elemento(Vetor* V, char x, int i);
char elemento(const Vetor* V, int i);
void remove_elemento(Vetor* V, int i);
void vetor_memcpy(Vetor *dest, const Vetor *src, int ini, int fim);

#endif
