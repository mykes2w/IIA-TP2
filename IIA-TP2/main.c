#include <stdio.h>
#include "algoritmo.h"

int main() {
    srand(time(NULL));

    static Problema p;
    Solucao s_inicial,s_hc_rand,s_hc_dois;
    Solucao s_genetico,s_hib1,s_hib2;
    char nome_ficheiro[] = "tourism_500.txt";
    ler_ficheiro(nome_ficheiro,&p);

    sol_inicial(&s_inicial,&p);
    printf("Solucao Inicial (Random): %.4f\n",s_inicial.media);
    printf("--------------------------------------------------------\n");

    printf("Trepa-Colinas (Vizinhanca Aleatoria - 1000 iteracoes)\n");
    sub_sol(&s_hc_rand,&s_inicial);
    trepa_colinas(&s_hc_rand,&p,1000,1);
    printf("     Resultado: %.4f\n",s_hc_rand.media);

    printf("Trepa-Colinas (Vizinhança 2 - 1000 iteracoes)\n");
    sub_sol(&s_hc_dois,&s_inicial);
    trepa_colinas(&s_hc_dois,&p,1000,2);
    printf("     Resultado: %.4f\n",s_hc_dois.media);

    printf("\nAlgoritmo Evolutivo\n");
    printf(" (Pop: %d | Geracoes: %d | Cross: %.2f | Mut: %.2f)\n",TAM_POP,NUM_GERACOES,PROB_CROSS,PROB_MUT);

    algoritmo_genetico(&p,&s_genetico);
    printf("Resultado GA:%.4f\n",s_genetico.media);

    int cnt=0;
    for (int i=0;i<p.n_candidaturas;i++) if (s_genetico.sel[i]==1) cnt++;
    if (cnt!=p.n_locais) printf("ERRO:Solucao Invalida (%d locais)\n",cnt);

    printf("\nAbordagens Hibridas\n");

    hibrido_sequencial(&p,&s_hib1);
    printf("Resultado Hibrido 1: %.4f\n",s_hib1.media);

    fflush(stdout);
    hibrido_intercalado(&p,&s_hib2);
    printf("Resultado Hibrido 2: %.4f\n",s_hib2.media);

    return 0;
}