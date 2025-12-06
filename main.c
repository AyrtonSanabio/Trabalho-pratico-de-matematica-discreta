#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAMINHO "../lista.dot"

typedef struct
{
    int** principal;
    int** fecho_transitivo;
    int** fecho_reflexivo;
    int** fecho_simetrico;
    int eh_transitivo;
    int eh_reflexivo;
    int eh_simetrico;
}matriz;

void exibir_matriz(int **matriz, int ordem_matriz) {
    
    printf("\n");
    for (int i = 0; i < ordem_matriz; i++) {
        for (int j = 0; j < ordem_matriz; j++) {
            printf("[%d] ", matriz[i][j]);
        }
        printf("\n");
    }
}

void copia_matriz(int** origem, int** copia, int ordem) {
    for (int i = 0; i < ordem; i++) {
        for (int j = 0; j < ordem; j++) {
            copia[i][j] = origem[i][j];
        }
    }
}

int** aloca_matriz(int ordem_matriz) {

    int **matriz = calloc(ordem_matriz, sizeof(int*));

    for (int i = 0; i < ordem_matriz; i++)
    {
        matriz[i] = calloc(ordem_matriz, sizeof(int));
    }
    
    return matriz;
}

int le_ordem(char caminho[]) {

    FILE *arquivo = fopen(caminho, "r");
    if (arquivo == NULL) {
        printf("\nErro ao abrir o arquivo");
        return;
    }

    char linha[8];
    fgets(linha, sizeof(linha), arquivo);

    fclose(arquivo);
    
    return atoi(&linha[2]);
}

void le_arquivo(char *nome_arquivo, int **matriz) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("\nErro ao abrir o arquivo");
        return;
    }

    char linha[8];
    fgets(linha, sizeof(linha), arquivo);
    int ordem = atoi(&linha[2]);
    
   while (fgets(linha, sizeof(linha), arquivo) && linha[0] != 'f' && ordem) {
        int ind_lin = atoi(&linha[2]);
        int ind_col = atoi(&linha[4]);

        if (ind_col > ordem || ind_lin > ordem)
        {
            printf("\nIndice fora dos limites da matriz");
            exit(1);
        }
        
        matriz[ind_lin - 1][ind_col - 1] = 1;
    }

    fclose(arquivo);
}

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

void fecho_reflexivo (matriz* matriz, int ordem) {

    matriz->fecho_reflexivo = aloca_matriz(ordem);

    //copia os dados da matriz principal
    copia_matriz(matriz->principal, matriz->fecho_reflexivo, ordem);
    
    if (matriz->eh_reflexivo) return;
    
    for (int i = 0; i < ordem; i++)
    {
        if(matriz->fecho_reflexivo[i][i] == 0) {
            matriz->fecho_reflexivo[i][i] = 2;
        }
    }
}

void fecho_simetrico (matriz* matriz, int ordem) {

    matriz->fecho_simetrico = aloca_matriz(ordem);

    //copia os dados da matriz principal
    copia_matriz(matriz->principal, matriz->fecho_simetrico, ordem);
    
    if (matriz->eh_simetrico) return;

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

void fecho_transitivo(matriz* matriz, int ordem) {
    matriz->fecho_transitivo = aloca_matriz(ordem);

    // copia os dados
    copia_matriz(matriz->principal, matriz->fecho_transitivo, ordem);

    if (matriz->eh_transitivo) return;
    
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

    eh_transitiva(matriz, ordem);
}

    
    
    


int main (int argc, char *argv[]) {

    int ordem = le_ordem(CAMINHO);
    matriz m;
    m.principal = aloca_matriz(ordem);
   
    le_arquivo(CAMINHO, m.principal);
    
    eh_reflexiva(&m, ordem);
    eh_simetrico(&m, ordem);
    eh_transitiva(&m, ordem);

    fecho_reflexivo(&m, ordem);
    fecho_simetrico(&m, ordem);
    fecho_transitivo(&m, ordem);

    printf("Matriz Principal:");
    exibir_matriz(m.principal, ordem);
    printf("Fecho Reflexivo:");
    exibir_matriz(m.fecho_reflexivo, ordem);
    printf("Fecho Simetrico:");
    exibir_matriz(m.fecho_simetrico, ordem);
    printf("Fecho Transitivo:");
    exibir_matriz(m.fecho_transitivo, ordem);

    printf("\nReflexiva: %d", m.eh_reflexivo);
    printf("\nSimetrico: %d", m.eh_simetrico);
    printf("\nTransitivo: %d", m.eh_transitivo);
    printf("\n");

    for (int i = 0; i < ordem; i++)
        free(m.principal[i]);
    free(m.principal);
    
    return 0;
}