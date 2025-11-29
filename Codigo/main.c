#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_C 500
#define BUFF_SIZE 100

typedef struct Problema{
    int n_candidaturas;
    int n_locais;
    double matriz_distancia[MAX_C][MAX_C];
}Problema;

typedef struct {
    int sel[MAX_C];
    double media;
}Solucao;

void ler_ficheiro(char *nome_fich , Problema *p)
{
    FILE *ficheiro = fopen(nome_fich, "r");
    if (!ficheiro)
    {
        printf("Erro ao abrir o Ficheiro: %s\n ", nome_fich);
        exit(1);
    }
    if (fscanf(ficheiro,"%d %d" , &p->n_candidaturas, &p->n_locais) != 2) {
        printf("Erro ao ler dimensoes do ficheiro.\n");
        exit(1);
    }
    for (int i = 0; i < p->n_candidaturas; i++) {
        for (int j = 0; j < p->n_locais; j++) {
            p->matriz_distancia[i][j] = 0.0;
        }
    }
    char buf1[20],buf2[20];
    double distancia;
    int id1,id2;

    while (fscanf(ficheiro,"%s %s %lf" , buf1 , buf2 , &distancia ) == 3) {
        sscanf(buf1 , "%*c%d" , &id1); //guarda e deita fora o "e" , ou seja deixa apenas o nr do local
        sscanf(buf2 , "%*c%d" , &id2);

        id1--; //isto é para preencher o primeiro valor da matriz e nao deixar um em branco
        id2--;

        if (id1 >= 0 && id1 < p->n_candidaturas && id2 >= 0 && id2 < p->n_candidaturas) {
            p->matriz_distancia[id1][id2] = distancia;
            p->matriz_distancia[id2][id1] = distancia;
        }
    }

    fclose(ficheiro);
    printf("Leitura Concluida: C=%d M=%d\n",p->n_candidaturas,p->n_locais);
}

void media_distancias(Solucao *s ,Problema *p) {
    double soma_distancia=0.0;

    for (int i=0; i<p->n_candidaturas-1; i++) {
        if (s->sel[i] == 1) {
            for (int j=i+1; j<p->n_candidaturas; j++) {
                if (s->sel[j]==1) {
                    soma_distancia+=p->matriz_distancia[i][j];
                }
            }
        }
    }

    if (p->n_locais>0) {
        s->media = soma_distancia/p->n_locais;
    }
    else {
        s->media = 0.0;
    }
}

void sol_inicial(Solucao *s ,Problema *p) {
    for (int i=0; i<p->n_candidaturas; i++) {
        s->sel[i] = 0;
    }

    int sel_cnt=0;
    while (sel_cnt<p->n_locais) {
        int r=rand()%p->n_locais;
        if (s->sel[r]==0) {
            s->sel[r] = 1;
            sel_cnt++;
        }
    }

    media_distancias(s,p);
}

int main() {
    srand(time(NULL));

    Problema prob;
    Solucao sol;

    ler_ficheiro("tourism_5.txt",&prob);

    printf("\nGerar uma soluçao inicial aleatoria\n");
    sol_inicial(&sol,&prob);

    printf("Solucao Gerada: ");
    for (int i=0;i<prob.n_candidaturas;i++) {
        if (sol.sel[i]==1) {
            printf("e%d" , i+1);
        }
    }
    printf("\nMedia: %.4f\n",sol.media);
    return 0;
}