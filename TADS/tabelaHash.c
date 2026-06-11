#include <stdio.h>
#include <stdlib.h>
#include "tabelaHash.h"

// --- Criação e Inicialização ---
TabelaHash* criarTabelaHash(int tamanho) {
    TabelaHash *ht = (TabelaHash*)malloc(sizeof(TabelaHash));
    ht->tamanho = tamanho;
    ht->buckets = (NoHash**)malloc(tamanho * sizeof(NoHash*));
    
    // Inicializa todos os ponteiros do vetor com NULL
    for (int i = 0; i < tamanho; i++) {
        ht->buckets[i] = NULL;
    }
    
    return ht;
}

// --- Função de Espalhamento (Hash) ---
// O método da divisão simples é ideal aqui. 
// Para forçar colisões como pede o trabalho, basta criar a tabela na main com um 'tamanho' pequeno.
int calcularHash(int chave, int tamanho) {
    return chave % tamanho;
}

// --- Inserção ---
void inserirHash(TabelaHash *ht, int chave, int numero_registro) {
    int indice = calcularHash(chave, ht->tamanho);
    
    // Aloca o novo nó com o par <chave, registro>
    NoHash *novo = (NoHash*)malloc(sizeof(NoHash));
    novo->chave = chave;
    novo->numero_registro = numero_registro;
    
    // Insere no início da lista encadeada daquele índice (bucket)
    novo->proximo = ht->buckets[indice];
    ht->buckets[indice] = novo;
}

// --- Busca e Contagem de Colisões ---
int buscarHash(TabelaHash *ht, int chave, int *colisoes) {
    int indice = calcularHash(chave, ht->tamanho);
    NoHash *atual = ht->buckets[indice];
    
    *colisoes = 0; // Zera o contador para a busca atual
    
    while (atual != NULL) {
        if (atual->chave == chave) {
            return atual->numero_registro; // Achou o registro
        }
        
        // Se entrou aqui e não é a chave buscada, ocorreu uma colisão na lista
        (*colisoes)++;
        atual = atual->proximo;
    }
    
    return -1; // Chave não encontrada
}

// --- Caminhamento (Impressão) ---
// Serve para mostrar os pares armazenados e comparar com a árvore.
void imprimirTabelaHash(TabelaHash *ht) {
    printf("\n=== Tabela Hash (Pares <Chave, Num. Registro>) ===\n");
    for (int i = 0; i < ht->tamanho; i++) {
        printf("Bucket [%d]: ", i);
        NoHash *atual = ht->buckets[i];
        
        if (atual == NULL) {
            printf("Vazio");
        } else {
            while (atual != NULL) {
                printf("<%d, %d> -> ", atual->chave, atual->numero_registro);
                atual = atual->proximo;
            }
            printf("NULL");
        }
        printf("\n");
    }
    printf("==================================================\n");
}

// --- Liberação de Memória ---
void liberarTabelaHash(TabelaHash *ht) {
    if (ht == NULL) return;
    
    // Libera os nós das listas encadeadas de cada bucket
    for (int i = 0; i < ht->tamanho; i++) {
        NoHash *atual = ht->buckets[i];
        while (atual != NULL) {
            NoHash *temp = atual;
            atual = atual->proximo;
            free(temp);
        }
    }
    
    // Libera o vetor de ponteiros e a estrutura principal
    free(ht->buckets);
    free(ht);
}