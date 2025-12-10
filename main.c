/*
    Trabalho de matemática discreta feito pelos integrantes:
        - Ayrton Sanabio RA: 0117925
        - Enricco Faria RA: 0117036
        - Felipe Dias RA: 0117254
        - João Vítor RA: 0117181
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//Constantes para os arquivos de saída
#define CAMINHO "../lista.dot" 

#define COR_FECHO "[color=red];\n"

#define PRI_LIN_ARQ "digraph fecho\n"

#define EXTENSAO_REF "-ref.dot"
#define EXTENSAO_SIM "-sim.dot"
#define EXTENSAO_TRA "-tra.dot"

#define ERRO_ENTRADA "Os parametros digitados estao com a sintaxe errada. Sintaxe correta: (./check-closure.bin <arquivo-entrada> <preâmbulo-saida>)"



// Estrutura que representa a matriz de uma relação e seus fechamentos associados. Usada para armazenar tanto a matriz original quanto as matrizes geradas pelos fechamentos lógico-matemáticos. 
typedef struct  
{
    int** principal; // Matriz original da relação
    int** fecho_transitivo; // Fecho transitivo da relação
    int** fecho_reflexivo; // Fecho reflexivo da relação
    int** fecho_simetrico; // Fecho simétrico da relação
    
    int eh_transitivo; // 1 se a relação é transitiva, 0 caso contrário
    int eh_reflexivo; // 1 se é reflexiva, 0 caso contrário
    int eh_simetrico; // 1 se é simétrica, 0 caso contrário
}matriz;


// Exibe a matriz na tela
void exibir_matriz(int **matriz, int ordem_matriz) {
    
    printf("\n");
    for (int i = 0; i < ordem_matriz; i++) {
        for (int j = 0; j < ordem_matriz; j++) {
            printf("[%d] ", matriz[i][j]);
        }
        printf("\n");
    }
}

// Copia os valores de uma matriz para outra, ambas devem ter a mesma ordem e já estarem alocadas
void copia_matriz(int** origem, int** copia, int ordem) {
    for (int i = 0; i < ordem; i++) {
        for (int j = 0; j < ordem; j++) {
            copia[i][j] = origem[i][j];
        }
    }
}

// Aloca dinamicamente uma matriz de inteiros
int** aloca_matriz(int ordem_matriz) {

    int **matriz = calloc(ordem_matriz, sizeof(int*));

    for (int i = 0; i < ordem_matriz; i++)
    {
        matriz[i] = calloc(ordem_matriz, sizeof(int));
    }
    
    return matriz;
}

// Puxa a ordem da matriz da primeira linha do arquivo dot
int le_ordem(char caminho[]) {

    FILE *arquivo = fopen(caminho, "r");
    if (arquivo == NULL) {
        printf("\nErro ao abrir o arquivo");
        return 0;
    }

    char linha[8];
    fgets(linha, sizeof(linha), arquivo);

    int ordem;
    sscanf(linha,"n %d", &ordem);

    fclose(arquivo);
    return ordem;
}

// Função para preencher a matriz com as relações informadas no arquivo.dot
void le_arquivo(char *nome_arquivo, int **matriz) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("\nErro ao abrir o arquivo");
        return;
    }

    char linha[8];
    fgets(linha, sizeof(linha), arquivo);
    int ordem;
    sscanf(linha,"n %d", &ordem);
    
   while (fgets(linha, sizeof(linha), arquivo) && linha[0] != 'f' && ordem) {
        int ind_lin;
        int ind_col; 

        sscanf(linha, "r %d %d", &ind_lin, &ind_col);

        if (ind_col > ordem || ind_lin > ordem)
        {
            printf("\nIndice fora dos limites da matriz");
            exit(1);
        }
        
        matriz[ind_lin - 1][ind_col - 1] = 1;
    }

    fclose(arquivo);
}

// Se existir algum elemento da diagonal principal que esteja em 0, a matriz não é reflexiva
void eh_reflexiva (matriz* matriz, int ordem) {

    for (int i = 0; i < ordem; i++)
    {
        if(matriz->principal[i][i] == 0) {
            matriz->eh_reflexivo = 0;
            return;
        }
    }

    matriz->eh_reflexivo = 1;
    return;
}

// Se não tiver um arco saindo de i para j, e um arco de j para i, a matriz não é simétrica.
void eh_simetrico (matriz* matriz, int ordem) {

    for (int i = 0; i < ordem; i++)
    {
        for (int j = i; j < ordem; j++)
        {
            if(matriz->principal[i][j] != matriz->principal[j][i]) {
                matriz->eh_simetrico = 0;
                return;
            }
        }  
    }
    matriz->eh_simetrico = 1;
    return;
}

//Testa a condição "(Se houver um arco de i para j e um arco j para k), se for verdade e não houver um arco de i para k, logo a matriz não é transitiva"
void eh_transitiva (matriz* matriz, int ordem) {

    for (int i = 0; i < ordem; i++)
    {
        for (int j = 0; j < ordem; j++)
        {
            for (int k = 0; k < ordem; k++)
            {
                if((matriz->principal[i][j] && matriz->principal[j][k]) && !(matriz->principal[i][k])) {
                    matriz->eh_transitivo = 0;
                    return;
                }
            }
        }  
    }
    matriz->eh_transitivo = 1;
    return;
}

// Nas posições da diagonal principal que não houverem relação (Estiver como 0), preenchemos como "2", para simbolizar as relações necessárias para que a matrix se torne reflexiva.
void fecho_reflexivo (matriz* matriz, int ordem) {

    matriz->fecho_reflexivo = aloca_matriz(ordem);

    //copia os dados da matriz principal
    copia_matriz(matriz->principal, matriz->fecho_reflexivo, ordem);
    
    for (int i = 0; i < ordem; i++)
    {
        if(matriz->fecho_reflexivo[i][i] == 0) {
            matriz->fecho_reflexivo[i][i] = 2;
        }
    }
}

//Se na matriz a posição [i][j] for diferente da posição [j][i], isso é, uma tem arco de ida mas a outra não tem o arco de volta, adicionamos na matriz que representa as relações para o fecho 
//simétrico o valor 2 onde não tem um arco de saída (Onde está marcado como 0).
void fecho_simetrico (matriz* matriz, int ordem) {

    matriz->fecho_simetrico = aloca_matriz(ordem);

    //copia os dados da matriz principal
    copia_matriz(matriz->principal, matriz->fecho_simetrico, ordem);

    for (int i = 0; i < ordem; i++)
    {
        for (int j = i; j < ordem; j++)
        {
            if(matriz->principal[i][j] != matriz->principal[j][i]) {
                if (matriz->principal[i][j] == 0)
                    matriz->fecho_simetrico[i][j] = 2;

                if (matriz->principal[j][i] == 0)
                    matriz->fecho_simetrico[j][i] = 2;
            }
        }  
    }
}

//Para adicionar o fecho transitivo verificamos se existe um iRk e um KrJ, logo deve ter um iRj, verificamos, e caso não exista essa relação (representado por 0), preenchemos com "2" para 
//representar os arcos necessários para o fecho transitivo.
void fecho_transitivo (matriz* matriz, int ordem) {
    matriz->fecho_transitivo = aloca_matriz(ordem);

    // copia os dados
    copia_matriz(matriz->principal, matriz->fecho_transitivo, ordem);
    
    for (int k = 0; k < ordem; k++) {
        for (int i = 0; i < ordem; i++) {
            for (int j = 0; j < ordem; j++) {
                //para todo k, se existe um iRk e um kRj, deve ter um iRj
                if (matriz->fecho_transitivo[i][k] && matriz->fecho_transitivo[k][j]) {
                    if (matriz->fecho_transitivo[i][j] == 0) {
                        matriz->fecho_transitivo[i][j] = 2; // seu valor especial
                    }
                }
            }
        }
    }
}

/* As funções abaixo "persistencia_fecho_tra", "persistencia_fecho_sim" e "persistencia_fecho_ref" são dedicadas  a saída de dados.

A lógica é semelhante nas três funções, onde abrimos um arquivo para escrita, e escrevemos nele na ordem que foi indicada no enunciado do trabalho prático.


digraph fecho  -  1° linha
{              -  2° linha
fazemos um for até a quantidade de total de elementos do conjunto, marcando em cada linha o índice de i
Aqui printamos o índice i - > indíce j onde existe uma relação na matriz.
Aqui printamos o índice i - > indíce j onde deve existir uma relação na matriz para que ela adquira a propriedade espécífica, seguida de "[color=red];" para diferenciar de vermelho os 
arcos adicionados para que ela adquira a propriedade.
} Adicionamos o fechamento de chave na última linha
*/

// Arquivo de saída do fecho transitivo
void persistencia_fecho_tra(char caminho[], matriz* matriz, int ordem){

    char caminho_novo[128];
    snprintf(caminho_novo, sizeof(caminho_novo), "%s"EXTENSAO_TRA, caminho);

    FILE *arq = fopen(caminho_novo, "w");

    if(!arq) return;
    fprintf(arq, PRI_LIN_ARQ); 
    fprintf(arq, "{\n"); 

    for (int i = 0; i < ordem; i++)
    {
        fprintf(arq, "\t%d;\n", i + 1);
    }   
    for (int i = 0; i < ordem; i++)
    {
        for (int j = 0; j < ordem; j++)
        {
            if (matriz->principal[i][j] == 1) {
                fprintf(arq, "\t%d -> %d;\n", i+1, j+1);
            }
        }
    }
    for (int i = 0; i < ordem; i++)
    {
        for (int j = 0; j < ordem; j++)
        {
            if (matriz->fecho_transitivo[i][j] == 2) {
                fprintf(arq, "\t%d -> %d "COR_FECHO, i+1, j+1);
            }
        }
    }
    
    fprintf(arq, "}\n");
}


// Arquivo de saída do fecho simétrico
void persistencia_fecho_sim(char caminho[], matriz* matriz, int ordem){

    char caminho_novo[128];
    snprintf(caminho_novo, sizeof(caminho_novo), "%s"EXTENSAO_SIM, caminho);

    FILE *arq = fopen(caminho_novo, "w");

    if(!arq) return;
    fprintf(arq, PRI_LIN_ARQ);
    fprintf(arq, "{\n");

    for (int i = 0; i < ordem; i++)
    {
        fprintf(arq, "\t%d;\n", i + 1);
    }   
    for (int i = 0; i < ordem; i++)
    {
        for (int j = 0; j < ordem; j++)
        {
            if (matriz->principal[i][j] == 1) {
                fprintf(arq, "\t%d -> %d;\n", i+1, j+1);
            }
        }
    }
    for (int i = 0; i < ordem; i++)
    {
        for (int j = 0; j < ordem; j++)
        {
            if (matriz->fecho_simetrico[i][j] == 2) {
                fprintf(arq, "\t%d -> %d "COR_FECHO, i+1, j+1);
            }
        }
    }
    
    fprintf(arq, "}\n");
}


// Arquivo de saída do fecho reflexivo
void persistencia_fecho_ref(char caminho[], matriz* matriz, int ordem){

    char caminho_novo[128];
    snprintf(caminho_novo, sizeof(caminho_novo), "%s"EXTENSAO_REF, caminho);

    FILE *arq = fopen(caminho_novo, "w");

    if(!arq) return;
    fprintf(arq, PRI_LIN_ARQ);
    fprintf(arq, "{\n");

    for (int i = 0; i < ordem; i++)
    {
        fprintf(arq, "\t%d;\n", i + 1);
    }   
    for (int i = 0; i < ordem; i++)
    {
        for (int j = 0; j < ordem; j++)
        {
            if (matriz->principal[i][j] == 1) {
                fprintf(arq, "\t%d -> %d;\n", i+1, j+1);
            }
        }
    }
    for (int i = 0; i < ordem; i++)
    {
        for (int j = 0; j < ordem; j++)
        {
            if (matriz->fecho_reflexivo[i][j] == 2) {
                fprintf(arq, "\t%d -> %d "COR_FECHO, i+1, j+1);
            }
        }
    }
    
    fprintf(arq, "}\n");
}


int main (int argc, char *argv[]) {

    if (argc != 3) {printf(ERRO_ENTRADA); return 0;}

    char *entrada = argv[1];
    char *saida = argv[2];

    
    int ordem = le_ordem(entrada); //Pegamos a ordem da matriz informada na primeira linha do arquivo.dot
    matriz m; 
    m.principal = aloca_matriz(ordem); // Criamos a matriz principal e alocamos ela na memória
   
    le_arquivo(entrada, m.principal); // Preenhemos a matriz com as relações contidas no arquivo.dot
    

    // Verificamos quais propriedades a matriz possui
    eh_reflexiva(&m, ordem);
    eh_simetrico(&m, ordem);
    eh_transitiva(&m, ordem);

    // Fazemos uma verificação, onde só adicionamos o fecho reflexivo e acionamos a função que faz o arquivo de saída com as relações correspondentes do fecho, se a matriz não tiver a propriedade
    // que queremos adicionar
    if (!m.eh_reflexivo)
    {
        fecho_reflexivo(&m, ordem);
        persistencia_fecho_ref(saida, &m, ordem);
    }
    if (!m.eh_simetrico)
    {
        fecho_simetrico(&m, ordem);
        persistencia_fecho_sim(saida, &m, ordem);
    }
    if (!m.eh_transitivo)
    {
        fecho_transitivo(&m, ordem);
        persistencia_fecho_tra(saida, &m, ordem);
    }
    
    // Exibimos no terminal quais propriedades a matriz possui (0 ou 1)
    printf("\nReflexiva: %d", m.eh_reflexivo);
    printf("\nSimetrico: %d", m.eh_simetrico);
    printf("\nTransitivo: %d", m.eh_transitivo);
    printf("\n");

    // Liberando memória alocada
    for (int i = 0; i < ordem; i++) {
        free(m.principal[i]);

        if (!m.eh_reflexivo)
            free(m.fecho_reflexivo[i]);

        if (!m.eh_simetrico)
            free(m.fecho_simetrico[i]);

        if (!m.eh_transitivo)
            free(m.fecho_transitivo[i]);
    }

    free(m.principal);

    // Fizemos uma verificação, pois se a matriz tiver a propriedade reflexiva, como exemplo, não teremos adicionado e criado a matriz para o fecho reflexivo, dispensando a liberação de memória
    if (!m.eh_reflexivo) free(m.fecho_reflexivo);
    if (!m.eh_simetrico) free(m.fecho_simetrico);
    if (!m.eh_transitivo) free(m.fecho_transitivo);
    
    return 0;
}