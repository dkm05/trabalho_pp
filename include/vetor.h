#ifndef VETOR_H
#define VETOR_H

typedef struct Vetor Vetor;

Vetor* novo_vetor(int capacidade);
void finaliza_vetor(Vetor* V);
int tamanho_vetor(const Vetor* V);
char elemento(const Vetor* V, int i);
void vetor_clear(Vetor *V);
void adiciona_elemento(Vetor* V, char x, int i);
void remove_elemento(Vetor* V, int i);
void vetor_memcpy(Vetor *dest, const Vetor *src, int ini, int fim);
void organize_buffer(Vetor *src);

#endif
