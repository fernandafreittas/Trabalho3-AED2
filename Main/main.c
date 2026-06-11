#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../TADS/arvoreAVL.h"
#include "../TADS/tabelaHash.h"
#include "../TADS/registro.h"

// Defina um valor alto para que a busca no disco demore e você consiga medir o tempo.
// Se usar apenas 100, os tempos darão 0.0000s devido à velocidade extrema do C.
#define QTD_REGISTROS 700000 
#define QTD_TESTES 30
#define ARQUIVO_BD "dados.bin"

// --- Função Auxiliar para Busca de Intervalo na AVL ---
// Faz o caminhamento na árvore de salários e acessa o disco se a condição bater.
void buscarIntervaloAVL_Main(NoAVL* raiz, int valor_buscado, const char* operador, FILE* arquivo) {
    if (raiz == NULL) return;

    int atende_condicao = 0;
    if (strcmp(operador, ">") == 0 && raiz->chave > valor_buscado) atende_condicao = 1;
    else if (strcmp(operador, ">=") == 0 && raiz->chave >= valor_buscado) atende_condicao = 1;
    else if (strcmp(operador, "<") == 0 && raiz->chave < valor_buscado) atende_condicao = 1;
    else if (strcmp(operador, "<=") == 0 && raiz->chave <= valor_buscado) atende_condicao = 1;

    // Se atende, vai lá no disco e lê o registro em O(1)
    if (atende_condicao) {
        Funcionario f = lerRegistro(arquivo, raiz->numero_registro);
        // Descomente a linha abaixo para ver os resultados imprimindo (deixe comentado para o teste de tempo ficar limpo)
        // imprimirFuncionario(f); 
    }

    // Otimização do caminhamento baseado no operador
    if (strcmp(operador, ">") == 0 || strcmp(operador, ">=") == 0) {
        if (raiz->chave >= valor_buscado) buscarIntervaloAVL_Main(raiz->esq, valor_buscado, operador, arquivo);
        buscarIntervaloAVL_Main(raiz->dir, valor_buscado, operador, arquivo);
    } else {
        buscarIntervaloAVL_Main(raiz->esq, valor_buscado, operador, arquivo);
        if (raiz->chave <= valor_buscado) buscarIntervaloAVL_Main(raiz->dir, valor_buscado, operador, arquivo);
    }
}

int main() {
    // ==============================================================================
    // FASE 1: Geração e Carga Inicial (Simulando o "Boot" do Banco de Dados)
    // ==============================================================================
    srand(time(NULL)); // Garante aleatoriedade baseada no relógio do sistema
    printf("Inicializando o Banco de Dados...\n");
    gerarArquivoDados(ARQUIVO_BD, QTD_REGISTROS);

    NoAVL *indice_id_avl = NULL;
    NoAVL *indice_salario_avl = NULL;
    // Tabela Hash com 75% da capacidade para forçar algumas colisões
    TabelaHash *indice_id_hash = criarTabelaHash((int)(QTD_REGISTROS * 0.75)); 

    FILE *arquivo = fopen(ARQUIVO_BD, "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o banco de dados.\n");
        return 1;
    }

    Funcionario f;
    int num_registro = 0;
    printf("Carregando indices para a memoria RAM...\n");
    while (fread(&f, sizeof(Funcionario), 1, arquivo) == 1) {
        indice_id_avl = inserirAVL(indice_id_avl, f.id_funcionario, num_registro);
        inserirHash(indice_id_hash, f.id_funcionario, num_registro);
        indice_salario_avl = inserirAVL(indice_salario_avl, (int)f.salario, num_registro);
        num_registro++;
    }
    printf("Indices carregados com sucesso!\n\n");

    // ==============================================================================
    // FASE 2: Preparação dos Dados de Teste (30 Valores Distintos)
    // ==============================================================================
    int chaves_busca_id[QTD_TESTES];
    float chaves_busca_salario[QTD_TESTES];
    
    // Pegando valores espaçados para garantir diversidade nos testes
    for (int i = 0; i < QTD_TESTES; i++) {
        // Os IDs gerados no registro.c são 1000 + (i * 3)
        int index_aleatorio = (QTD_REGISTROS / QTD_TESTES) * i; 
        chaves_busca_id[i] = 1000 + (index_aleatorio * 3);
        
        // Salários variam, vamos testar faixas de 3000 a 18000
        chaves_busca_salario[i] = 3000.0 + (i * 500); 
    }

    // Variáveis para medição de tempo
    clock_t inicio, fim;
    double tempo_total;

    printf("====================================================\n");
    printf("           INICIANDO BENCHMARK (30 EXECUCOES)       \n");
    printf("====================================================\n\n");

    // ==============================================================================
    // ESTRATÉGIA 1: Busca AVL (Atributo Chave)
    // ==============================================================================
    double soma_tempos = 0;
    printf("\n1) DETALHES BUSCA AVL (CHAVE):\n");
    for (int i = 0; i < QTD_TESTES; i++) {
        inicio = clock();
        NoAVL* resultado = buscarAVL(indice_id_avl, chaves_busca_id[i]);
        if (resultado != NULL) {
            Funcionario func = lerRegistro(arquivo, resultado->numero_registro);
            (void)func;
        }
        fim = clock();
        double tempo_iter = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
        soma_tempos += tempo_iter;
        printf("   -> Execucao %2d: Buscando ID %-6d | Tempo: %f s\n", i+1, chaves_busca_id[i], tempo_iter);
    }
    printf("=> TEMPO MEDIO AVL (CHAVE): %f segundos\n", soma_tempos / QTD_TESTES);

    // ==============================================================================
    // ESTRATÉGIA 2: Busca HASH (Atributo Chave)
    // ==============================================================================
    int total_colisoes = 0;
    int colisoes_temp = 0;
    soma_tempos = 0;
    
    printf("\n2) DETALHES BUSCA HASH (CHAVE):\n");
    for (int i = 0; i < QTD_TESTES; i++) {
        inicio = clock();
        int pos = buscarHash(indice_id_hash, chaves_busca_id[i], &colisoes_temp);
        if (pos != -1) {
            Funcionario func = lerRegistro(arquivo, pos);
            (void)func;
        }
        fim = clock();
        double tempo_iter = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
        soma_tempos += tempo_iter;
        total_colisoes += colisoes_temp;
        printf("   -> Execucao %2d: Buscando ID %-6d | Tempo: %f s | Colisoes: %d\n", i+1, chaves_busca_id[i], tempo_iter, colisoes_temp);
    }
    printf("=> TEMPO MEDIO HASH (CHAVE): %f segundos (Media de %d colisoes/busca)\n", soma_tempos / QTD_TESTES, total_colisoes / QTD_TESTES);

    // ==============================================================================
    // ESTRATÉGIA 3: Busca Sequencial no Disco (Atributo Chave)
    // ==============================================================================
    soma_tempos = 0;
    printf("\n3) DETALHES BUSCA SEQ. DISCO (CHAVE):\n");
    for (int i = 0; i < QTD_TESTES; i++) {
        inicio = clock();
        buscaSequencialPorChave(ARQUIVO_BD, chaves_busca_id[i]);
        fim = clock();
        double tempo_iter = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
        soma_tempos += tempo_iter;
        printf("   -> Execucao %2d: Buscando ID %-6d | Tempo: %f s\n", i+1, chaves_busca_id[i], tempo_iter);
    }
    printf("=> TEMPO MEDIO SEQ. DISCO (CHAVE): %f segundos\n", soma_tempos / QTD_TESTES);

    // ==============================================================================
    // ESTRATÉGIA 4: Busca AVL por Intervalo (Atributo Não-Chave - Salário)
    // ==============================================================================
    soma_tempos = 0;
    printf("\n4) DETALHES BUSCA AVL INTERVALO (SALARIO > X):\n");
    for (int i = 0; i < QTD_TESTES; i++) {
        inicio = clock();
        buscarIntervaloAVL_Main(indice_salario_avl, (int)chaves_busca_salario[i], ">", arquivo);
        fim = clock();
        double tempo_iter = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
        soma_tempos += tempo_iter;
        printf("   -> Execucao %2d: Salario > %-7.2f | Tempo: %f s\n", i+1, chaves_busca_salario[i], tempo_iter);
    }
    printf("=> TEMPO MEDIO AVL (SALARIO): %f segundos\n", soma_tempos / QTD_TESTES);

    // ==============================================================================
    // ESTRATÉGIA 5: Busca Sequencial no Disco (Atributo Não-Chave - Salário)
    // ==============================================================================
    soma_tempos = 0;
    printf("\n5) DETALHES BUSCA SEQ. DISCO INTERVALO (SALARIO > X):\n");
    for (int i = 0; i < QTD_TESTES; i++) {
        inicio = clock();
        
        // Mutando o terminal para não sujar a tela com milhares de impressões
        #ifdef _WIN32
            freopen("nul", "w", stdout);
        #else
            freopen("/dev/null", "w", stdout);
        #endif
        
        buscaSequencialPorSalario(ARQUIVO_BD, chaves_busca_salario[i], ">");
        
        // Restaurando o terminal
        #ifdef _WIN32
            freopen("CON", "w", stdout);
        #else
            freopen("/dev/tty", "w", stdout);
        #endif

        fim = clock();
        double tempo_iter = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
        soma_tempos += tempo_iter;
        
        // Agora que o terminal voltou, imprimimos o tempo desta iteração
        printf("   -> Execucao %2d: Salario > %-7.2f | Tempo: %f s\n", i+1, chaves_busca_salario[i], tempo_iter);
    }
    printf("=> TEMPO MEDIO SEQ. DISCO (SALARIO): %f segundos\n", soma_tempos / QTD_TESTES);
    printf("====================================================\n");

    // ==============================================================================
    // FASE 3: Limpeza de Memória (Finalização)
    // ==============================================================================
    fclose(arquivo);
    liberarAVL(indice_id_avl);
    liberarAVL(indice_salario_avl);
    liberarTabelaHash(indice_id_hash);

    return 0;
}