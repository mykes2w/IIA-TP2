#include "algoritmo.h"

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

void inic_pop(Populacao *p,Problema *prob) { //inicializa a populaçao com 100 soluçoes randoms validas
    for (int i=0;i<TAM_POP;i++) {
        sol_inicial(&p->pessoas[i],prob);
    }
}

void torneio(Populacao *p,Solucao *vencedor) { //basicamente aqui isto vai escolher 5 pessoas random e devolve o melhor desses 5 (para se tornar progenitor)
    int r=rand() % TAM_POP;
    sub_sol(vencedor,&p->pessoas[r]); //assume que o primeiro vai ser o melhor

    for (int i=1;i<T_SIZE;i++) {
        r=rand() % TAM_POP;
        if (p->pessoas[r].media>vencedor->media) {
            sub_sol(vencedor,&p->pessoas[r]);
        }
    }
}

void rep_solucao(Solucao *s, Problema *p) {
    int cnt=0;

    for (int i=0;i<p->n_candidaturas;i++) {
        if (s->sel[i]==1) {cnt++;}
    }

    int ajuda=0;
    while (cnt>p->n_locais && ajuda<5000) {
        int r=rand() % p->n_candidaturas;
        if (s->sel[r]==1) {
            s->sel[r]=0;
            cnt--;
        }
        ajuda++;
    }
    ajuda=0;
    while (cnt<p->n_locais && ajuda<5000) {
        int r=rand() % p->n_candidaturas;
        if (s->sel[r]==0) {
            s->sel[r]=1;
            cnt++;
        }
        ajuda++;
    }
    media_distancias(s,p);
}

void crossover(Solucao *p1,Solucao *p2,Solucao *f1,Solucao *f2,Problema *p) {
    int cortar = (rand() % (p->n_candidaturas-1))+1;

    for (int i=0;i<p->n_candidaturas;i++) {
        if (i<cortar) {
            f1->sel[i] = p1->sel[i];
            f2->sel[i] = p2->sel[i];
        }else {
            f1->sel[i] = p2->sel[i];
            f2->sel[i] = p1->sel[i];
        }
    }

    rep_solucao(f1,p);
    rep_solucao(f2,p);
}

void mutacao(Solucao *s,Problema *p) {
    int saiu,entrou;
    int ajuda=0;

    do {
        saiu=rand() % p->n_candidaturas;
        ajuda++;
        if (ajuda>1000) return;
    }while (s->sel[saiu]==0);
    ajuda =0;
    do {
        entrou=rand() % p->n_candidaturas;
        ajuda++;
        if (ajuda>1000) return;
    }while (s->sel[entrou]==1);

    s->sel[saiu]=0;
    s->sel[entrou]=1;

    media_distancias(s,p);
}

void algoritmo_genetico(Problema *p,Solucao *melhor) {
    static Populacao pop_atual,pop_nova;
    Solucao pai1,pai2,filho1,filho2;

    inic_pop(&pop_atual,p);

    *melhor=pop_atual.pessoas[0];
    for (int i=1;i<TAM_POP;i++) {
        if (pop_atual.pessoas[i].media>melhor->media) {
            *melhor=pop_atual.pessoas[i];
        }
    }

    for (int gen=0;gen<NUM_GERACOES;gen++) {
        int index=0;

        if (index<TAM_POP) {
            pop_nova.pessoas[index++]=*melhor;
        }

        while (index<TAM_POP) {
            torneio(&pop_atual,&pai1);
            torneio(&pop_atual,&pai2);

            if ((rand() / (double) RAND_MAX) < PROB_CROSS) {
                //crossover(&pai1,&pai2,&filho1,&filho2,p);
                crossover_uniforme(&pai1,&pai2,&filho1,&filho2,p);
            }else {
                sub_sol(&filho1,&pai1);
                sub_sol(&filho2,&pai2);
            }

            if ((rand() / (double) RAND_MAX) < PROB_MUT){ //mutacao(&filho1,p)
                mutacao_bitflip(&filho1,p);
                }
            if ((rand() / (double) RAND_MAX) < PROB_MUT){ //mutacao(&filho2,p);
                mutacao_bitflip(&filho2,p);
                }

            if (index<TAM_POP) pop_nova.pessoas[index++]=filho1;
            if (index<TAM_POP) pop_nova.pessoas[index++]=filho2;

        }
        pop_atual=pop_nova;

        for (int i=0;i<TAM_POP;i++) {
            if (pop_nova.pessoas[i].media>melhor->media) {
                *melhor=pop_nova.pessoas[i];
            }
        }
    }
}

void roleta(Populacao *p,Solucao *vencedor) {
    double media_total=0.0;

    for (int i=0;i<TAM_POP;i++) {
        media_total+=p->pessoas[i].media;
    }

    double valor_sorteado = (rand() / (double)RAND_MAX) * media_total;

    double soma_atual=0.0;
    for (int i=0;i<TAM_POP;i++) {
        soma_atual+=p->pessoas[i].media;
        if (soma_atual>=valor_sorteado) {
            sub_sol(vencedor,&p->pessoas[i]);
            return;
        }
    }
    sub_sol(vencedor,&p->pessoas[TAM_POP-1]);
}

void crossover_uniforme(Solucao *p1,Solucao *p2,Solucao *f1,Solucao *f2,Problema *p) {
    for (int i=0;i<p->n_candidaturas;i++) {
        if (rand() % 2 ==0) {
            f1->sel[i] = p1->sel[i];
            f2->sel[i] = p2->sel[i];
        }else {
            f1->sel[i] = p2->sel[i];
            f2->sel[i] = p1->sel[i];
        }
    }
    rep_solucao(f1,p);
    rep_solucao(f2,p);
}

void mutacao_bitflip(Solucao *s,Problema *p) {
    int ponto = rand() % p->n_candidaturas;

    if (s->sel[ponto]==0) {
        s->sel[ponto]=1;
    }else {
        s->sel[ponto]=0;
    }

    rep_solucao(s,p);
}

void hibrido_sequencial(Problema *p,Solucao *melhor) {
    algoritmo_genetico(p,melhor);
    trepa_colinas(melhor,p,2000,2);
}

void hibrido_intercalado(Problema *p,Solucao *melhor) {
    static Populacao pop_atual,pop_nova;
    Solucao pai1,pai2,filho1,filho2;

    inic_pop(&pop_atual,p);

    *melhor=pop_atual.pessoas[0];
    for (int i=1;i<TAM_POP;i++) {
        if (pop_atual.pessoas[i].media>melhor->media) {
            *melhor=pop_atual.pessoas[i];
        }
    }

    for (int gen=0;gen<NUM_GERACOES;gen++) {
        int index=0;
        if (index<TAM_POP) {
            pop_nova.pessoas[index++]=*melhor;
        }
        if (gen % 10==0) {
            trepa_colinas(melhor,p,100,2);
            pop_nova.pessoas[0]=*melhor;
        }
        while (index<TAM_POP) {
            torneio(&pop_atual,&pai1);
            torneio(&pop_atual,&pai2);

            if ((rand() / (double) RAND_MAX) < PROB_CROSS) {
                crossover(&pai1,&pai2,&filho1,&filho2,p);
            }else {
                sub_sol(&filho1,&pai1);
                sub_sol(&filho2,&pai2);
            }

            if ((rand() / (double) RAND_MAX) < PROB_MUT) mutacao(&filho1,p);
            if ((rand() / (double) RAND_MAX) < PROB_MUT) mutacao(&filho2,p);

            if (index<TAM_POP) pop_nova.pessoas[index++]=filho1;
            if (index<TAM_POP) pop_nova.pessoas[index++]=filho2;
        }
        pop_atual=pop_nova;
        for (int i=0;i<TAM_POP;i++) {
            if (pop_nova.pessoas[i].media>melhor->media) {
                *melhor=pop_nova.pessoas[i];
            }
        }
    }
}