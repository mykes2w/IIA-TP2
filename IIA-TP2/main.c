#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h> // Necessário para calcular o Desvio Padrão
#include "algoritmo.h"

// --- Função auxiliar para correr testes 30 vezes e dar as estatísticas ---
void correr_teste_estatistico(Problema *p, int tipo_algo, int vizinhanca) {
    float resultados[30];
    float soma = 0.0, media_final = 0.0, melhor_global = 0.0, desvio_padrao = 0.0;
    Solucao s_in, s_out;
    int runs_hc = 1000; // Iterações do Trepa-Colinas

    printf("\n>>> A executar 30 vezes... Aguarde. <<<\n");

    for (int i = 0; i < 30; i++) {
        sol_inicial(&s_in, p); // Gera nova solução aleatória

        // Copia para s_out para não estragar a s_in
        sub_sol(&s_out, &s_in);

        // Escolhe o algoritmo
        if (tipo_algo == 1) { // Trepa Colinas
            trepa_colinas(&s_out, p, runs_hc, vizinhanca);
        } else if (tipo_algo == 2) { // Genético
            algoritmo_genetico(p, &s_out);
        } else if (tipo_algo == 3) { // Híbrido
            hibrido_sequencial(p, &s_out);
            //hibrido_intercalado(p, &s_out);
        }

        // Guarda o resultado
        resultados[i] = s_out.media;

        // CORREÇÃO: Aqui estava "results", mudei para "resultados"
        soma += resultados[i];

        // Atualiza o melhor global encontrado
        if (i == 0 || s_out.media > melhor_global) {
            melhor_global = s_out.media;
        }
    }

    // Calcular Média
    media_final = soma / 30.0;

    // Calcular Desvio Padrão
    float soma_quadrados = 0.0;
    for (int i = 0; i < 30; i++) {
        soma_quadrados += pow(resultados[i] - media_final, 2);
    }
    desvio_padrao = sqrt(soma_quadrados / 30.0);

    // IMPRIMIR RESULTADOS PARA O EXCEL
    printf("\n------------------------------------------------");
    printf("\nRESULTADOS ESTATISTICOS (30 Execucoes)");
    if(tipo_algo == 1) printf("\nAlgoritmo: Trepa-Colinas (Viz %d)", vizinhanca);
    if(tipo_algo == 2) printf("\nAlgoritmo: Genetico");
    if(tipo_algo == 3) printf("\nAlgoritmo: Hibrido");
    printf("\n------------------------------------------------");
    printf("\nMelhor (Best):   %.4f", melhor_global);
    printf("\nMedia (Avg):     %.4f", media_final);
    printf("\nDesvio Padrao:   %.4f", desvio_padrao);
    printf("\n------------------------------------------------\n");
}

int main() {
    srand(time(NULL));
    static Problema p;
    Solucao s_inicial, s_final, s_genetico, s_hibrido;
    char nome_ficheiro[] = "tourism_500.txt";
    int opcao, sub_opcao;
    int runs = 1000;

    // Carregar ficheiro
    ler_ficheiro(nome_ficheiro, &p);

    do {
        printf("\n\n### MENU PROJETO IIA ###\n");
        printf("1 - Pesquisa Local (Uma execucao)\n");
        printf("2 - Algoritmo Evolutivo (Uma execucao)\n");
        printf("3 - Algoritmo Hibrido (Uma execucao)\n");
        printf("4 - [TESTES AUTOMATICOS] - Correr 30x para Excel\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n');
            continue;
        }

        sol_inicial(&s_inicial, &p);

        switch (opcao) {
            case 1: // TREPA COLINAS SINGLE RUN
                printf("Qual a vizinhanca? (1 ou 2): ");
                scanf("%d", &sub_opcao);
                sub_sol(&s_final, &s_inicial);
                if (sub_opcao == 1 || sub_opcao == 2) {
                    trepa_colinas(&s_final, &p, runs, sub_opcao);
                    printf("Resultado: %.4f\n", s_final.media);
                }
                break;

            case 2: // GENETICO SINGLE RUN
                algoritmo_genetico(&p, &s_genetico);
                printf("Resultado GA: %.4f\n", s_genetico.media);
                break;

            case 3: // HIBRIDO SINGLE RUN
                hibrido_sequencial(&p, &s_hibrido);
                printf("Resultado Hibrido: %.4f\n", s_hibrido.media);
                break;

            case 4: // MODO AUTOMATICO PARA O EXCEL
                printf("\n--- MODO TESTE ESTATISTICO ---\n");
                printf("Qual algoritmo quer testar 30x?\n");
                printf("1 - Trepa-Colinas\n");
                printf("2 - Genetico\n");
                printf("3 - Hibrido\n");
                printf("Escolha: ");
                int t_algo;
                scanf("%d", &t_algo);

                if (t_algo == 1) {
                    printf("Qual a vizinhanca? (1 ou 2): ");
                    int t_viz;
                    scanf("%d", &t_viz);
                    correr_teste_estatistico(&p, 1, t_viz);
                } else {
                    correr_teste_estatistico(&p, t_algo, 0);
                }
                break;

            case 0: break;
            default: printf("Opcao invalida!\n");
        }
    } while (opcao != 0);

    return 0;
}