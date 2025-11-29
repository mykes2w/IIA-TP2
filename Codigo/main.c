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
        for (int j = 0; j < p->n_candidaturas; j++) {
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

void sol_inicial(Solucao *s ,Problema *p) { //gerar uma soluçao inicial para ter um termo de comparaçao
    for (int i=0; i<p->n_candidaturas; i++) {
        s->sel[i] = 0;
    }

    int sel_cnt=0;
    while (sel_cnt<p->n_locais) {
        int r=rand()%p->n_candidaturas;
        if (s->sel[r]==0) {
            s->sel[r] = 1;
            sel_cnt++;
        }
    }

    media_distancias(s,p);
}

void sub_sol(Solucao *destino, Solucao *origem) { //serve para guardar a melhor soluçao caso a nova seja pior
    destino->media = origem->media;
    for (int i=0;i<MAX_C;i++) {
        destino->sel[i] =origem->sel[i];
    }
}

void gera_vizinho_random(Solucao *s ,Problema *p) { //funcao para que tenhas sempre M locais , para n ter mais do que o possivel
    int saiu=-1,entrou =-1;
    do {
        saiu=rand()%p->n_candidaturas;
    }while (s->sel[saiu]==0);

    do {
        entrou=rand()%p->n_candidaturas;
    }while (s->sel[entrou]==1);

    s->sel[saiu] = 0;
    s->sel[entrou] = 1;

    media_distancias(s,p);
}

void gera_vizinho_pior (Solucao *s,Problema *p) { //esta funçao encontra o pior dos locais , ou seja o que contribui menos para eles estarem "longe" uns dos outros
    int pior_indice=-1;
    double menor_soma_distancia=-1.0;
    //encontra o que esta mais perto dos outros , ou seja , o que esta mais perto do resto
    for (int i=0;i<p->n_candidaturas;i++) {
        if (s->sel[i]==1) {
            double soma_atual =0.0;
            for (int j=0;j<p->n_candidaturas;j++) {
                soma_atual+=p->matriz_distancia[i][j];
            }

            if (pior_indice==-1 || soma_atual<menor_soma_distancia) {
                menor_soma_distancia=soma_atual;
                pior_indice=i;
            }
        }
    }

    //agr é basicamente trocar um ponto random de fora pelo que foi determinado antes


    if (pior_indice!=-1) {
        int entrou=-1;
        do {
            entrou = rand() % p->n_candidaturas;
        }while (s->sel[entrou]==1);

        s->sel[pior_indice] = 0;
        s->sel[entrou] = 1;

        media_distancias(s,p);
    }
}
void trepa_colinas(Solucao *sol_atual,Problema *p,int n_iter,int tipo_viz) { //faz a mudança de uma media para a outra se tiver melhor guarda , se nao volta ao anterior
    Solucao sol_vizinha;

    for (int i=0;i<n_iter;i++) {
        sub_sol(&sol_vizinha,sol_atual);
        if (tipo_viz==1) {
            gera_vizinho_random(&sol_vizinha,p);
        }else {
            gera_vizinho_pior(&sol_vizinha,p);
        }
        if (sol_vizinha.media>sol_atual->media) {
            sub_sol(sol_atual,&sol_vizinha);
        }
    }
}
int main() {
    srand(time(NULL));

    Problema prob;
    Solucao sol_inic,sol_final;

    ler_ficheiro("tourism_5.txt",&prob);

    printf("\nGerar uma solucao inicial aleatoria\n");
    sol_inicial(&sol_inic,&prob);

    printf("\nSolucao Inicial (Aleatoria)\n");
    printf("Media: %.4f\n",sol_inic.media);
    printf("Locais: ");
    for (int i=0;i<prob.n_candidaturas;i++) {
        if (sol_inic.sel[i]==1) {
            printf("e%d " , i+1);
        }
    }
    printf("\n");

    //Testar com a Vizinhança 1 - Aleatoria
    printf("A executar o Trepa-Colinas (Vizinhanca Aleatoria)...\n");
    sub_sol(&sol_final,&sol_inic);
    trepa_colinas(&sol_final,&prob,1000,1);
    printf("Media Final: %.4f\n",sol_final.media);
    printf("Locais: ");
    for (int i=0;i<prob.n_candidaturas;i++) {
        if (sol_final.sel[i]==1) {
            printf("e%d " , i+1);
        }
    }
    printf("\n");

    //Testar com Vizinhança 2 - Pior
    printf("A Executar o Trepa-Colinas(Vizinhanca 2) \n");
    sub_sol(&sol_final,&sol_inic);
    trepa_colinas(&sol_final,&prob,1000,2);
    printf("Media Final: %.4f\n",sol_final.media);
    printf("Locais: ");
    for (int i=0;i<prob.n_candidaturas;i++) {
        if (sol_final.sel[i]==1) {
            printf("e%d " , i+1);
        }
    }
    printf("\n");

    return 0;
}
