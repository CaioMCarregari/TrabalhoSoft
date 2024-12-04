#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <sys/stat.h> 
#include "funcoes.h"

Produto* Ler_Arquivo() {
    char linha[100];
    FILE *arquivo = fopen("Produtos.txt", "r");
    
    if(!arquivo) {
        printf("Erro ao abrir o arquivo para leitura.\n\n");
        return NULL;
    }

    Produto *inicio = NULL;
    Produto *atual = NULL;

    //consome cabeçalho
    for(int i = 0; i < 3; i++)
        fgets(linha, sizeof(linha), arquivo);

    while(fgets(linha, sizeof(linha), arquivo)) {
        Produto *novo = malloc(sizeof(Produto));

        if(novo) {
            if(sscanf(linha, "%d %29s %d %f %d", &novo->id, novo->nome, &novo->quantidade, &novo->preco, &novo->fornecedor_id) != 5) {
                free(novo); // Libera memória alocada para o produto inválido
                continue;   // Pula para a próxima linha
            }

            if(inicio == NULL) {
                inicio = novo;
                atual = novo;
            } 
            else {
                atual->proximo = novo;
                atual = novo;
            }
        }
    }

    fclose(arquivo);
    return inicio;
}

Fornecedor* Ler_Arquivo_Fornecedores() {
    char linha[100];
    FILE *arquivo = fopen("Fornecedores.txt", "r");

    if(!arquivo) {
        printf("Erro ao abrir o arquivo para leitura.\n\n");
        return NULL;
    }

    Fornecedor *inicio = NULL;
    Fornecedor *atual = NULL;

    //consome cabeçalho
    for(int i = 0; i < 3; i++)
        fgets(linha, sizeof(linha), arquivo);


    while(fgets(linha, sizeof(linha), arquivo)) {
        Fornecedor *novo = malloc(sizeof(Fornecedor));

        if(novo) {
            sscanf(linha, "%d %49s %49s %49s", &novo->id, novo->nome, novo->contato, novo->email);
            novo->proximo = NULL;

            if(inicio == NULL) {
                inicio = novo;
                atual = novo;
            } 
            else {
                atual->proximo = novo;
                atual = novo;
            }
        }
    }

    fclose(arquivo);
    return inicio;
}

void Salvar_Fornecedores(Fornecedor *inicio) {
    FILE *salvar = fopen("Fornecedores.txt", "w");
    if(salvar) {
        Fornecedor *atual = inicio;
        fprintf(salvar, "Fornecedores na lista:\n");
        fprintf(salvar, "ID\t\t\tNome\t\t\t\tContato\t\t\t\tEmail\n");
        fprintf(salvar, "------------------------------------------------------------\n");

        while(atual != NULL) {
            fprintf(salvar, "%-10d%-20s%-17s%-10s\n", atual->id, atual->nome, atual->contato, atual->email);
            atual = atual->proximo;
        }
        fclose(salvar);
    }

    else
        printf("Erro ao abrir o arquivo para escrita.\n");
}



void Salvar_Arquivo(Produto *inicio){
    FILE *salvar = fopen("Produtos.txt", "w");
    if(salvar){
        Produto *atual = inicio;
        fprintf(salvar, "Produtos na lista:\n");
        fprintf(salvar, "ID\t\tNome\t\tQuantidade\t\tPreço\t\tFornecedor\n");
        fprintf(salvar, "------------------------------------------------------------\n");
    
        while(atual != NULL) {
            fprintf(salvar, "%-10d%-20s%-17d%-10.2f%-10d\n", atual->id, atual->nome, atual->quantidade, atual->preco, atual->fornecedor_id);
            atual = atual->proximo;
        }
        fclose(salvar);
    } 

    else
        printf("Erro ao abrir o arquivo para escrita.\n");
}



void Mostrar_um_Produto(Produto *atual){

    if(atual == NULL) {
        printf("Produto inválido.\n");
        return;
    }

    printf("%-15d", atual->id);            
    printf("%-20s", atual->nome);            
    printf("%-15d", atual->quantidade);      
    printf("%10.2f\n", atual->preco);        
}


void Exibir_Produtos(Produto *inicio) {
    Produto *atual = inicio;

    if(atual == NULL) {
        printf("Nenhum produto cadastrado.\n");
        return;
    }

    printf("\nProdutos na lista:\n");
    printf("ID\t\tNome\t\tQuantidade\t\tPreço\n");
    printf("------------------------------------------------------------\n");
    
    while (atual != NULL) {
        Mostrar_um_Produto(atual);
        atual = atual->proximo;
    }
}


void Liberar_Lista(Produto *inicio) {
    Produto *atual = inicio;
    
    while (atual != NULL) {
        Produto *temp = atual;
        atual = atual->proximo;
        free(temp);
    }
}


Produto* Buscar_ID(int id, Produto *inicial, int *encontrado){
    Produto *atual = inicial;
    *encontrado = 0;

    while(atual != NULL){
        if(id != atual->id)
            atual = atual->proximo;

        else{
            *encontrado = 1;
            return atual;
        }
    }
   
    if(!(*encontrado))
        return NULL;
}


Produto* Buscar_Nome(char Nome[30], Produto *inicial, int *encontrado) {
   Nome[strcspn(Nome, "\n")] = '\0';
   
    Produto *atual = inicial;
    *encontrado = 0;

    while(atual != NULL) {
        if(strcmp(atual->nome, Nome) == 0) {
            *encontrado = 1;
            return atual;
        }
        atual = atual->proximo;
    }

    if(!encontrado)
        printf("Produto com nome %s não encontrado.\n", Nome);
        return NULL;
}


void Buscar_Preco(float Preco, Produto *inicial) {
    Produto *atual = inicial;
    int encontrado = 0;

    while(atual != NULL) {
        if(atual->preco == Preco) {
            Mostrar_um_Produto(atual);
            encontrado = 1;
        }
        atual = atual->proximo;
    }

    if(!encontrado)
        printf("Nenhum produto com preço %.2f foi encontrado!\n", Preco);
}


void Adicionar_Produto(Produto **inicio, int id, const char *nome, float preco, int quantidade, int id_fornecedor, Fornecedor **fornetes) {
    Produto *atual = *inicio;
    Produto *anterior = NULL;

    while(atual != NULL) {
        if(atual->id == id) { 
            atual->quantidade += quantidade;
            printf("Produto atualizado com novo total de unidades: %d unidades.\n", atual->quantidade);
            Mostrar_um_Produto(atual);
            return;
        }
        anterior = atual;
        atual = atual->proximo;
    }

    Produto *novo = (Produto *)malloc(sizeof(Produto));
    if(novo == NULL) {
        printf("Erro ao alocar memória para o novo produto.\n");
        return;
    }
    
    novo->id = id;
    strcpy(novo->nome, nome);
    novo->preco = preco;
    novo->quantidade = quantidade;
    novo->fornecedor_id = id_fornecedor;
    novo->proximo = NULL;
    Adicionar_Fornecedor(fornetes, id_fornecedor);

    if(*inicio == NULL)
        *inicio = novo;
    else
        anterior->proximo = novo;
    printf("Produto adicionado com sucesso.\n");
}


void Comprar_Produto(int ID, int comprar_quantidades, Produto *inicial, int *encontrado) {
    Produto *atual = inicial;

    while(atual != NULL) {
        if(atual->id == ID) {
            if(encontrado && comprar_quantidades > 0) {
                if((atual->quantidade) - comprar_quantidades >= 0) {
                    printf("Produto %s foi comprado com sucesso!\n", atual->nome);
                    atual->quantidade -= comprar_quantidades;
                } 
                else {
                    printf("Produto %s apenas com %i unidades no momento!\n", atual->nome, atual->quantidade);
                    return;
                }
            }
            *encontrado = 1;
            return;
        }
        atual = atual->proximo;
    }

    *encontrado = 0;
    printf("Produto com ID %d não encontrado!\n", ID);
}


void Remover_Produto(int ID, int remover_quantidades, Produto *inicial, int *encontrado) {
    Produto *atual = inicial;

    while (atual != NULL) {
        if(atual->id == ID) {
            if(encontrado && remover_quantidades > 0) {
                if((atual->quantidade) - remover_quantidades >= 0) {
                    printf("%i unidade(s) removida(s) do produto!\n", remover_quantidades);
                    atual->quantidade -= remover_quantidades;
                } 
                else {
                    printf("Todas as unidades foram removidas do produto!\n");
                    atual->quantidade = 0;
                }
                Mostrar_um_Produto(atual);
            }
            *encontrado = 1;
            return;
        }
        atual = atual->proximo;
    }

    *encontrado = 0;
    printf("Produto com ID %d não encontrado!\n", ID);
}


Venda* historico(int quantidade, Produto *atual, Venda *recibos, const char *data_atual) {
    Venda *nova_venda = (Venda *)malloc(sizeof(Venda));
    if(!nova_venda) {
        printf("Erro de alocação de memória!\n");
        return recibos;
    }

    nova_venda->produto.id = atual->id;
    strcpy(nova_venda->produto.nome, atual->nome);
    nova_venda->produto.quantidade = quantidade;
    nova_venda->produto.preco = quantidade * atual->preco;
    strcpy(nova_venda->data, data_atual);
    nova_venda->proxima = NULL;

    if (recibos == NULL)
        return nova_venda;

    Venda *aux = recibos;
    while (aux->proxima != NULL)
        aux = aux->proxima;

    aux->proxima = nova_venda;

    return recibos;
}

void obterDataAtual(char *buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}


void liberar_historico_vendas(Venda *historico) {
    while(historico != NULL) {
        Venda *temp = historico;
        historico = historico->proxima;
        free(temp);
    }
}


void exibir_historico_vendas(Venda *historico) {
    if(historico == NULL) {
        printf("Nenhuma venda registrada até o momento.\n");
        return;
    }

    printf("\n=== Histórico de Vendas ===\n");
    printf("ID\tNome\t\tQuantidade\tTotal\t\tData\n");
    printf("-----------------------------------------------------------\n");

    Venda *atual = historico;
    while(atual != NULL) {
        printf("%-10d%-20s%-15d%-10.2f%-12s\n",
               atual->produto.id, atual->produto.nome,
               atual->produto.quantidade, atual->produto.preco,
               atual->data);
        atual = atual->proxima;
    }
}


void gerarRelatorioMaisVendidos(Venda *historico) {
    int i = 0, j = 0, encontrado = 0, total_produtos = 0, conta_relatorio = 0;

    if (historico == NULL) {
        printf("Nenhuma venda registrada até o momento.\n");
        return;
    }

    Venda *atual = historico;
    while (atual != NULL) {
        total_produtos++;
        atual = atual->proxima;
    }

    RelatorioVendas *relatorio = malloc(total_produtos * sizeof(RelatorioVendas));

    atual = historico;
    while (atual != NULL) {
        encontrado = 0;
        for(i = 0; i < conta_relatorio; i++) {
            if(relatorio[i].id == atual->produto.id) {
                relatorio[i].quantidade_vendida += atual->produto.quantidade;
                encontrado = 1;
                break;
            }
        }

        if(!encontrado) {
            relatorio[conta_relatorio].id = atual->produto.id;
            strcpy(relatorio[conta_relatorio].nome, atual->produto.nome);
            relatorio[conta_relatorio].quantidade_vendida = atual->produto.quantidade;
            conta_relatorio++;
        }

        atual = atual->proxima;
    }

    for(i = 0; i < conta_relatorio - 1; i++) {
        for(j = i + 1; j < conta_relatorio; j++) {
            if(relatorio[i].quantidade_vendida < relatorio[j].quantidade_vendida) {
                RelatorioVendas temp = relatorio[i];
                relatorio[i] = relatorio[j];
                relatorio[j] = temp;
            }
        }
    }

    printf("\n=== Relatório de Produtos Mais Vendidos ===\n");
    printf("ID\tNome\t\tQuantidade Vendida\n");
    printf("-----------------------------------------\n");

    for (i = 0; i < conta_relatorio; i++)
        printf("%-10d%-20s%-10d\n", relatorio[i].id, relatorio[i].nome, relatorio[i].quantidade_vendida);

    free(relatorio);
}

void salvarRelatorioMaisVendidos(Venda *historico, const char *nomeArquivo) {
    int i = 0, j = 0, total_produtos = 0, encontrado = 0, conta_relatorio = 0;
    if(historico == NULL) {
        printf("Nenhuma venda registrada para gerar relatório.\n");
        return;
    }

    Venda *atual = historico;
    while(atual != NULL) {
        total_produtos++;
        atual = atual->proxima;
    }

    RelatorioVendas *relatorio = malloc(total_produtos * sizeof(RelatorioVendas));

    atual = historico;
    while(atual != NULL) {
        encontrado = 0;
        for(i = 0; i < conta_relatorio; i++) {
            if(relatorio[i].id == atual->produto.id) {
                relatorio[i].quantidade_vendida += atual->produto.quantidade;
                encontrado = 1;
                break;
            }
        }

        if(!encontrado) {
            relatorio[conta_relatorio].id = atual->produto.id;
            strcpy(relatorio[conta_relatorio].nome, atual->produto.nome);
            relatorio[conta_relatorio].quantidade_vendida = atual->produto.quantidade;
            conta_relatorio++;
        }

        atual = atual->proxima;
    }

    for(i = 0; i < conta_relatorio - 1; i++) {
        for(j = i + 1; j < conta_relatorio; j++) {
            if(relatorio[i].quantidade_vendida < relatorio[j].quantidade_vendida) {
                RelatorioVendas temp = relatorio[i];
                relatorio[i] = relatorio[j];
                relatorio[j] = temp;
            }
        }
    }

    FILE *arquivo = fopen(nomeArquivo, "w");
    if(arquivo == NULL) {
        perror("Erro ao abrir arquivo para salvar o relatório");
        free(relatorio);
        return;
    }

    fprintf(arquivo, "ID Produto, Nome Produto, Quantidade Vendida\n");
    for(i = 0; i < conta_relatorio; i++) {
        fprintf(arquivo, "%d, %s, %d\n", 
                relatorio[i].id, 
                relatorio[i].nome, 
                relatorio[i].quantidade_vendida);
    }

    fclose(arquivo);
    free(relatorio);
    printf("Relatório de produtos mais vendidos salvo em '%s'.\n", nomeArquivo);
}


void salvarHistoricoVendas(Venda *historico, const char *nomeArquivo) {
    if(historico == NULL) {
        printf("Nenhuma venda registrada para salvar.\n");
        return;
    }

    FILE *arquivo = fopen(nomeArquivo, "w");
    if(arquivo == NULL) {
        perror("Erro ao abrir arquivo para salvar o histórico de vendas");
        return;
    }

    fprintf(arquivo, "ID Produto, Nome Produto, Quantidade Vendida, Preço Unitário, Data da Venda\n");
    Venda *atual = historico;
    while(atual != NULL) {
        fprintf(arquivo, "%d, %s, %d, %.2f, %s\n", 
                atual->produto.id, 
                atual->produto.nome, 
                atual->produto.quantidade, 
                atual->produto.preco, 
                atual->data);
        atual = atual->proxima;
    }

    fclose(arquivo);
    printf("Histórico de vendas salvo em '%s'.\n", nomeArquivo);
}

Venda* lerHistoricoVendas(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if(arquivo == NULL) {
        perror("Erro ao abrir arquivo do histórico de vendas");
        return NULL;
    }

    char linha[256];
    Venda *historico = NULL, *ultima = NULL;

    //Consome cabeçalho
    fgets(linha, sizeof(linha), arquivo);

    while(fgets(linha, sizeof(linha), arquivo)) {
        Venda *novaVenda = malloc(sizeof(Venda));
        if(novaVenda == NULL) {
            perror("Erro ao alocar memória para uma nova venda");
            fclose(arquivo);
            return NULL;
        }

        sscanf(linha, "%d, %[^,], %d, %f, %[^\n]", 
               &novaVenda->produto.id,
               novaVenda->produto.nome, 
               &novaVenda->produto.quantidade, 
               &novaVenda->produto.preco, 
               novaVenda->data);

        novaVenda->proxima = NULL;

        if(historico == NULL)
            historico = novaVenda;

        else
            ultima->proxima = novaVenda;

        ultima = novaVenda;
    }

    fclose(arquivo);
    printf("Histórico de vendas carregado de '%s'.\n", nomeArquivo);
    return historico;
}


RelatorioVendas* lerRelatorioMaisVendidos(const char *nomeArquivo, int *quantidadeProdutos) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if(arquivo == NULL) {
        perror("Erro ao abrir arquivo do relatório de mais vendidos");
        *quantidadeProdutos = 0;
        return NULL;
    }

    struct stat st;
    if(stat(nomeArquivo, &st) == 0 && st.st_size == 0) {
        printf("Arquivo vazio: '%s'.\n", nomeArquivo);
        fclose(arquivo);
        *quantidadeProdutos = 0;
        return NULL;
    }

    char linha[256];
    RelatorioVendas *relatorio = NULL;
    int capacidade = 10; //Tamanho inicial do array dinâmico
    *quantidadeProdutos = 0;

    relatorio = malloc(capacidade * sizeof(RelatorioVendas));
    if (relatorio == NULL) {
        perror("Erro ao alocar memória para o relatório");
        fclose(arquivo);
        return NULL;
    }

    //Consome cabeçalho
    if (fgets(linha, sizeof(linha), arquivo) == NULL) {
        printf("Arquivo sem conteúdo relevante (apenas cabeçalho ou vazio): '%s'.\n", nomeArquivo);
        fclose(arquivo);
        free(relatorio);
        *quantidadeProdutos = 0;
        return NULL;
    }

    while(fgets(linha, sizeof(linha), arquivo)) {
        if(*quantidadeProdutos >= capacidade) {
            capacidade *= 2;
            relatorio = realloc(relatorio, capacidade * sizeof(RelatorioVendas));
            if(relatorio == NULL) {
                perror("Erro ao redimensionar memória para o relatório");
                fclose(arquivo);
                return NULL;
            }
        }

        sscanf(linha, "%d, %[^,], %d", 
               &relatorio[*quantidadeProdutos].id,
               relatorio[*quantidadeProdutos].nome,
               &relatorio[*quantidadeProdutos].quantidade_vendida);

        (*quantidadeProdutos)++;
    }

    fclose(arquivo);

    if(*quantidadeProdutos == 0) {
        free(relatorio);
        relatorio = NULL;
    }

    printf("Relatório de mais vendidos carregado de '%s'.\n", nomeArquivo);
    return relatorio;
}


void Conferir_Estoque(Produto *inicio, int limite) {
    int encontrou = 0;
    if (!inicio) {
        printf("Nenhum produto cadastrado.\n");
        return;
    }

    printf("\n==== Produtos com Pouco Estoque (Limite: %d) ====\n", limite);
    Produto *atual = inicio;
    while(atual != NULL) {
        if(atual->quantidade < limite) {
            Mostrar_um_Produto(atual);
            encontrou = 1;
        }
        atual = atual->proximo;
    }

    if(!encontrou) {
        printf("Todos os produtos estão com estoque acima de %d unidades.\n", limite);
    }
}

void Adicionar_Fornecedor(Fornecedor **inicio, int id) {
    Fornecedor *novo = (Fornecedor *)malloc(sizeof(Fornecedor));
    char fornecedor_nome[50];
    char fornecedor_contato[50];
    char fornecedor_email[50];

    if(!novo) {
        printf("Erro ao alocar memória.\n");
        return;
    }

    novo->id = id;

    printf("Digite o nome do fornecedor: ");
    scanf(" %[^\n]s", fornecedor_nome);
    strcpy(novo->nome, fornecedor_nome);

    printf("Digite o contato do fornecedor: ");
    scanf(" %[^\n]s", fornecedor_contato);
    strcpy(novo->contato, fornecedor_contato);

    printf("Digite o email do fornecedor: ");
    scanf(" %[^\n]s", fornecedor_email);
    strcpy(novo->email, fornecedor_email);

    novo->proximo = *inicio;
    *inicio = novo;

    printf("Fornecedor adicionado com sucesso!\n");
}


void Exibir_Fornecedores(Fornecedor *inicio) {
    if (!inicio) {
        printf("Nenhum fornecedor cadastrado.\n");
        return;
    }

    Fornecedor *atual = inicio;
    while(atual) {
        printf("ID: %d\n", atual->id);
        printf("Nome: %s\n", atual->nome);
        printf("Contato: %s\n", atual->contato);
        printf("Email: %s\n\n", atual->email);
        atual = atual->proximo;
    }
}

Fornecedor* Buscar_Fornecedor(int id, Fornecedor *inicio) {
    while(inicio) {
        if(inicio->id == id)
            return inicio;

        inicio = inicio->proximo;
    }
    return NULL;
}


void Associar_Produto_Fornecedor(Produto *produto, Fornecedor *inicio, int id) {
    int encontrado = 0;
    if(!produto || !inicio) {
        printf("Erro: Lista de produtos ou fornecedores está vazia.\n");
        return;
    }

    Produto *aux = Buscar_ID(id, produto, &encontrado);

    if(!aux) {
        printf("Erro: Produto com ID %d não encontrado.\n", id);
        return;
    }

    Fornecedor *fornecedor = Buscar_Fornecedor(aux->fornecedor_id, inicio);

    if(!fornecedor) {
        printf("Erro: Fornecedor com ID %d não encontrado.\n", aux->fornecedor_id);
        return;
    }

    printf("O fornecedor do produto '%s' é '%s'.\n", aux->nome, fornecedor->nome);
}


void Menus(int qual_menu) {
    printf("\n=================================================\n");
    if(qual_menu == 1)
        printf("[Cliente]\n\n1 - Buscar produto\n2 - Comprar produto\n3 - Encerrar sessão\n");
    else
        printf("[Administrador]\n\n1 - Buscar produto\n2 - Adicionar produto\n3 - Remover produto\n4 - Exibir produtos\n5 - imprimir historico\n6 - Gerar relatorio\n7 - Conferir estoque\n8 - Fornecedores\n9 - Imprimir fornecedores\n10 - Encerrar sessão\n");  
    printf("=================================================\n");
}