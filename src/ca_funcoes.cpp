#include "corpo_automato.h"
#include <vector>
#include <cmath>

// Composite evaluation score. 
// capitulo 2.1; pagina 806
// Para cada variavel em gis, será multiplicado por um peso.
double composite_score(std::vector<double> gis, std::vector<double> weights) {
    double r = 0.0;
    for (size_t i = 0; i < gis.size(); i++)
    {
        r = r + gis[i]*weights[i];
    }
    return r;
}

// transition probabilities
// capitulo 2.1; pagina 806
// retornar um percentual de trancição
double transition_potentials(std::vector<CELL> automato, int i, int j, int neiboorghSize, double phi, double alpha, std::vector<double> weights) {
    return (valor_constante(phi, alpha) / denominador(automato[i*j].gis, weights)) * funcNeiboorg(automato, i, j, neiboorghSize); // * constraint score. Não descobrie o que é
}

double valor_constante(double phi, double alpha) {
    return (1+pow(-log(phi), alpha));
}

double denominador(std::vector<double> gis, std::vector<double> weights) {
    return 1 + exp(-composite_score(gis, weights));
}

double funcNeiboorg(std::vector<CELL> automato, int i, int j, int neiboorghSize) {
    // fazer um loop que some as probabilidades das celulas vizinhas
    // i, j são o centro, e o tamanho da vizinhança é neiboorghSize

    return 0.0;
}