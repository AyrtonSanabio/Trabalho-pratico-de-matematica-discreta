#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAMINHO "../lista.dot"

#define COR_FECHO "[color=red];\n"

#define PRI_LIN_ARQ "digraph fecho\n"

#define EXTENSAO_REF "-ref.dot"
#define EXTENSAO_SIM "-sim.dot"
#define EXTENSAO_TRA "-tra.dot"

#define ERRO_ENTRADA "Os parametros digitados estao com a sintaxe errada. Sintaxe correta: (./check-closure.bin <arquivo-entrada> <preâmbulo-saida>)"

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
        return 0;
    }

    char linha[8];
    fgets(linha, sizeof(linha), arquivo);

    int ordem;
    sscanf(linha,"n %d", &ordem);

    fclose(arquivo);
    return ordem;
}
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
void fecho_transitivo (matriz* matriz, int ordem) {
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
}

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

    int ordem = le_ordem(entrada);
    matriz m;
    m.principal = aloca_matriz(ordem);
   
    le_arquivo(entrada, m.principal);
    
    eh_reflexiva(&m, ordem);
    eh_simetrico(&m, ordem);
    eh_transitiva(&m, ordem);

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
    
    printf("\nReflexiva: %d", m.eh_reflexivo);
    printf("\nSimetrico: %d", m.eh_simetrico);
    printf("\nTransitivo: %d", m.eh_transitivo);
    printf("\n");

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

    if (!m.eh_reflexivo) free(m.fecho_reflexivo);
    if (!m.eh_simetrico) free(m.fecho_simetrico);
    if (!m.eh_transitivo) free(m.fecho_transitivo);
    
    return 0;
}