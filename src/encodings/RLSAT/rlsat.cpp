//
// Created by jordic on 06/02/24.
//

#include "rlsat.h"

RLSAT::RLSAT(int U, int S, int A, int TS):
    U(U),
    S(S),
    A(A),
    TS(TS),
    TSA(A,vector<bool>(TS,false)),
    TSS(S,vector<bool>(TS,false)),
    r(A,vector<int>(S,0)),
    HN(A,0),
    HE(A,0) { }
