#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registro.h"

// --- Função Auxiliar ---
void imprimirFuncionario(Funcionario f) {
    printf("ID: %-6d | Nome: %-20s | Cargo: %-15s | Idade: %d | Salario: R$ %.2f\n", 
           f.id_funcionario, f.nome, f.cargo, f.idade, f.salario);
}

// --- Funções de Inicialização ---
// --- Funções de Inicialização ---
void gerarArquivoDados(const char *nomeArquivo, int quantidade) {
    FILE *arquivo = fopen(nomeArquivo, "wb");
    if (arquivo == NULL) {
        printf("Erro ao criar o arquivo de banco de dados!\n");
        return;
    }

    // Vetores ampliados para maior diversidade
    const char *nomes[] = {"Ana", "Carlos", "Beatriz", "Daniel", "Eduarda", "Fernando", "Gabriela", "Henrique", "Isabela", "Joao", "Laura", "Marcelo", "Natalia", "Otavio", "Paula", "Rafael", "Sofia", "Thiago", "Vitoria", "Bruno"};
    const char *sobrenomes[] = {"Silva", "Souza", "Costa", "Santos", "Lima", "Alves", "Ribeiro", "Martins", "Carvalho", "Gomes", "Mendes", "Ferreira", "Rodrigues", "Almeida", "Nunes", "Vieira", "Barbosa", "Rocha", "Dias", "Moraes"};
    const char *cargos[] = {"Desenvolvedor Junior", "Desenvolvedor Pleno", "Desenvolvedor Senior", "Analista de Sistemas", "Gerente de Projetos", "Arquiteto de Software", "Tech Lead", "Engenheiro de Dados", "Cientista de Dados", "DevOps", "QA", "Scrum Master"};

    // Calculando o tamanho dos vetores dinamicamente
    int num_nomes = sizeof(nomes) / sizeof(nomes[0]);
    int num_sobrenomes = sizeof(sobrenomes) / sizeof(sobrenomes[0]);
    int num_cargos = sizeof(cargos) / sizeof(cargos[0]);

    for (int i = 0; i < quantidade; i++) {
        Funcionario f;
        
        // Mantém a lógica da Chave Única (ID) que é a regra de ouro do trabalho
        f.id_funcionario = 1000 + (i * 3); 
        
        // A função sprintf junta o nome e o sobrenome com um espaço no meio
        sprintf(f.nome, "%s %s", nomes[rand() % num_nomes], sobrenomes[rand() % num_sobrenomes]);
        
        strcpy(f.cargo, cargos[rand() % num_cargos]);
        
        f.idade = 20 + (rand() % 40); // Idade entre 20 e 59
        f.salario = 3000.0 + (rand() % 15000); // Salário entre 3k e 18k

        fwrite(&f, sizeof(Funcionario), 1, arquivo);
    }

    fclose(arquivo);
    printf("Arquivo '%s' gerado com sucesso com %d registros super diversificados.\n", nomeArquivo, quantidade);
}

// --- Funções de Acesso Direto ---
Funcionario lerRegistro(FILE *arquivo, int numero_registro) {
    Funcionario f;
    
    // Zera a struct por segurança caso a leitura falhe
    f.id_funcionario = -1; 

    if (arquivo == NULL) return f;

    // A MÁGICA ACONTECE AQUI:
    // SEEK_SET significa "a partir do início do arquivo"
    // Pulamos (numero_registro * tamanho_do_funcionario) bytes.
    long offset = (long)numero_registro * sizeof(Funcionario);
    fseek(arquivo, offset, SEEK_SET);

    // Lê exatamente 1 bloco do tamanho de um Funcionario na posição que paramos
    fread(&f, sizeof(Funcionario), 1, arquivo);

    return f;
}

// --- Funções de Busca Sequencial (Varredura Completa) ---
int buscaSequencialPorChave(const char *nomeArquivo, int id_buscado) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL) return -1;

    Funcionario f;
    int posicao = 0;
    int encontrado = -1;

    // Lê registro por registro sequencialmente até o fim do arquivo
    while (fread(&f, sizeof(Funcionario), 1, arquivo) == 1) {
        if (f.id_funcionario == id_buscado) {
            encontrado = posicao;
            break; // Achou, pode parar a busca
        }
        posicao++;
    }

    fclose(arquivo);
    return encontrado; // Retorna a posição (numero_registro) ou -1 se não achar
}

void buscaSequencialPorSalario(const char *nomeArquivo, float salario_buscado, const char* operador) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL) return;

    Funcionario f;
    int encontrou_algum = 0;

    printf("--- Busca Sequencial: Salario %s %.2f ---\n", operador, salario_buscado);

    while (fread(&f, sizeof(Funcionario), 1, arquivo) == 1) {
        int condicao_atendida = 0;

        // Verifica qual operador lógico foi solicitado usando strcmp
        if (strcmp(operador, ">") == 0 && f.salario > salario_buscado) condicao_atendida = 1;
        else if (strcmp(operador, ">=") == 0 && f.salario >= salario_buscado) condicao_atendida = 1;
        else if (strcmp(operador, "<") == 0 && f.salario < salario_buscado) condicao_atendida = 1;
        else if (strcmp(operador, "<=") == 0 && f.salario <= salario_buscado) condicao_atendida = 1;

        if (condicao_atendida) {
            imprimirFuncionario(f);
            encontrou_algum = 1;
        }
    }

    if (!encontrou_algum) {
        printf("Nenhum funcionario encontrado nessa faixa salarial.\n");
    }

    fclose(arquivo);
}