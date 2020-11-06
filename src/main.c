#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <string.h>

#define QTD_GIS 5
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

struct CELL
{
    int isUrban; // Estado da celula
    int isUnavailable; // pode mudar de estado?
    double probTransicao; // probabilidade de transição
    double gis[QTD_GIS]; // Valores GIS desta celula
    int qtdGis; // talvez precisemos para fazer o calculo
    int pos_i;
    int pos_j;
} typedef Cell;

struct AUTOMATA{
    Cell* cells;
    int width;
    int height;
}typedef Automata;

Automata* create_automata(int, int);
void copy_automata(Automata*, Automata*);
Automata* allocate(int, int);
int* list_of_neighbors(Automata*, int, int, int, int*);
void print_automata(Automata*);
// void simulate_automata(Automata*, size_t);
void simulate_automata(Automata*, Automata*, double);
double calculate_prob(Cell cell);
int main(void){
    srand(time(NULL));
    int ordem = 3;
    int x = ordem, y = ordem;
    int iteracoes = 10;
    double trashold = 0.11;
    Automata* automata = create_automata(x,y);
    Automata* automataAux = allocate(x, y);
    copy_automata(automataAux, automata); // A = B

    puts("===Before===");
    print_automata(automata);

    for (size_t i = 0; i < iteracoes; i++)
    {
        simulate_automata(automata, automataAux, trashold);
        copy_automata(automataAux, automata);
    }
    puts("===After===");
    print_automata(automata);
    
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


Cell* create_random_matrix(int width, int height){
    Cell* matriz = malloc(width*height*sizeof(Cell));
    
    for (size_t i = 0; i < width; i++)
    {
        for (size_t j = 0; j < height; j++)
        {
            matriz[i*width + j].isUrban = ((double) rand() / (double) RAND_MAX) < 0.20 ? 1 : 0; // 20% de chance da celula ja ser urbanizada
            matriz[i*width + j].qtdGis = QTD_GIS;
            matriz[i*width + j].isUnavailable = rand() % 2; // 0 ou 1
            matriz[i*width + j].probTransicao = (double) rand() / (double) RAND_MAX; // entre 0 e 1
            matriz[i*width + j].pos_i = i;
            matriz[i*width + j].pos_j = j;
            
            for (size_t k = 0; k < QTD_GIS; k++)
            {
                matriz[i*width + j].gis[k] = rand() % 5 + 1; // [0, 5]
            }
            
        }   
    }
    return matriz;
}

Automata* allocate(int width, int height) {
    Automata* automata = malloc(sizeof(Automata));
    automata->cells = malloc(width*height*sizeof(Cell));
}

Automata* create_automata(int width, int height){
    Automata* automata = malloc(sizeof(Automata));
    automata->width = width;
    automata->height = height;
    automata->cells = create_random_matrix(width, height);
    return automata;
}

void copy_automata(Automata* autoA, Automata* autoB) {
    // Automata* newAutomato = malloc(sizeof(Automata));
    autoA->width = autoB->width;
    autoA->height = autoB->height;
    for (size_t i = 0; i < autoB->width * autoB->height; i++)
    {
        autoA->cells[i] = autoB->cells[i];
    }
}

int* list_of_neighbors(Automata* automata, int pos_i, int pos_j, int neighborhood_size, int* n_neighbors){
    int m_size = (neighborhood_size * 2) +1;
    int* temp_array = malloc(sizeof(int) * m_size * m_size);
    *n_neighbors = 0;
    for (int i = pos_i - neighborhood_size; i <= pos_i + neighborhood_size; i++){
        for(int j = pos_j - neighborhood_size; j<= pos_j + neighborhood_size; j++){
            if(i >= 0 && i < automata->width){ //inside left and right boundries
                if(j >= 0 && j < automata->height){ //inside up and down boundries
                    if(!(i == pos_i && j == pos_j)){ //don't pick yourself
                        temp_array[*n_neighbors] = i*automata->width + j;
                        *n_neighbors = *n_neighbors + 1;
                    }
                }
            }
        }
    }
    int* final_array = malloc(*n_neighbors * sizeof(int));
    for(int i = 0; i < *n_neighbors; i++){
        final_array[i] = temp_array[i];
    }
    free(temp_array);
    return final_array;
}

Cell calculate_cell(Cell cell){
    //Do something with this cell. for eg.
    cell.gis[0] = 0;
    return cell;
}

double calculate_prob(Cell cell) {
    // conta ficticia so para testes
    double sum = 0.0;
    for (int i = 0; i < cell.qtdGis; i++)
    {
        sum += cell.gis[i];
    }
    
    return (cell.probTransicao + sum) / 100;
}

// void simulate_automata(Automata* automata, size_t n_iterations){
//     for(size_t curr_iteration = 0; curr_iteration < n_iterations; curr_iteration++){
//         for(int i = 0; i < automata->width; i++){
//             for(int j = 0; j< automata->height; j++){
//                 automata->cells[i*automata->width + j] = calculate_cell(automata->cells[i*automata->width + j]);
//             }
//         }
//     }
// }

void simulate_automata(Automata* automataAux, Automata* automata, double trashold){
    int width = automata->width;
    int height = automata->height;

    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            automataAux->cells[i*automataAux->width + j].probTransicao = calculate_prob(automata->cells[i*automata->width + j]); //calculate_cell(automata->cells[i*automata->width + j]);
            
            automataAux->cells[i*automataAux->width + j].isUrban = automataAux->cells[i*automataAux->width + j].probTransicao < trashold
                ? 1 : 0;
        }
    }
    
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
                curr_cell.isUnavailable,
                curr_cell.probTransicao);

            for (size_t k = 0; k < QTD_GIS; k++)
            {
                printf("%lf ", curr_cell.gis[k]);
            }
            printf("]\n");
        }
    }
}