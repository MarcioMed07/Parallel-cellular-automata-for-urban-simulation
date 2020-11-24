#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define QTD_GIS 5
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))


struct CELL
{
    int isUrban; // Estado da celula
    int isAvailable; // pode mudar de estado?
    double probTransicao; // probabilidade de transição
    double gis[QTD_GIS]; // Valores GIS desta celula
    double pesos[QTD_GIS]; // pesos para cada valor gis
    int qtdGis; // talvez precisemos para fazer o calculo
    int pos_i;
    int pos_j;
} typedef Cell;

struct AUTOMATA{
    Cell* cells;
    int width;
    int height;

    double ligma; // usada para o calculo de transicao
    double alpha; // usada para o calculo de transicao
    int neighborhoodSize;
}typedef Automata;


Cell* create_random_matrix(int width, int height){
    Cell* matriz = malloc((size_t)width*height*sizeof(Cell));
    
    for (size_t i = 0; i < width; i++)
    {
        for (size_t j = 0; j < height; j++)
        {
            matriz[i*width + j].isUrban = ((double) rand() / (double) RAND_MAX) < 0.05 ? 1 : 0; // 5% de chance da celula já ser urbanizada
            matriz[i*width + j].qtdGis = QTD_GIS;
            matriz[i*width + j].isAvailable = ((double) rand() / (double) RAND_MAX) >= 0.20 ? 1 : 0; // 20% de chance da celula ser imutável
            matriz[i*width + j].probTransicao = (double) rand() / (double) RAND_MAX; // entre 0 e 1
            matriz[i*width + j].pos_i = i;
            matriz[i*width + j].pos_j = j;
            
            for (size_t k = 0; k < QTD_GIS; k++)
            {
                matriz[i*width + j].gis[k] = rand() % 5 + 1; // [0, 5]
                matriz[i*width + j].pesos[k] = ((double) rand() / (double) RAND_MAX) + 1; // [1.0, 2.0]
            }
            
        }   
    }
    return matriz;
}


Automata* create_automata(int width, int height){
    Automata* automata = malloc(sizeof(Automata));
    automata->width = width;
    automata->height = height;
    automata->ligma = 10.0;
    automata->alpha = 1.0;
    automata->neighborhoodSize = 1;
    automata->cells = create_random_matrix(width, height);
    return automata;
}

void free_automata(Automata* automato) {
    free(automato->cells);
    free(automato);
}

void print_automata(Automata* automata){
    for (size_t i = 0; i < automata->width; i++)
    {
        for (size_t j = 0; j < automata->height; j++)
        {
            Cell curr_cell = automata->cells[i*automata->width + j];
            printf("cell [%zu][%zu] -> isUrban: %d, mudavel: %d, prob: %lf, gis: ",
                i,
                j,
                curr_cell.isUrban,
                curr_cell.isAvailable,
                curr_cell.probTransicao);

            for (size_t k = 0; k < QTD_GIS; k++)
            {
                printf("%lf ", curr_cell.gis[k]);
            }
            printf("]\n");
        }
    }
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    
    int ordem = atoi(argv[1]);
    
    int x = ordem, y = ordem;
    Automata* automata = create_automata(x,y);

    // print_automata(automata);   

    FILE* f = fopen64(argv[2], "wb");

    fwrite(&automata->width, sizeof(int), 1, f);
    fwrite(&automata->height, sizeof(int), 1, f);
    fwrite(&automata->ligma, sizeof(double), 1, f);
    fwrite(&automata->alpha, sizeof(double), 1, f);
    fwrite(&automata->neighborhoodSize, sizeof(int), 1, f);
    fwrite(automata->cells, (size_t) automata->width*automata->height*sizeof(Cell), 1, f);

    fclose(f);
    free_automata(automata);
}
