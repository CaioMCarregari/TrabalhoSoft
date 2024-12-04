#ifndef FUNCOES_H
#define FUNCOES_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <sys/stat.h> 

typedef struct produto {
    int id;
    char nome[30];
    int quantidade;
    float preco;
    int fornecedor_id;
    struct produto *proximo;
} Produto;

typedef struct venda {
    Produto produto;
    char data[11];
    struct venda *proxima; 
} Venda;

typedef struct {
    int id;
    char nome[30];
    int quantidade_vendida;
} RelatorioVendas;

typedef struct fornecedor {
    int id;
    char nome[50];
    char contato[50];
    char email[50];
    struct fornecedor *proximo;
} Fornecedor;


Produto* Ler_Arquivo();
void Salvar_Arquivo(Produto *inicio);
void Mostrar_um_Produto(Produto *atual);
void Exibir_Produtos(Produto *inicio);
void Liberar_Lista(Produto *inicio);
Produto* Buscar_ID(int id, Produto *inicial, int *encontrado);
Produto* Buscar_Nome(char Nome[30], Produto *inicial, int *encontrado);
void Buscar_Preco(float Preco, Produto *inicial);
void Adicionar_Produto(Produto **inicio, int id, const char *nome, float preco, int quantidade, int id_fornecedor, Fornecedor **fornetes);
void Comprar_Produto(int ID, int comprar_quantidades, Produto *inicial, int *encontrado);
void Remover_Produto(int ID, int remover_quantidades, Produto *inicial, int *encontrado);
Venda* historico(int quantidade, Produto *atual, Venda *recibos, const char *data_atual);
void obterDataAtual(char *buffer);
void liberar_historico_vendas(Venda *historico);
void exibir_historico_vendas(Venda *historico);
void gerarRelatorioMaisVendidos(Venda *historico);
void salvarHistoricoVendas(Venda *recibos, const char *nomeArquivo);
void salvarRelatorioMaisVendidos(Venda *recibos, const char *nomeArquivo);
Venda* lerHistoricoVendas(const char *nomeArquivo);
RelatorioVendas* lerRelatorioMaisVendidos(const char *nomeArquivo, int *quantidadeProdutos);
void Conferir_Estoque(Produto *inicio, int limite);
void Adicionar_Fornecedor(Fornecedor **inicio, int id);
void Exibir_Fornecedores(Fornecedor *inicio);
void Associar_Produto_Fornecedor(Produto *produto, Fornecedor *inicio, int fornecedor_id);
Fornecedor* Buscar_Fornecedor(int id, Fornecedor *inicio);
Fornecedor* Ler_Arquivo_Fornecedores();
void Salvar_Fornecedores(Fornecedor *inicio);
void Menus(int qual_menu);

#endif