#ifndef TABELAHASH_H
#define TABELAHASH_H

// Nó da lista encadeada para tratamento de colisões
typedef struct NoHash {
    int chave;               // ID/Matrícula do funcionário
    int numero_registro;     // Posição correspondente no arquivo em disco
    struct NoHash *proximo;  // Ponteiro para o próximo nó em caso de colisão
} NoHash;

// Estrutura da Tabela Hash
typedef struct {
    NoHash **buckets;        // Vetor de ponteiros para os nós (lista encadeada)
    int tamanho;             // Tamanho da tabela (capacidade máxima)
} TabelaHash;

// --- Interface Pública do TAD ---

// Aloca a memória para a tabela e inicializa os buckets com NULL
TabelaHash* criarTabelaHash(int tamanho);

// Função de espalhamento (pode usar o método da divisão: chave % tamanho)
int calcularHash(int chave, int tamanho);

// Insere o par <chave, numero_registro> na tabela
void inserirHash(TabelaHash *ht, int chave, int numero_registro);

// Realiza a busca por uma chave. 
// O parâmetro 'colisoes' é um ponteiro que retornará a quantidade de colisões encontradas nesta busca específica.
// Retorna o numero_registro se encontrar, ou -1 caso não exista.
int buscarHash(TabelaHash *ht, int chave, int *colisoes);

// Dica 2 do enunciado: Caminhamento simples para mostrar os pares <chave, numero_registro>
void imprimirTabelaHash(TabelaHash *ht);

// Libera toda a memória alocada para os nós e para a tabela
void liberarTabelaHash(TabelaHash *ht);

#endif