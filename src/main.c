#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <string.h>
#include <math.h>

// COMPILACAO: gcc -fopenmp main.c -lm

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

Automata* read_from_file(FILE *f);
Automata* create_automata(int, int);
void copy_automata(Automata*, Automata*);
Automata* allocate(int, int);
int* list_of_neighbors(Automata*, int, int, int, int*);
void print_automata(Automata*);
// void simulate_automata(Automata*, size_t);
void simulate_automata(Automata*, Automata*, double, int, int*);
// double calculate_prob(Cell cell);
double calculate_prob(Automata*, Cell);
int* divide_automato(Automata*, int, int*, double, double);
void free_automata(Automata*);
void printVetor(char*,int*,int);
void printVetorDouble(char*, double*, int);
int linha_inicial(int, int*);
int linha_final(int, int*);
double numerador(double, double);
double denominador(Cell);
double evaluation_score(double*, double*, int);
double develop_intensity(Cell*, int*, int);
double total_constrait();
int main(int argc, char *argv[]){
    srand(time(NULL));
    // int ordem = 10;
    // int x = ordem, y = ordem;
    int iteracoes = atoi(argv[1]);
    double trashold = 0.30;
    // Automata* automata = create_automata(x,y);
    FILE* ptr;
    ptr = fopen(argv[3],"rb");
    if(ptr == NULL){
        printf("Cant read file");
        return 1;
    }
    Automata* automata = read_from_file(ptr);
    Automata* automataAux = allocate(automata->width, automata->height);
    copy_automata(automataAux, automata); // A = B

    const int qtdThreads = atoi(argv[2]);
    int* indices = (int*)malloc((qtdThreads)*sizeof(int));

    indices = divide_automato(automata, qtdThreads, indices, 0.000127, 0.003674);
    // printVetor("indices", indices, qtdThreads);

    // puts("===Before===");
    // print_automata(automata);

    #pragma omp parallel num_threads(qtdThreads)
    {
        int thId = omp_get_thread_num();
        // printf("thread %d fará de %d até %d\n", thId, linha_inicial(thId, indices), linha_final(thId, indices));
        for (size_t i = 0; i < iteracoes; i++)
        {
            simulate_automata(automata, automataAux, trashold, thId, indices);
            #pragma omp barrier
            #pragma omp single
            copy_automata(automataAux, automata);

            #pragma omp barrier
        }
    }
    // puts("===After===");
    // print_automata(automata);
    
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

    free_automata(automata);
    free_automata(automataAux);
    return 0;
}

Automata* read_from_file(FILE *f){
    size_t width = 0;
    size_t height = 0;
    fread(&width, sizeof(int), 1, f);
    fread(&height, sizeof(int), 1, f);
    Automata* automata = allocate(width, height);
    automata->width = width;
    automata->height = height;
    fread(&automata->ligma, sizeof(double), 1, f);
    fread(&automata->alpha, sizeof(double), 1, f);
    fread(&automata->neighborhoodSize, sizeof(int), 1, f);
    fread(automata->cells,(size_t)automata->width*automata->height*sizeof(Cell), 1, f);
    fclose(f);
    return automata;
}

/**
 * exemplo:
 * Se a qtd for 2, deve-se retornar um indice k, thrad_1 => [0, k), thread_2 => [k, ordem); 
 * se qtd = 3, retornará dois indices: k1 e k2, thread_1 => [0, k1), thread_2 => [k1, k2), thread_3 => [k2, ordem)
 * ...
 * qtd = n, retorna (n-1) indices: k_1, ... K_(n-1), thread_1 => [0, k_1), ..., thread_a => [k_(a-1), k_a), ..., thread_(n-1) => [k_(n-2), ordem)
 **/
int* divide_automato(Automata* automato, int qtd, int* indices, double time_uv, double time_av) {
    const int ordem = automato->width; // O AUTOMATO PRECISA SER QUADRADO
    double sum = 0.0;
    double* linhas = (double*)malloc(ordem*sizeof(double)); // quardar a soma de cada linha do automato
    for (size_t i = 0; i < ordem; i++)
    {
        int count_uv = 0, count_av = 0; // quantidade de celulas unVaieble e avaieble
        for (size_t j = 0; j < ordem; j++)
        {
            if (automato->cells[i*ordem + j].isAvailable) {
                count_av++;
            } else {
                count_uv++;
            }
        }
        linhas[i] = count_uv*time_uv + count_av*time_av;
    }
    
    for (size_t i = 0; i < ordem; i++)
    {
        sum += linhas[i];
    }

    double workForLine = sum / qtd;
    // printVetorDouble("tempo de cada linha", linhas, ordem);
    // printf("Workflow = %lf\n", workForLine);
    // printf("total = %lf\n", sum);
    for (size_t ind = 0, i = 0; ind < qtd; ind++)
    {
        double sumLine = 0.0;
        while (i < ordem && sumLine <= workForLine)
        {
            sumLine += linhas[i];
            i++;
        }

        indices[ind] = i; // a thread ind só irá até o indice i
    }
    free(linhas);
    return indices;
}


Cell* create_random_matrix(int width, int height){
    Cell* matriz = malloc(width*height*sizeof(Cell));
    
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

Automata* allocate(int width, int height) {
    Automata* automata = malloc(sizeof(Automata));
    automata->cells = malloc((size_t)width*height*sizeof(Cell));
    return automata;
}

void free_automata(Automata* automato) {
    free(automato->cells);
    free(automato);
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

void copy_automata(Automata* autoA, Automata* autoB) {
    // Automata* newAutomato = malloc(sizeof(Automata));
    autoA->width = autoB->width;
    autoA->height = autoB->height;
    autoA->ligma = autoB->ligma;
    autoA->alpha = autoB->alpha;
    autoA->neighborhoodSize = autoB->neighborhoodSize;
    for (size_t i = 0; i < autoB->width * autoB->height; i++)
    {
        autoA->cells[i] = autoB->cells[i];
    }
}

// n_neighbors retorna q quantidade de vizinhos
// final_array retorna os indices vizinhos de automata
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

double calculate_prob(Automata* automato, Cell cell) {
    int size = 0;
    int* vizinhos = list_of_neighbors(automato, cell.pos_i, cell.pos_j, automato->neighborhoodSize, &size);
    double prob = numerador(automato->ligma, automato->alpha) / denominador(cell) * develop_intensity(automato->cells, vizinhos, size) * total_constrait();
    free(vizinhos);
    return prob;
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

void simulate_automata(Automata* automataAux, Automata* automata, double trashold, int thId, int* indices){
    int height = automata->height;

    for(int i = linha_inicial(thId, indices); i < linha_final(thId, indices); i++){
        for(int j = 0; j < height; j++){
            // automataAux->cells[i*automataAux->width + j].probTransicao = calculate_prob(automata->cells[i*automata->width + j]); 
            
            if (automata->cells[i*automataAux->width + j].isAvailable) {
                automataAux->cells[i*automataAux->width + j].probTransicao = calculate_prob(automata, automata->cells[i*automataAux->width + j]);
                automataAux->cells[i*automataAux->width + j].isUrban = automataAux->cells[i*automataAux->width + j].probTransicao >= trashold
                    ? 1 : 0;
            }
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

void printVetor(char* str,int* vetor, int size) {
    printf("%s = ", str);
    for (size_t i = 0; i < size; i++)
    {
        printf("%d ", vetor[i]);
    }
    printf("\n");
}

void printVetorDouble(char* str, double* vetor, int size) {
    printf("%s = ", str);
    for (size_t i = 0; i < size; i++)
    {
        printf("%.6lf ", vetor[i]);
    }
    printf("\n");
}

int linha_inicial(int thId, int* indices) {
    int ant = thId -1;
    if (ant < 0) {
        return 0; // caso especial: primeira thread, primeira linha
    }
    return indices[ant];
}

int linha_final(int thId, int* indices) {
    return indices[thId];
}

double numerador(double ligma, double alpha) {
    return 1.0 + pow(-log(ligma), alpha);
}

double denominador(Cell cell) {
    double r = evaluation_score(cell.gis, cell.pesos, cell.qtdGis);
    return 1.0 + exp(-r);
}

double evaluation_score(double* gis, double* pesos, int size) {
    double sum = 0.0;
    for (size_t i = 0; i < size; i++)
    {
        sum += gis[i]*pesos[i];
    }
    return sum;
}

double develop_intensity(Cell* matriz, int* vizinhos, int size) {
    double sum = 0.0;
    for (size_t i = 0; i < size; i++)
    {
        sum += matriz[vizinhos[i]].probTransicao;
    }
    return sum / size;
}

double total_constrait() {
    return 1.0; // ainda nao sabemos o que e...
}
