//
// Created by jordic on 06/02/24.
//

#include "RLSATEncoding.h"
#include <vector>

using namespace std;

RLSATEncoding::RLSATEncoding(RLSAT * instance) {
    this->instance = instance;
}

SMTFormula *  RLSATEncoding::encode(int lb, int ub) {

    SMTFormula * f = new SMTFormula();

    // x_a_s_h agent a is assigned to service s at timeslot h
    vector<vector<vector<literal>>> x(instance->A,vector<vector<literal>>(instance->S,vector<literal>(instance->TS)));

    for(int i = 0; i < instance->A; i++)
        for(int j = 0; j < instance->S; j++)
            for(int k = 0; k < instance->TS; k++)
                x[i][j][k]=f->newBoolVar("x",i,j,k);

    

}
