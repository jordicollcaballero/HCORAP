//
// Created by jordic on 05/03/24.
//

#ifndef RLSAT_HCORAPNSERVICESENCODING_H
#define RLSAT_HCORAPNSERVICESENCODING_H

#include "encoding.h"
#include "hcorap.h"


class HCORAPNServicesEncoding : public Encoding{
    HCORAP * instance;
    vector<vector<vector<literal>>> x;
    vector<vector<literal>> y;
    vector<vector<literal>> su;
    int totalSoftStrat;

    vector<clause> softClauses;
    vector<int> weights;
    vector<bool> model;

    int checkAllSoftClauses();


public:
    HCORAPNServicesEncoding(HCORAP * instance);

    SMTFormula *  encode(int lb = INT_MIN, int ub = INT_MAX);

    void checkSolution(const string & filename);
};


#endif //RLSAT_HCORAPNSERVICESENCODING_H
