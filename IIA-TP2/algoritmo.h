#ifndef CODIGO_ALGORITMO_H
#define CODIGO_ALGORITMO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_C 500
#define BUFF_SIZE 100
#define TAM_POP 100
#define T_SIZE 5
#define NUM_GERACOES 5000
#define PROB_CROSS 0.5
#define PROB_MUT 0.1

typedef struct Problema{
    int n_candidaturas;
    int n_locais;
    double matriz_distancia[MAX_C][MAX_C];
}Problema;

typedef struct {
    int sel[MAX_C];
    double media;
}Solucao;

typedef struct {
    Solucao pessoas[TAM_POP];
}Populacao;

void ler_ficheiro(char *nome_fich, Problema *p);
void media_distancias(Solucao *s,Problema *p);
void sol_inicial(Solucao *s,Problema *p);
void sub_sol(Solucao *destino, Solucao *origem);

void gera_vizinho_random(Solucao *s,Problema *p);
void gera_vizinho_pior(Solucao *s,Problema *p);

void trepa_colinas(Solucao *sol_atual,Problema *p,int n_iter,int tipo_viz);

void inic_pop(Populacao *p,Problema *prob);
void torneio(Populacao *p,Solucao *vencedor);
void roleta(Populacao *p,Solucao *vencedor);
void rep_solucao(Solucao *s, Problema *p);
void crossover(Solucao *p1,Solucao *p2,Solucao *f1,Solucao *f2,Problema *p);
void crossover_uniforme(Solucao *p1,Solucao *p2,Solucao *f1,Solucao *f2,Problema *p);

void mutacao(Solucao *s,Problema *p);
void mutacao_bitflip(Solucao *s,Problema *p);
void algoritmo_genetico(Problema *p,Solucao *melhor);

void hibrido_sequencial(Problema *p,Solucao *melhor);
void hibrido_intercalado(Problema *p,Solucao *melhor);

#endif