#ifndef AVL_H
#define AVL_H

typedef struct NoAVL {
    int chave;               // O ID ou Matrícula do funcionário
    int numero_registro;     // A posição dele no arquivo binário (ex: 0, 1, 2...)
    int altura;
    struct NoAVL *esq;
    struct NoAVL *dir;
} NoAVL;

// As assinaturas das funções agora recebem o numero_registro em vez de 'char conteudo'
NoAVL* criarNoAVL(int chave, int numero_registro);
NoAVL* inserirAVL(NoAVL* no, int chave, int numero_registro);
NoAVL* buscarAVL(NoAVL* raiz, int chave);
void liberarAVL(NoAVL* raiz);

#endif