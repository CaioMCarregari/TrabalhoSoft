#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <sys/stat.h> 
#include "funcoes.h"

int main() {
    int menus = 0, opcao = 0, encontrado = 0, quantidadeProdutos = 0;

    Produto *produtos = NULL; 
    Produto *aux = NULL;
    produtos = Ler_Arquivo();

    Venda *recibos = NULL;

    Fornecedor *fornecedores = NULL;
    fornecedores = Ler_Arquivo_Fornecedores();

    recibos = lerHistoricoVendas("historico_vendas.csv");
    RelatorioVendas *relatorio = lerRelatorioMaisVendidos("relatorio_mais_vendidos.csv", &quantidadeProdutos);

    do{
        printf("=================================================\n");
        printf("Para acessar como cliente digite 1. \nPara acessar como administrador digite 2. \nPara sair do programa digite 3: ");
        printf("\n=================================================\n");

        while(scanf("%i", &menus) != 1 || (menus != 1 && menus != 2 && menus != 3)) {
            printf("Digite uma opção válida!\n");
            while (getchar() != '\n');
        }

        if(menus == 1) {
            do {
                Menus(menus);
                scanf("%i", &opcao);
                switch(opcao) {
                    case 1: {
                        char busca[100];
                        int encontrado = 0;

                        printf("Digite o critério de busca (nome, ID ou preço): ");
                        scanf(" %[^\n]s", busca);
                        busca[strcspn(busca, "\n")] = 0;

                        if(isdigit(busca[0])) {
                            if (strchr(busca, '.') == NULL) {
                                int idBusca = atoi(busca);
                                aux = Buscar_ID(idBusca, produtos, &encontrado);
                                 if(encontrado)
                                    Mostrar_um_Produto(aux);
                                else
                                    printf("Nenhum produto com ID %i foi encontrado!\n", idBusca);
                            } 
                            else {
                                float precoBusca = atof(busca);
                                Buscar_Preco(precoBusca, produtos);
                            }
                        } 
                        else {
                            aux = Buscar_Nome(busca, produtos, &encontrado);

                            if(encontrado)
                                Mostrar_um_Produto(aux);
                            else
                                printf("Nenhum produto com Nome %s foi encontrado!\n", busca);
                        }
                        break;
                    }

                    case 2: {
                        int qual_comprar = 0, quantidades_quer_comprar = 0;
                        char data_atual[11];

                        printf("Digite o ID do produto que quer comprar: ");
                        scanf("%d", &qual_comprar);

                        aux = Buscar_ID(qual_comprar, produtos, &encontrado);
                        if(encontrado) {
                            printf("Digite a quantidade que quer comprar: ");
                            scanf("%d", &quantidades_quer_comprar);

                            Comprar_Produto(qual_comprar, quantidades_quer_comprar, aux, &encontrado);
                            obterDataAtual(data_atual);
                            recibos = historico(quantidades_quer_comprar, aux, recibos, data_atual);
                            salvarHistoricoVendas(recibos, "historico_vendas.csv");
                            salvarRelatorioMaisVendidos(recibos, "relatorio_mais_vendidos.csv");

                        } 
                        else
                            printf("Produto com ID %d não encontrado.\n", qual_comprar);

                        break;
                    }

                    case 3: 
                        printf("Sessão cliente encerrada!\n\n");
                        break;

                    default:
                        printf("Digite uma opção válida!\n");
                        break;
                }
            } while(opcao != 3);
        }

        else if(menus == 2){
            do {
                Menus(menus);
                scanf("%i", &opcao);
                switch(opcao) {
                    case 1: {
                        char busca[100];
                        int encontrado = 0;

                        printf("Digite o critério de busca (nome, ID ou preço): ");
                        scanf(" %[^\n]s", busca);
                        busca[strcspn(busca, "\n")] = 0;

                        if(isdigit(busca[0])) {
                            if (strchr(busca, '.') == NULL) {
                                int idBusca = atoi(busca);
                                aux = Buscar_ID(idBusca, produtos, &encontrado);
                                if(encontrado)
                                    Mostrar_um_Produto(aux);
                                else
                                    printf("Nenhum produto com ID %i foi encontrado!\n", idBusca);
                            } 
                            else {
                                float precoBusca = atof(busca);
                                Buscar_Preco(precoBusca, produtos);
                            }
                        } 
                        else {
                            aux = Buscar_Nome(busca, produtos, &encontrado);

                            if(encontrado)
                                Mostrar_um_Produto(aux);
                            else
                                printf("Nenhum produto com Nome %s foi encontrado!\n", busca);

                        }
                        break;
                    }

                    case 2: {
                        int id_adicionar = 0, quantidade_adicionar = 0, fornecedor = 0;
                        char nome_adicionar[30];
                        float preco_adicionar = 0;
                        printf("Digite o novo ID do produto que quer adicionar: ");
                        scanf("%i", &id_adicionar);

                        aux = Buscar_ID(id_adicionar, produtos, &encontrado);
                        if(encontrado) {
                            printf("Produto com mesmo ID encontrado. Digite quantas unidades quer adicionar no produto: ");
                            scanf("%i", &quantidade_adicionar);
                            strcpy(nome_adicionar, produtos->nome);
                            preco_adicionar = produtos->preco;
                            
                            Adicionar_Produto(&produtos, id_adicionar, nome_adicionar, preco_adicionar, quantidade_adicionar, fornecedor, &fornecedores);
                            Salvar_Arquivo(produtos);
                            Salvar_Fornecedores(fornecedores);
                            break;
                        }
                        
                        else {
                            printf("Digite o nome desse produto: ");
                            scanf(" %[^\n]s", nome_adicionar);

                            printf("Digite o preço desse produto: ");
                            scanf("%f", &preco_adicionar);

                            printf("Digite a quantidade disponível desse produto: ");
                            scanf("%i", &quantidade_adicionar);

                            printf("Digite qual o ID do fornecedor: ");
                            scanf("%d", &fornecedor);

                            Adicionar_Produto(&produtos, id_adicionar, nome_adicionar, preco_adicionar, quantidade_adicionar, fornecedor, &fornecedores);
                            Salvar_Arquivo(produtos);
                            Salvar_Fornecedores(fornecedores);
                            break;
                        }
                    }

                    case 3: {
                        int qual_remover = 0, quantidades_remover = 0;
                        printf("Digite o ID do produto que deseja remover: ");
                        scanf("%i", &qual_remover);
                        
                        aux = Buscar_ID(qual_remover, produtos, &encontrado);
                        if(encontrado) {
                            printf("Digite quantas unidades do produto quer remover: ");
                            scanf("%i", &quantidades_remover);
                            Remover_Produto(qual_remover, quantidades_remover, aux, &encontrado);
                            Salvar_Arquivo(produtos);
                        }

                        else {
                            printf("Produto com ID %d não encontrado.\n", qual_remover);
                            break;
                        }
                        break;
                    }

                    case 4:
                        Exibir_Produtos(produtos);
                        break;

                    case 5:
                        exibir_historico_vendas(recibos);
                        break;

                    case 6:
                        gerarRelatorioMaisVendidos(recibos);
                        break;

                    case 7:
                        int limite = 0;
                        printf("Digite o valor limite ");
                        scanf("%d", &limite);
                        Conferir_Estoque(produtos, limite);
                        break;

                    case 8:
                        int id = 0;
                        printf("Digite o ID do produto ");
                        scanf("%d", &id);
                        Associar_Produto_Fornecedor(produtos, fornecedores, id);
                        break;

                    case 9:
                        Exibir_Fornecedores(fornecedores);
                        break;
                    
                    case 10:
                        printf("Sessão como administrador encerrada!\n\n");
                        break;

                    default:
                        printf("Digite uma opção válida!\n");
                        break;
                }
            } while(opcao != 10);
        }
    } while(menus != 3);

    Salvar_Arquivo(produtos);
    Liberar_Lista(produtos);
    liberar_historico_vendas(recibos);
    Salvar_Fornecedores(fornecedores);
    free(relatorio);

    printf("Programa encerrado!\n");
    return 0;
}