#ifndef REGISTRO_H
#define REGISTRO_H

#include <stdio.h>

// Estrutura com aproximadamente 92 bytes (atende ao requisito de ~100 bytes)
typedef struct {
    int id_funcionario;    // Chave primária (4 bytes) - Usada no Índice 1 e 2
    char nome[50];         // (50 bytes)
    char cargo[30];        // (30 bytes)
    float salario;         // Atributo não-chave (4 bytes) - Usado no Índice 3 (consultas de intervalo)
    int idade;             // (4 bytes)
} Funcionario;

// --- Funções de Inicialização ---
// Gera o arquivo binário inicial com 'n' registros aleatórios para os testes
void gerarArquivoDados(const char *nomeArquivo, int quantidade);

// --- Funções de Acesso Direto (Usadas em conjunto com as Árvores e Hash) ---
// Dá um fseek() direto para a posição do registro e retorna o Funcionario
Funcionario lerRegistro(FILE *arquivo, int numero_registro);

// --- Funções de Busca Sequencial (Varredura no Disco) ---
// Estratégia 3: Busca sequencial no arquivo inteiro pelo atributo chave
int buscaSequencialPorChave(const char *nomeArquivo, int id_buscado);

// Estratégia 5: Busca sequencial no arquivo por intervalo de salário (>, >=, <, <=)
// O parâmetro 'operador' pode receber strings como ">", "<=", etc., para definir a lógica
void buscaSequencialPorSalario(const char *nomeArquivo, float salario_buscado, const char* operador);

// --- Função Auxiliar ---
// Imprime os dados de um funcionário formatados na tela
void imprimirFuncionario(Funcionario f);

#endif