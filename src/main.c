#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <string.h>

#define QTD_GIS 5

struct CELL
{
    int isUnavailable; // pode mudar de estado?
    double probTransicao; // probabilidade de transição
    double gis[QTD_GIS]; // Valores GIS desta celula
} typedef Cell;



int main(void){
    srand(time(NULL));
    int ordem = 10;
    int x = ordem, y = ordem;
    Cell* matriz = malloc(x*y*sizeof(Cell));

    for (size_t i = 0; i < x; i++)
    {
        for (size_t j = 0; j < y; j++)
        {
            matriz[i*ordem + j].isUnavailable = rand() % 2; // 0 ou 1
            matriz[i*ordem + j].probTransicao = (double) rand() / (double) RAND_MAX; // entre 0 e 1
            
            for (size_t k = 0; k < QTD_GIS; k++)
            {
                matriz[i*ordem + j].gis[k] = rand() % 5 + 1; // [0, 5]
            }
            
        }   
    }

    for (size_t i = 0; i < ordem; i++)
    {
        for (size_t j = 0; j < ordem; j++)
        {
            printf("cell [%zu][%zu] -> mudavel: %d, prob: %lf, gis: ", i,j,matriz[i*ordem + j].isUnavailable, matriz[i*ordem + j].probTransicao);

            for (size_t k = 0; k < QTD_GIS; k++)
            {
                printf("%lf ", matriz[i*ordem + j].gis[k]);
            }
            printf("]\n");
        }
    }
    
    
    // int th_id, nthreads, flag = -1;
    // #pragma omp parallel private(th_id)
    // {
    //     th_id = omp_get_thread_num();
    //     printf("Hello from %d\n", th_id);
    //     #pragma omp barrier
    //     #pragma omp single
    //     nthreads = omp_get_num_threads();
    //     flag = th_id;
    //     #pragma omp barrier
    //     #pragma omp single
    //     printf("Hi, I'm thread %d,\nThread %d discoverd that there are %d threads\n",th_id, flag, nthreads);
    // }
    return 0;
}