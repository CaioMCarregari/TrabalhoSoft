#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <time.h>

typedef struct Produto{
    int id;
    char nome[30];
    int quantidade;
    float preco;
    int fornecedor_id;
    struct Produto *proximo;
} Produto;

typedef struct fornecedor{
    int id;
    char nome[50];
    char contato[50];
    char email[50];
    struct fornecedor *proximo;
} Fornecedor;

typedef struct venda{
    Produto produto;
    char data[11];
    struct venda *proxima; 
} Venda;

typedef struct {
    int id;
    char nome[30];
    int quantidade_vendida;
} RelatorioVendas;

Produto* Ler_Arquivo();
void Salvar_Arquivo(Produto *inicio);
Fornecedor* Ler_Arquivo_Fornecedores();
void Salvar_Fornecedores(Fornecedor *inicio);
Venda* lerHistoricoVendas(const char *nomeArquivo);
void salvarHistoricoVendas(Venda *historico, const char *nomeArquivo);
RelatorioVendas* lerRelatorioMaisVendidos(const char *nomeArquivo, int *quantidadeProdutos);
void salvarRelatorioMaisVendidos(Venda *historico, const char *nomeArquivo);
void Mostrar_um_Produto(Produto *atual);
void Exibir_Produtos(Produto *inicio);
Produto* Buscar_ID(int id, Produto *inicial, int *encontrado);
Produto* Buscar_Nome(char Nome[30], Produto *inicial, int *encontrado);
void Buscar_Preco(float Preco, Produto *inicial);
void Adicionar_Produto(Produto **inicio, int id, const char *nome, float preco, int quantidade, int id_fornecedor, Fornecedor **fornetes);
void Comprar_Produto(int ID, int comprar_quantidades, Produto *inicial, int *encontrado);
void Remover_Produto(int ID, int remover_quantidades, Produto *inicial, int *encontrado);
Venda* historico(int quantidade, Produto *atual, Venda *recibos, const char *data_atual);
void obterDataAtual(char *buffer);
void exibir_historico_vendas(Venda *historico);
void gerarRelatorioMaisVendidos(Venda *historico);
void Conferir_Estoque(Produto *inicio, int limite);
void Adicionar_Fornecedor(Fornecedor **inicio, int id);
void Exibir_Fornecedores(Fornecedor *inicio);
void Associar_Produto_Fornecedor(Produto *produto, Fornecedor *inicio, int fornecedor_id);
Fornecedor* Buscar_Fornecedor(int id, Fornecedor *inicio);

//-----------------------------------------------------------------------------------------------------------------------

void test_Ler_Arquivo_arquivo_vazio(void){
    // Crie um arquivo vazio
    FILE *arquivo = fopen("Produtos.txt", "w");
    fclose(arquivo);

    Produto *resultado = Ler_Arquivo();
    CU_ASSERT_PTR_NULL(resultado);
}

void test_Ler_Arquivo_arquivo_valido(void){
    // Crie um arquivo com dados válidos
    FILE *arquivo = fopen("Produtos.txt", "w");
    fprintf(arquivo, "Produtos na lista:\n");
    fprintf(arquivo, "ID\t\tnome\t\tQuantidade\t\tPreço\t\tFornecedor\n");
    fprintf(arquivo, "------------------------------------------------------------\n");
    fprintf(arquivo, "1 Produto1 10 5.99 100\n");
    fprintf(arquivo, "2 Produto2 20 15.99 200\n");
    fclose(arquivo);

    Produto *resultado = Ler_Arquivo();
    CU_ASSERT_PTR_NOT_NULL(resultado);
    CU_ASSERT_EQUAL(resultado->id, 1);
    CU_ASSERT_STRING_EQUAL(resultado->nome, "Produto1");
    CU_ASSERT_EQUAL(resultado->quantidade, 10);
    CU_ASSERT_DOUBLE_EQUAL(resultado->preco, 5.99, 0.01);
    CU_ASSERT_EQUAL(resultado->fornecedor_id, 100);

    Produto *proximo = resultado->proximo;
    CU_ASSERT_PTR_NOT_NULL(proximo);
    CU_ASSERT_EQUAL(proximo->id, 2);
    CU_ASSERT_STRING_EQUAL(proximo->nome, "Produto2");
    CU_ASSERT_EQUAL(proximo->quantidade, 20);
    CU_ASSERT_DOUBLE_EQUAL(proximo->preco, 15.99, 0.01);
    CU_ASSERT_EQUAL(proximo->fornecedor_id, 200);

    // Libere a memória alocada
    free(proximo);
    free(resultado);
}

//-----------------------------------------------------------------------------------------------------------------------
void test_Salvar_Arquivo_arquivo_vazio(void){
    // Cria uma lista de produtos vazia
    Produto *inicio = NULL;

    // Chama a função para salvar no arquivo
    Salvar_Arquivo(inicio);

    // Verifica se o arquivo foi criado e está vazio
    FILE *arquivo = fopen("Produtos.txt", "r");
    CU_ASSERT_PTR_NOT_NULL(arquivo);

    char linha[100];
    CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof(linha), arquivo)); // Verifica se a primeira linha existe
    CU_ASSERT_STRING_EQUAL(linha, "Produtos na lista:\n");

    fclose(arquivo);
}

void test_Salvar_Arquivo_arquivo_com_produtos(void){
    // Cria uma lista de produtos
    Produto p1 = {1, "Produto1", 10, 5.99, 100, NULL};
    Produto p2 = {2, "Produto2", 20, 15.99, 200, NULL};
    p1.proximo = &p2;

    // Chama a função para salvar no arquivo
    Salvar_Arquivo(&p1);

    // Verifica se o arquivo foi criado e contém os dados corretos
    FILE *arquivo = fopen("Produtos.txt", "r");
    CU_ASSERT_PTR_NOT_NULL(arquivo);

    // Verifica se o arquivo foi escrito de forma correta (com os produtos)
    char linha[100];
    CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof(linha), arquivo));
    CU_ASSERT_STRING_EQUAL(linha, "Produtos na lista:\n");
    CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof(linha), arquivo));
    CU_ASSERT_STRING_EQUAL(linha, "ID\t\tNome\t\tQuantidade\t\tPreço\t\tFornecedor\n");
    CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof(linha), arquivo));
    CU_ASSERT_STRING_EQUAL(linha, "------------------------------------------------------------\n");
    CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof(linha), arquivo));
    CU_ASSERT_STRING_EQUAL(linha, "1         Produto1            10               5.99      100       \n");
    CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof(linha), arquivo));
    CU_ASSERT_STRING_EQUAL(linha, "2         Produto2            20               15.99     200       \n");

    fclose(arquivo);
}
//-----------------------------------------------------------------------------------------------------------------------

void test_Ler_Arquivo_Vazio_Fornecedores(void){
    // Crie um arquivo vazio
    FILE *arquivo = fopen("Fornecedores.txt", "w");
    fclose(arquivo);

    Fornecedor *resultado = Ler_Arquivo_Fornecedores();
    CU_ASSERT_PTR_NULL(resultado);
}

void test_Ler_Arquivo_Valido_Fornecedores(void){
    // Crie um arquivo com dados válidos
    FILE *arquivo = fopen("Fornecedores.txt", "w");
    fprintf(arquivo, "Fornecedores na lista:\n");
    fprintf(arquivo, "ID\t\t\tNome\t\t\t\tContato\t\t\t\tEmail\n");
    fprintf(arquivo, "------------------------------------------------------------\n");
    fprintf(arquivo, "1 Fornecedor1 11234567 Fornecedor1@gmail.com\n");
    fprintf(arquivo, "2 Fornecedor2 11325476 Fornecedor2@gmail.com\n");
    fclose(arquivo);

    Fornecedor *resultado = Ler_Arquivo_Fornecedores();
    CU_ASSERT_PTR_NOT_NULL(resultado);
    CU_ASSERT_EQUAL(resultado->id, 1);
    CU_ASSERT_STRING_EQUAL(resultado->nome, "Fornecedor1");
    CU_ASSERT_STRING_EQUAL(resultado->contato, "11234567");
    CU_ASSERT_STRING_EQUAL(resultado->email, "Fornecedor1@gmail.com");

    Fornecedor *proximo = resultado->proximo;
    CU_ASSERT_PTR_NOT_NULL(proximo);
    CU_ASSERT_EQUAL(proximo->id, 2);
    CU_ASSERT_STRING_EQUAL(proximo->nome, "Fornecedor2");
    CU_ASSERT_STRING_EQUAL(proximo->contato, "11325476");
    CU_ASSERT_STRING_EQUAL(proximo->email, "Fornecedor2@gmail.com");

    // Libere a memória alocada
    free(proximo);
    free(resultado);
}
//-----------------------------------------------------------------------------------------------------------------------

void test_Salvar_Arquivo_Fornecedor(void){
    // Cria uma lista de produtos vazia
    Fornecedor *inicio = NULL;

    // Chama a função para salvar no arquivo
    Salvar_Fornecedores(inicio);

    // Verifica se o arquivo foi criado e está vazio
    FILE *arquivo = fopen("Fornecedores.txt", "r");
    CU_ASSERT_PTR_NOT_NULL(arquivo);

    char linha[100];
    CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof(linha), arquivo));
    CU_ASSERT_STRING_EQUAL(linha, "Fornecedores na lista:\n");

    fclose(arquivo);
}

void test_Salvar_Arquivo_com_Fornecedores(void){
    // Cria uma lista de produtos
    Fornecedor f1 = {1, "Fornecedor1", "11234567", "Fornecedor1@gmail.com", NULL};
    Fornecedor f2 = {2, "Fornecedor2", "11325476", "Fornecedor2@gmail.com", NULL};
    f1.proximo = &f2;

    // Chama a função para salvar no arquivo
    Salvar_Fornecedores(&f1);

    // Verifica se o arquivo foi criado e contém os dados corretos
    FILE *arquivo = fopen("Fornecedores.txt", "r");
    CU_ASSERT_PTR_NOT_NULL(arquivo);

    // Verifica se o arquivo foi escrito de forma correta (com os fornecedores)
    char linha[100];
    CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof(linha), arquivo)); 
    CU_ASSERT_STRING_EQUAL(linha, "Fornecedores na lista:\n");
    CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof(linha), arquivo)); 
    CU_ASSERT_STRING_EQUAL(linha, "ID\t\t\tNome\t\t\t\tContato\t\t\t\tEmail\n");
    CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof(linha), arquivo)); 
    CU_ASSERT_STRING_EQUAL(linha, "------------------------------------------------------------\n");
    CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof(linha), arquivo)); 
    CU_ASSERT_STRING_EQUAL(linha, "1         Fornecedor1         11234567         Fornecedor1@gmail.com\n");
    CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof(linha), arquivo)); 
    CU_ASSERT_STRING_EQUAL(linha, "2         Fornecedor2         11325476         Fornecedor2@gmail.com\n");

    fclose(arquivo);
}

//-----------------------------------------------------------------------------------------------------------------------

void test_salvarHistoricoVendas_historico_valido(void) {
    // Cria um histórico de vendas
    Venda venda1 = {{1, "Produto1", 10, 5.99}, "2024-11-22", NULL};
    Venda venda2 = {{2, "Produto2", 20, 15.99}, "2024-11-23", NULL};
    venda1.proxima = &venda2;

    // Chama a função para salvar no arquivo
    salvarHistoricoVendas(&venda1, "historico_vendas.txt");

    // Verifica se o arquivo foi criado e contém os dados corretos
    FILE *arquivo = fopen("historico_vendas.txt", "r");
    CU_ASSERT_PTR_NOT_NULL(arquivo);

    // Verifica se o arquivo foi escrito de forma correta (com as vendas)
    char linha[100];
    CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof(linha), arquivo));
    CU_ASSERT_STRING_EQUAL(linha, "ID Produto, Nome Produto, Quantidade Vendida, Preço Unitário, Data da Venda\n");
    CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof(linha), arquivo));
    CU_ASSERT_STRING_EQUAL(linha, "1, Produto1, 10, 5.99, 2024-11-22\n");
    CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof(linha), arquivo));
    CU_ASSERT_STRING_EQUAL(linha, "2, Produto2, 20, 15.99, 2024-11-23\n");

    fclose(arquivo);
}


void test_lerHistoricoVendas_arquivo_vazio(void) {
    // Cria um arquivo vazio
    FILE *arquivo = fopen("arquivo_vazio.txt", "w");
    fclose(arquivo);

    Venda *historico = lerHistoricoVendas("arquivo_vazio.txt");
    CU_ASSERT_PTR_NULL(historico);
}

void test_lerHistoricoVendas_arquivo_valido(void) {
    // Cria um arquivo com dados válidos
    FILE *arquivo = fopen("historico_vendas.txt", "w");
    fprintf(arquivo, "ID Produto, Nome Produto, Quantidade Vendida, Preço Unitário, Data da Venda\n");
    fprintf(arquivo, "1, Produto1, 10, 5.99, 2024-11-22\n");
    fprintf(arquivo, "2, Produto2, 20, 15.99, 2024-11-23\n");
    fclose(arquivo);

    Venda *historico = lerHistoricoVendas("historico_vendas.txt");
    CU_ASSERT_PTR_NOT_NULL(historico);

    CU_ASSERT_EQUAL(historico->produto.id, 1);
    CU_ASSERT_STRING_EQUAL(historico->produto.nome, "Produto1");
    CU_ASSERT_EQUAL(historico->produto.quantidade, 10);
    CU_ASSERT_DOUBLE_EQUAL(historico->produto.preco, 5.99, 0.01);
    CU_ASSERT_STRING_EQUAL(historico->data, "2024-11-22");

    Venda *proxima = historico->proxima;
    CU_ASSERT_PTR_NOT_NULL(proxima);
    CU_ASSERT_EQUAL(proxima->produto.id, 2);
    CU_ASSERT_STRING_EQUAL(proxima->produto.nome, "Produto2");
    CU_ASSERT_EQUAL(proxima->produto.quantidade, 20);
    CU_ASSERT_DOUBLE_EQUAL(proxima->produto.preco, 15.99, 0.01);
    CU_ASSERT_STRING_EQUAL(proxima->data, "2024-11-23");

    // Libere a memória alocada
    free(proxima);
    free(historico);
}

//-----------------------------------------------------------------------------------------------------------------------

void test_salvarRelatorioMaisVendidos_historico_nulo(void) {
    // Removendo o arquivo antes de executar o teste para garantir que não existe
    remove("relatorio_vendas.txt");

    salvarRelatorioMaisVendidos(NULL, "relatorio_vendas.txt");

    FILE *arquivo = fopen("relatorio_vendas.txt", "r");
    CU_ASSERT_PTR_NULL(arquivo);
}

void test_salvarRelatorioMaisVendidos_historico_valido(void) {
    // Removendo o arquivo antes de executar o teste para garantir que não existe
    remove("relatorio_vendas.txt");

    // Cria um histórico de vendas
    Venda venda1 = {{1, "Produto1", 10, 5.99}, "2024-11-22", NULL};
    Venda venda2 = {{2, "Produto2", 20, 15.99}, "2024-11-23", NULL};
    venda1.proxima = &venda2;

    // Chama a função para salvar no arquivo
    salvarRelatorioMaisVendidos(&venda1, "relatorio_vendas.txt");

    // Verifica se o arquivo foi criado e contém os dados corretos
    FILE *arquivo = fopen("relatorio_vendas.txt", "r");
    CU_ASSERT_PTR_NOT_NULL(arquivo);

    // Verifica se o arquivo foi escrito de forma correta (no histórico de vendas)
    char linha[100];
    CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof(linha), arquivo));
    CU_ASSERT_STRING_EQUAL(linha, "ID Produto, Nome Produto, Quantidade Vendida\n");
    CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof(linha), arquivo));
    CU_ASSERT_STRING_EQUAL(linha, "2, Produto2, 20\n");
    CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof(linha), arquivo)); 
    CU_ASSERT_STRING_EQUAL(linha, "1, Produto1, 10\n");

    fclose(arquivo);
}


void test_lerRelatorioMaisVendidos_arquivo_vazio(void) {
    FILE *arquivo = fopen("arquivo_vazio.txt", "w");
    fclose(arquivo);

    int quantidadeProdutos = 0;
    RelatorioVendas *relatorio = lerRelatorioMaisVendidos("arquivo_vazio.txt", &quantidadeProdutos);
    CU_ASSERT_PTR_NULL(relatorio);
    CU_ASSERT_EQUAL(quantidadeProdutos, 0);
}

void test_lerRelatorioMaisVendidos_arquivo_valido(void) {
    FILE *arquivo = fopen("relatorio_vendas.txt", "w");
    fprintf(arquivo, "ID Produto, Nome do Produto, Quantidade Vendida\n");
    fprintf(arquivo, "1, Produto1, 10\n");
    fprintf(arquivo, "2, Produto2, 20\n");
    fclose(arquivo);

    int quantidadeProdutos = 0;
    RelatorioVendas *relatorio = lerRelatorioMaisVendidos("relatorio_vendas.txt", &quantidadeProdutos);
    CU_ASSERT_PTR_NOT_NULL(relatorio);
    CU_ASSERT_EQUAL(quantidadeProdutos, 2);

    CU_ASSERT_EQUAL(relatorio[0].id, 1);
    CU_ASSERT_STRING_EQUAL(relatorio[0].nome, "Produto1");
    CU_ASSERT_EQUAL(relatorio[0].quantidade_vendida, 10);

    CU_ASSERT_EQUAL(relatorio[1].id, 2);
    CU_ASSERT_STRING_EQUAL(relatorio[1].nome, "Produto2");
    CU_ASSERT_EQUAL(relatorio[1].quantidade_vendida, 20);

    free(relatorio);
}

//-----------------------------------------------------------------------------------------------------------------------

void test_Mostrar_Um_Produto_nulo(void) {
    // Redireciona a saída padrão para um buffer
    FILE *fp = freopen("test_output.txt", "w", stdout);
    Mostrar_um_Produto(NULL);
    freopen("/dev/tty", "a", stdout); // Restaura a saída padrão

    // Verifica o conteúdo do buffer
    fp = fopen("test_output.txt", "r");
    char buffer[256];
    fgets(buffer, sizeof(buffer), fp);
    CU_ASSERT_STRING_EQUAL(buffer, "Produto inválido.\n");
    fclose(fp);
}

void test_Mostrar_Um_Produto_valido(void) {
    Produto p = {1, "Produto1", 10, 5.99};

    // Redireciona a saída padrão para um arquivo
    FILE *fp = freopen("test_output.txt", "w", stdout);
    Mostrar_um_Produto(&p);
    freopen("/dev/tty", "a", stdout); // Restaura a saída padrão para o terminal ('/dev/tty') 

    // Verifica o conteúdo do arquivo
    fp = fopen("test_output.txt", "r");
    char buffer[256];
    fgets(buffer, sizeof(buffer), fp);
    CU_ASSERT_STRING_EQUAL(buffer, "1              Produto1            10                   5.99\n");
    fclose(fp);
}


//-----------------------------------------------------------------------------------------------------------------------

void test_Exibir_Produtos_lista_vazia() {
    // Lista de produtos vazia
    Produto *lista_vazia = NULL;

    // Redirecionando a saída para um buffer
    char buffer[200];
    memset(buffer, 0, sizeof(buffer));  //inicializa o buffer com zeros, garantindo que ele esteja limpo antes de ser usado

   // Redireciona a saída padrão para um arquivo
    FILE *fp = freopen("/dev/null", "w", stdout); // Ignora stdout original
    setbuf(fp, buffer); // Direciona a saída para o buffer

    // Chamando a função a ser testada
    Exibir_Produtos(lista_vazia);

    // Restaura a saída padrão para o terminal ('/dev/tty') 
    freopen("/dev/tty", "w", stdout); 

    // Esperando que a saída seja a mensagem "Nenhum produto cadastrado."
    char esperado[50];
    sprintf(esperado, "Nenhum produto cadastrado.\n");

    // Verifica se o buffer salvo é igual a string esperada
    CU_ASSERT_STRING_EQUAL(buffer, esperado);
}


void test_Exibir_Produtos_lista_multipla() {
    // Criando produtos para a lista
    Produto p1 = {1, "Produto1", 10, 5.99, 100, NULL};
    Produto p2 = {2, "Produto2", 20, 15.99, 200, NULL};

    // Ligando produtos
    p1.proximo = &p2;

    // Buffer para capturar a saída
    char buffer[500];
    memset(buffer, 0, sizeof(buffer));

   // Redireciona a saída padrão para um arquivo
    FILE *fp = freopen("test_output.txt", "w", stdout);
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de saída\n");
        return;
    }

    // Chamando a função a ser testada
    Exibir_Produtos(&p1);

    // Fechando o arquivo e restaura a saída padrão para o terminal ('/dev/tty') e Redireciona a saída padrão para um arquivo
    fclose(fp);
    freopen("/dev/tty", "w", stdout);

    // Lendo a saída do arquivo
    fp = fopen("test_output.txt", "r");
    fread(buffer, sizeof(char), 500, fp);
    fclose(fp);

    // Esperando que a string seja impressa assim
    char esperado[500];
    sprintf(esperado, "\nProdutos na lista:\nID\t\tNome\t\tQuantidade\t\tPreço\n------------------------------------------------------------\n%-15d%-20s%-15d%10.2f\n%-15d%-20s%-15d%10.2f\n", 
            1, "Produto1", 10, 5.99, 2, "Produto2", 20, 15.99);

    // Verifica se o buffer salvo é igual a string esperada
    CU_ASSERT_STRING_EQUAL(buffer, esperado);
}

//-----------------------------------------------------------------------------------------------------------------------

void test_Buscar_ID_produto_encontrado() {
    // Criando produtos para a lista
     Produto p1 = {1, "Produto1", 10, 5.99, 100, NULL};
    Produto p2 = {2, "Produto2", 20, 15.99, 200, NULL};
    p1.proximo = &p2;
    
    // Inicializando variável encontrada
    int encontrado = 0;

    // Chamando a função com o ID de um produto existente
    Produto *resultado = Buscar_ID(2, &p1, &encontrado);

    // Verificando se o produto foi encontrado e se o produto retornado é o correto
    CU_ASSERT_PTR_NOT_NULL(resultado);
    CU_ASSERT_EQUAL(resultado->id, 2);
    CU_ASSERT_STRING_EQUAL(resultado->nome, "Produto2");
    CU_ASSERT_EQUAL(encontrado, 1);
}


void test_Buscar_ID_produto_nao_encontrado() {
    // Criando produtos para a lista
    Produto p1 = {1, "Produto1", 10, 5.99, 100, NULL};
    Produto p2 = {2, "Produto2", 20, 15.99, 200, NULL};
    p1.proximo = &p2;
    
    // Inicializando variável encontrada
    int encontrado = 0;

    // Chamando a função com o ID de um produto inexistente
    Produto *resultado = Buscar_ID(3, &p1, &encontrado);

    // Verificando se o produto não foi encontrado
    CU_ASSERT_PTR_NULL(resultado);
    CU_ASSERT_EQUAL(encontrado, 0);
}

//-----------------------------------------------------------------------------------------------------------------------

void test_Buscar_Nome_produto_encontrado() {
    // Criando produtos para a lista
    Produto p1 = {1, "Produto1", 10, 5.99, 100, NULL};
    Produto p2 = {2, "Produto2", 20, 15.99, 200, NULL};
    p1.proximo = &p2;
    
    // Inicializando variável encontrado
    int encontrado = 0;

    char nome[30];
    strcpy(nome, "Produto2");
    // Chamando a função com o nome de um produto existente
    Produto *resultado = Buscar_Nome(nome, &p1, &encontrado);

    // Verificando se o produto foi encontrado e se o produto retornado é o correto
    CU_ASSERT_PTR_NOT_NULL(resultado);
    CU_ASSERT_STRING_EQUAL(resultado->nome, "Produto2");
    CU_ASSERT_EQUAL(encontrado, 1);
}


void test_Buscar_Nome_produto_nao_encontrado() {
    // Criando produtos para a lista
    Produto p1 = {1, "Produto1", 10, 5.99, 100, NULL};
    Produto p2 = {2, "Produto2", 20, 15.99, 200, NULL};
    p1.proximo = &p2;
    
    // Inicializando variável encontrado
    int encontrado = 0;
    char nome[30];
    strcpy(nome, "Produto3");

    // Chamando a função com o nome de um produto inexistente
    Produto *resultado = Buscar_Nome(nome, &p1, &encontrado);

    // Verificando se o produto não foi encontrado
    CU_ASSERT_PTR_NULL(resultado);
    CU_ASSERT_EQUAL(encontrado, 0);
}

//-----------------------------------------------------------------------------------------------------------------------

void test_Buscar_Preco_produto_encontrado() {
    // Criando produtos para a lista
    Produto p1 = {1, "Produto1", 10, 5.99, 100, NULL};
    Produto p2 = {2, "Produto2", 20, 15.99, 200, NULL};
    Produto p3 = {3, "Produto3", 8, 5.99, 300, NULL};
    p1.proximo = &p2;
    p2.proximo = &p3;

    // Buffer para capturar a saída
    char buffer[500];
    memset(buffer, 0, sizeof(buffer));

    // Redireciona a saída padrão para um arquivo
    FILE *fp = freopen("test_output.txt", "w", stdout);
    if(fp == NULL) {
        printf("Erro ao abrir o arquivo de saída\n");
        return;
    }

    // Chamando a função com o preço de um produto existente
    Buscar_Preco(5.99, &p1);

    // Fechando o arquivo e restaura a saída padrão para o terminal ('/dev/tty')
    fclose(fp);
    freopen("/dev/tty", "w", stdout);

    // Lendo a saída do arquivo
    fp = fopen("test_output.txt", "r");
    fread(buffer, sizeof(char), 500, fp);
    fclose(fp);

    // Esperando que a string seja impressa assim
    char esperado[500];
    sprintf(esperado, "%-15d%-20s%-15d%10.2f\n%-15d%-20s%-15d%10.2f\n", 
            1, "Produto1", 10, 5.99, 3, "Produto3", 8, 5.99);

    // Verifica se o buffer salvo é igual a string esperada
    CU_ASSERT_STRING_EQUAL(buffer, esperado);
}


void test_Buscar_Preco_produto_nao_encontrado() {
    // Criando produtos para a lista
    Produto p1 = {1, "Produto1", 10, 5.99, 100, NULL};
    Produto p2 = {2, "Produto2", 20, 15.99, 200, NULL};
    p1.proximo = &p2;

    // Buffer para capturar a saída
    char buffer[200];
    memset(buffer, 0, sizeof(buffer));

    // Redireciona a saída padrão para um arquivo
    FILE *fp = freopen("test_output.txt", "w", stdout);
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de saída\n");
        return;
    }

    // Chamando a função com um preço que não existe nos produtos
    Buscar_Preco(30.00, &p1);

    // Fechando o arquivo e restaura a saída padrão para o terminal ('/dev/tty')
    fclose(fp);
    freopen("/dev/tty", "w", stdout);

    // Lendo a saída do arquivo
    fp = fopen("test_output.txt", "r");
    fread(buffer, sizeof(char), 200, fp);
    fclose(fp);

    // Esperando a mensagem de produto não encontrado
    char esperado[200];
    sprintf(esperado, "Nenhum produto com preço %.2f foi encontrado!\n", 30.00);

    // Verifica se o buffer salvo é igual a string esperada
    CU_ASSERT_STRING_EQUAL(buffer, esperado);
}

//-----------------------------------------------------------------------------------------------------------------------


void test_Adicionar_Produto_atualizar_existente() {
    Produto p1 = {1, "Produto1", 10, 5.99, 100, NULL};
    Produto *inicio = &p1;
    Fornecedor *fornetes = NULL;

    // Chamando a função para atualizar a quantidade do produto existente
    Adicionar_Produto(&inicio, 1, "Produto1", 5, 5.99, 100, &fornetes);

    // Verificando se o produto foi atualizado corretamente
    CU_ASSERT_PTR_NOT_NULL(inicio);
    CU_ASSERT_EQUAL(inicio->id, 1);
    CU_ASSERT_STRING_EQUAL(inicio->nome, "Produto1");
    CU_ASSERT_EQUAL(inicio->quantidade, 15); // Quantidade atualizada
    CU_ASSERT_DOUBLE_EQUAL(inicio->preco, 5.99, 0.01);
    CU_ASSERT_EQUAL(inicio->fornecedor_id, 100);
}

//-----------------------------------------------------------------------------------------------------------------------

void test_Comprar_Produto_sucesso() {
    // Criando um produto para a lista
    Produto p1 = {1, "Produto1", 10, 5.99, 100, NULL};
    Produto p2 = {2, "Produto2", 20, 15.99, 200, NULL};
    p1.proximo = &p2;
    int encontrado = 1;

    // Buffer para capturar a saída
    char buffer[200];
    memset(buffer, 0, sizeof(buffer));

    // Redireciona a saída padrão para um arquivo
    FILE *fp = freopen("test_output.txt", "w", stdout);
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de saída\n");
        return;
    }

    // Chamando a função para comprar 5 unidades do produto
    Comprar_Produto(1, 5, &p1, &encontrado);

    // Fechando o arquivo e restaura a saída padrão para o terminal ('/dev/tty')
    fclose(fp);
    freopen("/dev/tty", "w", stdout);

    // Lendo a saída do arquivo
    fp = fopen("test_output.txt", "r");
    fread(buffer, sizeof(char), 200, fp);
    fclose(fp);

    // Esperando a mensagem de compra bem-sucedida
    char esperado[200];
    sprintf(esperado, "Produto %s foi comprado com sucesso!\n", "Produto1");

    // Verifica se o buffer salvo é igual a string esperada e a quantidade comprada
    CU_ASSERT_STRING_EQUAL(buffer, esperado);
    CU_ASSERT_EQUAL(p1.quantidade, 5);
}


void test_Comprar_Produto_quantidade_insuficiente() {
    // Criando um produto para a lista
    Produto p1 = {1, "Produto1", 2, 5.99, 100, NULL};
    int encontrado = 1;

    // Buffer para capturar a saída
    char buffer[200];
    memset(buffer, 0, sizeof(buffer));

    // Redireciona a saída padrão para um arquivo
    FILE *fp = freopen("test_output.txt", "w", stdout);
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de saída\n");
        return;
    }

    // Chamando a função para comprar 5 unidades do produto
    Comprar_Produto(1, 5, &p1, &encontrado);

    //Fechando o arquivo e restaura a saída padrão para o terminal ('/dev/tty')
    fclose(fp);
    freopen("/dev/tty", "w", stdout);

    // Lendo a saída do arquivo
    fp = fopen("test_output.txt", "r");
    fread(buffer, sizeof(char), 200, fp);
    fclose(fp);

    // Esperando a mensagem de quantidade insuficiente
    char esperado[200];
    sprintf(esperado, "Produto %s apenas com %i unidades no momento!\n", "Produto1", 2);

    // Verifica se o buffer salvo é igual a string esperada e a quantidade do Produto1
    CU_ASSERT_STRING_EQUAL(buffer, esperado);
    CU_ASSERT_EQUAL(p1.quantidade, 2);
}

//-----------------------------------------------------------------------------------------------------------------------

void test_Remover_Produto_sucesso_parcial() {
    // Criando um produto para a lista
    Produto p1 = {1, "Produto1", 10, 5.99, 100, NULL};
    int encontrado = 1;

    // Chamando a função para remover 5 unidades do produto
    Remover_Produto(1, 5, &p1, &encontrado);

    // Verifica se a quantidade do produto removido corresponde ao que foi removido
    CU_ASSERT_EQUAL(p1.quantidade, 5);
}


void test_Remover_Produto_sucesso_total() {
    // Criando um produto para a lista
    Produto p1 = {1, "Produto1", 5, 5.99, 100, NULL};
    int encontrado = 1;

    // Chamando a função para remover 6 unidades do produto (mais do que disponível)
    Remover_Produto(1, 6, &p1, &encontrado);

    // Verifica se a quantidade do produto removido corresponde ao que foi removido
    CU_ASSERT_EQUAL(p1.quantidade, 0);
}

//-----------------------------------------------------------------------------------------------------------------------

void test_historico_lista_vazia() {
    Produto p1 = {1, "Produto1", 10, 5.99, 100, NULL};
    Venda *recibos = NULL;
    const char *data_atual = "2024-11-24";

    // Chamando a função para adicionar uma venda
    recibos = historico(5, &p1, recibos, data_atual);

    // Verificando se a venda foi adicionada corretamente
    CU_ASSERT_PTR_NOT_NULL(recibos);
    CU_ASSERT_EQUAL(recibos->produto.id, 1);
    CU_ASSERT_STRING_EQUAL(recibos->produto.nome, "Produto1");
    CU_ASSERT_EQUAL(recibos->produto.quantidade, 5);
    CU_ASSERT_DOUBLE_EQUAL(recibos->produto.preco, 29.95, 0.01);
    CU_ASSERT_STRING_EQUAL(recibos->data, "2024-11-24");
    CU_ASSERT_PTR_NULL(recibos->proxima);
}


void test_historico_lista_existente() {
    Produto p1 = {1, "Produto1", 10, 5.99, 100, NULL};
    Produto p2 = {2, "Produto2", 20, 15.99, 200, NULL};
    const char *data_atual = "2024-11-24";

    // Criando uma lista de vendas existente
    Venda venda1 = {{1, "Produto1", 2, 11.98}, "2024-11-23", NULL};
    Venda *recibos = &venda1;

    // Chamando a função para adicionar uma nova venda
    recibos = historico(3, &p2, recibos, data_atual);

    // Verificando se a nova venda foi adicionada corretamente ao final da lista
    CU_ASSERT_PTR_NOT_NULL(recibos->proxima);
    CU_ASSERT_EQUAL(recibos->proxima->produto.id, 2);
    CU_ASSERT_STRING_EQUAL(recibos->proxima->produto.nome, "Produto2");
    CU_ASSERT_EQUAL(recibos->proxima->produto.quantidade, 3);
    CU_ASSERT_DOUBLE_EQUAL(recibos->proxima->produto.preco, 47.97, 0.01);
    CU_ASSERT_STRING_EQUAL(recibos->proxima->data, "2024-11-24");
    CU_ASSERT_PTR_NULL(recibos->proxima->proxima);
}

//-----------------------------------------------------------------------------------------------------------------------

void test_obterDataAtual_formato() {
    char buffer[11];  // Buffer para armazenar a data no formato DD/MM/YYYY

    // Chamando a função para obter a data atual
    obterDataAtual(buffer);

    // Verificando se a data está no formato correto
    CU_ASSERT_EQUAL(buffer[2], '/');
    CU_ASSERT_EQUAL(buffer[5], '/');
    CU_ASSERT_EQUAL(strlen(buffer), 10);  // Verificando se o comprimento da string é 10
}


void test_obterDataAtual_valor() {
    char buffer[11];  // Buffer para armazenar a data no formato DD/MM/YYYY
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char data_esperada[11];
    sprintf(data_esperada, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

    // Chamando a função para obter a data atual
    obterDataAtual(buffer);

    // Verifica se o buffer é igual a string data_esperada
    CU_ASSERT_STRING_EQUAL(buffer, data_esperada);
}

//-----------------------------------------------------------------------------------------------------------------------

void test_exibir_historico_vendas_vazio() {
    // Histórico de vendas vazio
    Venda *historico = NULL;

    // Buffer para capturar a saída
    char buffer[200];
    memset(buffer, 0, sizeof(buffer));

    // Redireciona a saída padrão para um arquivo
    FILE *fp = freopen("test_output.txt", "w", stdout);
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de saída\n");
        return;
    }

    // Chamando a função para exibir o histórico de vendas
    exibir_historico_vendas(historico);

    // Fechando o arquivo e restaura a saída padrão para o terminal ('/dev/tty')
    fclose(fp);
    freopen("/dev/tty", "w", stdout);

    // Lendo a saída do arquivo
    fp = fopen("test_output.txt", "r");
    fread(buffer, sizeof(char), 200, fp);
    fclose(fp);

    // Esperando a mensagem de histórico vazio
    char esperado[200];
    sprintf(esperado, "Nenhuma venda registrada até o momento.\n");

    // Verifica se o buffer salvo é igual a string esperada
    CU_ASSERT_STRING_EQUAL(buffer, esperado);
}



void test_exibir_historico_vendas_com_vendas() {
    // Criando vendas para o histórico
    Venda venda1 = {{1, "Produto1", 2, 11.98}, "2024-11-23", NULL};
    Venda venda2 = {{2, "Produto2", 3, 47.97}, "2024-11-24", NULL};
    venda1.proxima = &venda2;

    // Buffer para capturar a saída
    char buffer[500];
    memset(buffer, 0, sizeof(buffer));

    // Redireciona a saída padrão para um arquivo
    FILE *fp = freopen("test_output.txt", "w", stdout);
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de saída\n");
        return;
    }

    // Chamando a função para exibir o histórico de vendas
    exibir_historico_vendas(&venda1);

    // Fechando o arquivo e restaura a saída padrão para o terminal ('/dev/tty')
    fclose(fp);
    freopen("/dev/tty", "w", stdout);

    // Lendo a saída do arquivo
    fp = fopen("test_output.txt", "r");
    fread(buffer, sizeof(char), 500, fp);
    fclose(fp);

    // Esperando o histórico completo
    char esperado[500];
    sprintf(esperado, "\n=== Histórico de Vendas ===\nID\tNome\t\tQuantidade\tTotal\t\tData\n-----------------------------------------------------------\n%-10d%-20s%-15d%-10.2f%-12s\n%-10d%-20s%-15d%-10.2f%-12s\n",
            1, "Produto1", 2, 11.98, "2024-11-23", 
            2, "Produto2", 3, 47.97, "2024-11-24");

    // Verifica se o buffer salvo é igual a string esperada
    CU_ASSERT_STRING_EQUAL(buffer, esperado);
}


//-----------------------------------------------------------------------------------------------------------------------

void test_gerarRelatorioMaisVendidos_vazio() {
    // Histórico de vendas vazio
    Venda *historico = NULL;

    // Buffer para capturar a saída
    char buffer[200];
    memset(buffer, 0, sizeof(buffer));

    // Redireciona a saída padrão para um arquivo
    FILE *fp = freopen("test_output.txt", "w", stdout);
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de saída\n");
        return;
    }

    // Chamando a função para gerar o relatório
    gerarRelatorioMaisVendidos(historico);

    // Fechando o arquivo e restaura a saída padrão para o terminal ('/dev/tty')
    fclose(fp);
    freopen("/dev/tty", "w", stdout);

    // Lendo a saída do arquivo
    fp = fopen("test_output.txt", "r");
    fread(buffer, sizeof(char), 200, fp);
    fclose(fp);

    // Esperando a mensagem de histórico vazio
    char esperado[200];
    sprintf(esperado, "Nenhuma venda registrada até o momento.\n");

    // Verifica se o buffer salvo é igual a string esperada
    CU_ASSERT_STRING_EQUAL(buffer, esperado);
}


void test_gerarRelatorioMaisVendidos_com_vendas() {
    // Criando vendas para o histórico
    Venda venda1 = {{1, "Produto1", 2, 11.98}, "2024-11-23", NULL};
    Venda venda2 = {{2, "Produto2", 3, 47.97}, "2024-11-24", NULL};
    Venda venda3 = {{1, "Produto1", 1, 5.99}, "2024-11-25", NULL};
    venda1.proxima = &venda2;
    venda2.proxima = &venda3;

    // Buffer para capturar a saída
    char buffer[500];
    memset(buffer, 0, sizeof(buffer));

    // Redireciona a saída padrão para um arquivo
    FILE *fp = freopen("test_output.txt", "w", stdout);
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de saída\n");
        return;
    }

    // Chamando a função para gerar o relatório
    gerarRelatorioMaisVendidos(&venda1);

    // Fechando o arquivo e restaura a saída padrão para o terminal ('/dev/tty')
    fclose(fp);
    freopen("/dev/tty", "w", stdout);

    // Lendo a saída do arquivo
    fp = fopen("test_output.txt", "r");
    fread(buffer, sizeof(char), 500, fp);
    fclose(fp);

    // Esperando o relatório completo
    char esperado[500];
    sprintf(esperado, "\n=== Relatório de Produtos Mais Vendidos ===\nID\tNome\t\tQuantidade Vendida\n-----------------------------------------\n%-10d%-20s%-10d\n%-10d%-20s%-10d\n",
            1, "Produto1", 3,
            2, "Produto2", 3);

    // Verifica se o buffer salvo é igual a string esperada
    CU_ASSERT_STRING_EQUAL(buffer, esperado);
}

//-----------------------------------------------------------------------------------------------------------------------

void test_Conferir_Estoque_sem_produtos() {
    // Lista de produtos vazia
    Produto *inicio = NULL;
    int limite = 10;

    // Buffer para capturar a saída
    char buffer[200];
    memset(buffer, 0, sizeof(buffer));

    // Redireciona a saída padrão para um arquivo
    FILE *fp = freopen("test_output.txt", "w", stdout);
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de saída\n");
        return;
    }

    // Chamando a função para conferir o estoque
    Conferir_Estoque(inicio, limite);

    // Fechando o arquivo e restaura a saída padrão para o terminal ('/dev/tty')
    fclose(fp);
    freopen("/dev/tty", "w", stdout);

    // Lendo a saída do arquivo
    fp = fopen("test_output.txt", "r");
    fread(buffer, sizeof(char), 200, fp);
    fclose(fp);

    // Esperando a mensagem de nenhum produto cadastrado
    char esperado[200];
    sprintf(esperado, "Nenhum produto cadastrado.\n");

    // Verifica se o buffer salvo é igual a string esperada
    CU_ASSERT_STRING_EQUAL(buffer, esperado);
}



void test_Conferir_Estoque_produtos_abaixo_do_limite() {
    // Criando produtos para a lista
    Produto p1 = {1, "Produto1", 5, 5.99, 100, NULL};
    Produto p2 = {2, "Produto2", 20, 15.99, 200, NULL};
    Produto p3 = {3, "Produto3", 8, 5.99, 300, NULL};
    p1.proximo = &p2;
    p2.proximo = &p3;

    int limite = 10;

    // Buffer para capturar a saída
    char buffer[500];
    memset(buffer, 0, sizeof(buffer));

    // Redireciona a saída padrão para um arquivo
    FILE *fp = freopen("test_output.txt", "w", stdout);
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de saída\n");
        return;
    }

    // Chamando a função para conferir o estoque
    Conferir_Estoque(&p1, limite);

    // Fechando o arquivo e restaura a saída padrão para o terminal ('/dev/tty')
    fclose(fp);
    freopen("/dev/tty", "w", stdout);

    // Lendo a saída do arquivo
    fp = fopen("test_output.txt", "r");
    fread(buffer, sizeof(char), 500, fp);
    fclose(fp);

    // Esperando a saída dos produtos abaixo do limite
    char esperado[500];
    sprintf(esperado, "\n==== Produtos com Pouco Estoque (Limite: %d) ====\n%-15d%-20s%-15d%10.2f\n%-15d%-20s%-15d%10.2f\n",
            limite, 1, "Produto1", 5, 5.99, 3, "Produto3", 8, 5.99);

    // Verifica se o buffer salvo é igual a string esperada
    CU_ASSERT_STRING_EQUAL(buffer, esperado);
}

//-----------------------------------------------------------------------------------------------------------------------

void test_Adicionar_Fornecedor_lista_vazia() {
    Fornecedor *inicio = NULL;

    // Escrevendo o input simulado em um arquivo temporário
    FILE *input_fp = fopen("input.txt", "w");
    fprintf(input_fp, "Fornecedor A\nContato A\nemail_a@exemplo.com\n");
    fclose(input_fp);

    // Redirecionando a entrada para o arquivo temporário
    freopen("input.txt", "r", stdin);

    // Redirecionando a saída para suprimir a saída
    freopen("/dev/null", "w", stdout);

    // Chamando a função para adicionar um fornecedor
    Adicionar_Fornecedor(&inicio, 1);

    // Restaura a saída padrão e entrada padrão para o terminal ('/dev/tty') 
    freopen("/dev/tty", "w", stdout);
    freopen("/dev/tty", "r", stdin);

    // Verificando se o fornecedor foi adicionado corretamente
    CU_ASSERT_PTR_NOT_NULL(inicio);
    CU_ASSERT_EQUAL(inicio->id, 1);
    CU_ASSERT_STRING_EQUAL(inicio->nome, "Fornecedor A");
    CU_ASSERT_STRING_EQUAL(inicio->contato, "Contato A");
    CU_ASSERT_STRING_EQUAL(inicio->email, "email_a@exemplo.com");
    CU_ASSERT_PTR_NULL(inicio->proximo);
}

//-----------------------------------------------------------------------------------------------------------------------

void test_Exibir_Fornecedores_vazio() {
    // Lista de fornecedores vazia
    Fornecedor *inicio = NULL;

    // Redirecionando a saída para um buffer
    char buffer[200];
    setbuf(stdout, buffer);

    // Chamando a função para exibir os fornecedores
    Exibir_Fornecedores(inicio);

    // Resetando a saída para stdout padrão
    setbuf(stdout, NULL);

    // Esperando a mensagem de nenhum fornecedor cadastrado
    char esperado[200];
    sprintf(esperado, "Nenhum fornecedor cadastrado.\n");

    // Verifica se o buffer salvo é igual a string esperada
    CU_ASSERT_STRING_EQUAL(buffer, esperado);
}


void test_Exibir_Fornecedores_com_fornecedores() {
    // Criando fornecedores para a lista
    Fornecedor fornecedor1 = {1, "Fornecedor A", "Contato A", "email_a@exemplo.com", NULL};
    Fornecedor fornecedor2 = {2, "Fornecedor B", "Contato B", "email_b@exemplo.com", NULL};
    fornecedor1.proximo = &fornecedor2;
    Fornecedor *inicio = &fornecedor1;

    // Buffer para capturar a saída
    char buffer[500];
    memset(buffer, 0, sizeof(buffer));

    // Redireciona a saída padrão para um arquivo
    FILE *fp = freopen("test_output.txt", "w", stdout);
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de saída\n");
        return;
    }

    // Chamando a função para exibir os fornecedores
    Exibir_Fornecedores(inicio);

    // Fechando o arquivo e restaura a saída padrão para o terminal ('/dev/tty')
    fclose(fp);
    freopen("/dev/tty", "w", stdout);

    // Lendo a saída do arquivo
    fp = fopen("test_output.txt", "r");
    fread(buffer, sizeof(char), 500, fp);
    fclose(fp);

    // Esperando a saída dos fornecedores cadastrados
    char esperado[500];
    sprintf(esperado, "ID: %d\nNome: %s\nContato: %s\nEmail: %s\n\nID: %d\nNome: %s\nContato: %s\nEmail: %s\n\n",
            1, "Fornecedor A", "Contato A", "email_a@exemplo.com",
            2, "Fornecedor B", "Contato B", "email_b@exemplo.com");

    // Verifica se o buffer salvo é igual a string esperada
    CU_ASSERT_STRING_EQUAL(buffer, esperado);
}


void test_Buscar_Fornecedor_encontrado() {
    // Criando fornecedores para a lista
    Fornecedor fornecedor1 = {1, "Fornecedor A", "Contato A", "email_a@exemplo.com", NULL};
    Fornecedor fornecedor2 = {2, "Fornecedor B", "Contato B", "email_b@exemplo.com", NULL};
    fornecedor1.proximo = &fornecedor2;
    Fornecedor *inicio = &fornecedor1;

    // Chamando a função para buscar um fornecedor existente
    Fornecedor *resultado = Buscar_Fornecedor(2, inicio);

    // Verificando se o fornecedor foi encontrado corretamente
    CU_ASSERT_PTR_NOT_NULL(resultado);
    CU_ASSERT_EQUAL(resultado->id, 2);
    CU_ASSERT_STRING_EQUAL(resultado->nome, "Fornecedor B");
    CU_ASSERT_STRING_EQUAL(resultado->contato, "Contato B");
    CU_ASSERT_STRING_EQUAL(resultado->email, "email_b@exemplo.com");
}

//-----------------------------------------------------------------------------------------------------------------------

void test_Associar_Produto_Fornecedor_encontrado() {
    // Criando produtos e fornecedores para a lista
    Produto p1 = {1, "Produto1", 10, 5.99, 100, NULL};
    Produto p2 = {2, "Produto2", 20, 15.99, 200, NULL};
    p1.proximo = &p2;
    Produto *produtos = &p1;

    Fornecedor fornecedor1 = {100, "Fornecedor A", "Contato A", "email_a@exemplo.com", NULL};
    Fornecedor fornecedor2 = {200, "Fornecedor B", "Contato B", "email_b@exemplo.com", NULL};
    fornecedor1.proximo = &fornecedor2;
    Fornecedor *fornecedores = &fornecedor1;

    // Buffer para capturar a saída
    char buffer[200];
    memset(buffer, 0, sizeof(buffer));

    // Redireciona a saída padrão para um arquivo
    FILE *fp = freopen("test_output.txt", "w", stdout);
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de saída\n");
        return;
    }

    // Chamando a função para associar produto e fornecedor
    Associar_Produto_Fornecedor(produtos, fornecedores, 1);

    // Fechando o arquivo e restaura a saída padrão para o terminal ('/dev/tty')
    fclose(fp);
    freopen("/dev/tty", "w", stdout);

    // Lendo a saída do arquivo
    fp = fopen("test_output.txt", "r");
    fread(buffer, sizeof(char), 200, fp);
    fclose(fp);

    // Esperando a mensagem de associação bem-sucedida
    char esperado[200];
    sprintf(esperado, "O fornecedor do produto '%s' é '%s'.\n", "Produto1", "Fornecedor A");

    // Verifica se o buffer salvo é igual a string esperada
    CU_ASSERT_STRING_EQUAL(buffer, esperado);
}


void test_Associar_Produto_Fornecedor_produto_nao_encontrado() {
    // Criando produtos e fornecedores para a lista
    Produto p1 = {1, "Produto1", 10, 5.99, 100, NULL};
    Produto p2 = {2, "Produto2", 20, 15.99, 200, NULL};
    p1.proximo = &p2;
    Produto *produtos = &p1;

    Fornecedor fornecedor1 = {100, "Fornecedor A", "Contato A", "email_a@exemplo.com", NULL};
    Fornecedor fornecedor2 = {200, "Fornecedor B", "Contato B", "email_b@exemplo.com", NULL};
    fornecedor1.proximo = &fornecedor2;
    Fornecedor *fornecedores = &fornecedor1;

    // Buffer para capturar a saída
    char buffer[200];
    memset(buffer, 0, sizeof(buffer));

    // Redireciona a saída padrão para um arquivo
    FILE *fp = freopen("test_output.txt", "w", stdout);
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de saída\n");
        return;
    }

    // Chamando a função para associar produto e fornecedor com ID inexistente
    Associar_Produto_Fornecedor(produtos, fornecedores, 300);

    // Fechando o arquivo e restaura a saída padrão para o terminal ('/dev/tty')
    fclose(fp);
    freopen("/dev/tty", "w", stdout);

    // Lendo a saída do arquivo
    fp = fopen("test_output.txt", "r");
    fread(buffer, sizeof(char), 200, fp);
    fclose(fp);

    // Esperando a mensagem de erro de produto não encontrado
    char esperado[200];
    sprintf(esperado, "Erro: Produto com ID %d não encontrado.\n", 300);

    // Verifica se o buffer salvo é igual a string esperada
    CU_ASSERT_STRING_EQUAL(buffer, esperado);
}

//-----------------------------------------------------------------------------------------------------------------------

int main() {
    // Inicializa o registro de testes
    if(CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    // Suite para testar arquivos
    CU_pSuite suite_arquivos = CU_add_suite("Suite_Ler_Arquivos", 0, 0);
    if(suite_arquivos == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
        (CU_add_test(suite_arquivos, "Teste arquivo vazio (produtos)", test_Ler_Arquivo_arquivo_vazio) == NULL) ||
        (CU_add_test(suite_arquivos, "Teste arquivo válido (produtos)", test_Ler_Arquivo_arquivo_valido) == NULL) ||
        (CU_add_test(suite_arquivos, "Teste salvar arquivo vazio (produtos)", test_Salvar_Arquivo_arquivo_vazio) == NULL) ||
        (CU_add_test(suite_arquivos, "Teste salvar arquivo válido (produtos)", test_Salvar_Arquivo_arquivo_com_produtos) == NULL) ||
        (CU_add_test(suite_arquivos, "Teste arquivo vazio (fornecedores)", test_Ler_Arquivo_Vazio_Fornecedores) == NULL) ||
        (CU_add_test(suite_arquivos, "Teste arquivo válido (fornecedores)", test_Ler_Arquivo_Valido_Fornecedores) == NULL) ||
        (CU_add_test(suite_arquivos, "Teste salvar arquivo vazio (fornecedores)", test_Salvar_Arquivo_Fornecedor) == NULL) ||
        (CU_add_test(suite_arquivos, "Teste salvar arquivo válido (fornecedores)", test_Salvar_Arquivo_com_Fornecedores) == NULL) ||
        (CU_add_test(suite_arquivos, "Teste arquivo vazio (histórico)", test_lerHistoricoVendas_arquivo_vazio) == NULL) ||
        (CU_add_test(suite_arquivos, "Teste salvar arquivo válido (histórico)", test_salvarHistoricoVendas_historico_valido) == NULL) ||
        (CU_add_test(suite_arquivos, "Teste arquivo válido (histórico)", test_lerHistoricoVendas_arquivo_valido) == NULL) ||
        (CU_add_test(suite_arquivos, "Teste arquivo vazio (relatório)", test_lerRelatorioMaisVendidos_arquivo_vazio) == NULL) ||
        (CU_add_test(suite_arquivos, "Teste arquivo válido (relatório)", test_lerRelatorioMaisVendidos_arquivo_valido) == NULL) ||
        (CU_add_test(suite_arquivos, "Teste salvar arquivo nulo (relatório)", test_salvarRelatorioMaisVendidos_historico_nulo) == NULL) ||
        (CU_add_test(suite_arquivos, "Teste salvar arquivo válido (relatório)", test_salvarRelatorioMaisVendidos_historico_valido) == NULL)
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Suite para testar produtos
    CU_pSuite suite_produtos = CU_add_suite("Suite_Produtos", 0, 0);
    if(suite_produtos == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
        (CU_add_test(suite_produtos, "Teste Mostrar Um Produto Nulo", test_Mostrar_Um_Produto_nulo) == NULL) ||
        (CU_add_test(suite_produtos, "Teste Mostrar Um Produto Válido", test_Mostrar_Um_Produto_valido) == NULL) ||
        (CU_add_test(suite_produtos, "Teste Exibir Produtos Lista Vazia", test_Exibir_Produtos_lista_vazia) == NULL) ||
        (CU_add_test(suite_produtos, "Teste Exibir Produtos Lista Múltipla", test_Exibir_Produtos_lista_multipla) == NULL)
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Suite para testar métodos de busca
    CU_pSuite suite_busca = CU_add_suite("Suite_Buscar", NULL, NULL);
    if(suite_busca == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
        (CU_add_test(suite_busca, "Teste Buscar_ID Produto Encontrado", test_Buscar_ID_produto_encontrado) == NULL) ||
        (CU_add_test(suite_busca, "Teste Buscar_ID Produto Não Encontrado", test_Buscar_ID_produto_nao_encontrado) == NULL) ||
        (CU_add_test(suite_busca, "Teste Buscar_Nome com produto encontrado", test_Buscar_Nome_produto_encontrado) == NULL) ||
        (CU_add_test(suite_busca, "Teste Buscar_Nome com produto não encontrado", test_Buscar_Nome_produto_nao_encontrado) == NULL) ||
        (CU_add_test(suite_busca, "Teste Buscar_Preco com produto encontrado", test_Buscar_Preco_produto_encontrado) == NULL) ||
        (CU_add_test(suite_busca, "Teste Buscar_Preco com produto não encontrado", test_Buscar_Preco_produto_nao_encontrado) == NULL)
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Suite para operar produtos
    CU_pSuite suite_modificar_produtos = CU_add_suite("Suite_Modificar_Produtos", NULL, NULL);
    if(suite_modificar_produtos == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }


    if (
        (CU_add_test(suite_modificar_produtos, "Teste Adicionar Produto - Atualizar Existente", test_Adicionar_Produto_atualizar_existente) == NULL) ||
        (CU_add_test(suite_modificar_produtos, "Teste Comprar Produto - Sucesso", test_Comprar_Produto_sucesso) == NULL) ||
        (CU_add_test(suite_modificar_produtos, "Teste Comprar Produto - Quantidade Insuficiente", test_Comprar_Produto_quantidade_insuficiente) == NULL) ||
        (CU_add_test(suite_modificar_produtos, "Teste Remover Produto - Remoção Parcial", test_Remover_Produto_sucesso_parcial) == NULL) ||
        (CU_add_test(suite_modificar_produtos, "Teste Remover Produto - Remoção Total", test_Remover_Produto_sucesso_total) == NULL) ||
        (CU_add_test(suite_modificar_produtos, "Teste Histórico - Lista Vazia", test_historico_lista_vazia) == NULL) ||
        (CU_add_test(suite_modificar_produtos, "Teste Histórico - Lista Existente", test_historico_lista_existente) == NULL)
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Suite para verificar datas
    CU_pSuite suite_datas = CU_add_suite("Suite_Datas", 0, 0);
    if(suite_datas == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (
        (CU_add_test(suite_datas, "Teste formato da data atual", test_obterDataAtual_formato) == NULL) ||
        (CU_add_test(suite_datas, "Teste valor da data atual", test_obterDataAtual_valor) == NULL)
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Suite para verificar históricos
    CU_pSuite suite_historico = CU_add_suite("Suite_Historico", 0, 0);
    if(suite_historico == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (
        (CU_add_test(suite_historico, "Teste histórico vazio", test_exibir_historico_vendas_vazio) == NULL) ||
        (CU_add_test(suite_historico, "Teste histórico com vendas", test_exibir_historico_vendas_com_vendas) == NULL) ||
        (CU_add_test(suite_historico, "Teste relatório mais vendidos vazio", test_gerarRelatorioMaisVendidos_vazio) == NULL) ||
        (CU_add_test(suite_historico, "Teste relatório mais vendidos com vendas", test_gerarRelatorioMaisVendidos_com_vendas) == NULL)
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Suíte para controle de estoque
    CU_pSuite suite_estoque = CU_add_suite("Suite_Controle_Estoque", 0, 0);
    if(suite_estoque == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (
        (CU_add_test(suite_estoque, "Teste estoque vazio", test_Conferir_Estoque_sem_produtos) == NULL) ||
        (CU_add_test(suite_estoque, "Teste produtos abaixo do limite", test_Conferir_Estoque_produtos_abaixo_do_limite) == NULL)
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Suite para fornecedores
    CU_pSuite suite_fornecedores = CU_add_suite("Suite_Fornecedores", NULL, NULL);
    if(NULL == suite_fornecedores) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
        (NULL == CU_add_test(suite_fornecedores, "Testar Adicionar Fornecedor (Lista Vazia)", test_Adicionar_Fornecedor_lista_vazia)) ||
        (NULL == CU_add_test(suite_fornecedores, "Testar Exibir Fornecedores (Lista Vazia)", test_Exibir_Fornecedores_vazio)) ||
        (NULL == CU_add_test(suite_fornecedores, "Testar Exibir Fornecedores (Lista com Fornecedores)", test_Exibir_Fornecedores_com_fornecedores)) ||
        (NULL == CU_add_test(suite_fornecedores, "Testar Buscar Fornecedor (Encontrado)", test_Buscar_Fornecedor_encontrado)) ||
        (NULL == CU_add_test(suite_fornecedores, "Testar Associar Produto a Fornecedor (Encontrado)", test_Associar_Produto_Fornecedor_encontrado)) ||
        (NULL == CU_add_test(suite_fornecedores, "Testar Associar Produto a Fornecedor (Produto Não Encontrado)", test_Associar_Produto_Fornecedor_produto_nao_encontrado))
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Executa os testes
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    // Limpa o registro de testes
    CU_cleanup_registry();

    return CU_get_error();
}