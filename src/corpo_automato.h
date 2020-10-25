#define CORPO_AUTPMATO_H
#ifdef CORPO_AUTPMATO_H

#include <vector>

struct CELL {
    int isUnavailable; // pode mudar de estado?
    double probTransicao; // probabilidade de transição
    std::vector<double> gis; // Valores GIS desta celula
};

#endif