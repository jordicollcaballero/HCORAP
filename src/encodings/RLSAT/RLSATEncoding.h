//
// Created by jordic on 06/02/24.
//

#ifndef RLSAT_RLSATENCODING_H
#define RLSAT_RLSATENCODING_H

#include "encoding.h"
#include "RLSAT.h"

class RLSATEncoding : public Encoding {
    RLSAT * instance;
public:
    RLSATEncoding(RLSAT * instance);

    SMTFormula *  encode(int lb = INT_MIN, int ub = INT_MAX);
};


#endif //RLSAT_RLSATENCODING_H
