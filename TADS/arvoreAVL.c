#include <stdio.h>
#include <stdlib.h>
#include "arvoreAVL.h"

// Funções auxiliares (uso interno)
int altura(NoAVL *N) {
    if (N == NULL) return 0;
    return N->altura;
}

int maior(int a, int b) {
    if(a > b) return a;
    else return b;
}

NoAVL* rotacaoDireita(NoAVL *y) {
    NoAVL *x = y->esq;
    NoAVL *T2 = x->dir;

    x->dir = y;
    y->esq = T2;

    y->altura = maior(altura(y->esq), altura(y->dir)) + 1;
    x->altura = maior(altura(x->esq), altura(x->dir)) + 1;

    return x;
}

NoAVL* rotacaoEsquerda(NoAVL *x) {
    NoAVL *y = x->dir;
    NoAVL *T2 = y->esq;

    y->esq = x;
    x->dir = T2;

    x->altura = maior(altura(x->esq), altura(x->dir)) + 1;
    y->altura = maior(altura(y->esq), altura(y->dir)) + 1;

    return y;
}

int obterBalanceamento(NoAVL *N) {
    if (N == NULL) return 0;
    return altura(N->esq) - altura(N->dir);
}

// Implementação das funções públicas da interface
NoAVL* criarNoAVL(int chave, int numero_registro) {
    NoAVL* novo = (NoAVL*)malloc(sizeof(NoAVL));
    if (novo) {
        novo->chave = chave;
        novo->numero_registro = numero_registro; // Agora armazena a posição no disco
        novo->esq = NULL;
        novo->dir = NULL;
        novo->altura = 1; 
    }
    return novo;
}

NoAVL* inserirAVL(NoAVL* no, int chave, int numero_registro) {
    if (no == NULL) return criarNoAVL(chave, numero_registro);

    if (chave < no->chave)
        no->esq = inserirAVL(no->esq, chave, numero_registro);
    else if (chave > no->chave)
        no->dir = inserirAVL(no->dir, chave, numero_registro);
    else
        return no; // Chaves iguais não são permitidas (conforme requisito do arquivo)

    no->altura = 1 + maior(altura(no->esq), altura(no->dir));

    int balanco = obterBalanceamento(no);

    // Casos de Rotação
    if (balanco > 1 && chave < no->esq->chave)
        return rotacaoDireita(no);

    if (balanco < -1 && chave > no->dir->chave)
        return rotacaoEsquerda(no);

    if (balanco > 1 && chave > no->esq->chave) {
        no->esq = rotacaoEsquerda(no->esq);
        return rotacaoDireita(no);
    }

    if (balanco < -1 && chave < no->dir->chave) {
        no->dir = rotacaoDireita(no->dir);
        return rotacaoEsquerda(no);
    }

    return no;
}

NoAVL* buscarAVL(NoAVL* raiz, int chave) {
    NoAVL* atual = raiz;
    
    while (atual != NULL) {
        if (chave == atual->chave) {
            return atual; // Encontrou o nó com a chave e o numero_registro
        } else if (chave < atual->chave) {
            atual = atual->esq; 
        } else {
            atual = atual->dir; 
        }
    }
    
    return NULL; // Não encontrou
}

void liberarAVL(NoAVL* raiz) {
    if (raiz != NULL) {
        liberarAVL(raiz->esq);
        liberarAVL(raiz->dir);
        free(raiz);
    }
}