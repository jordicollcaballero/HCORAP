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

    // y_a_s agent a is assigned to service s (at some time slot)
    vector<vector<literal>> y(instance->A, vector<literal>(instance->S));

    // s_a_seq agent a is assigned to some service of seq "seq"
    vector<vector<literal>> s(instance->A, vector<literal>(instance->SEQ.size()));

    // declaration of meaningful x variables (i.e. only considering allowed assignments)
    for(int i = 0; i < instance->A; i++)
        for(int j = 0; j < instance->S; j++)
            for(int k = 0; k < instance->TS; k++)
                x[i][j][k]=  ((instance->r[i][j]==0) || !(instance->TSA[i][k]) || !(instance->TSS[j][k]))
                        ? f->falseVar()
                        : f->newBoolVar("x",i,j,k);

    // declaration of meaningful y variables (i.e. only considering allowed assignments)
     for(int i = 0; i < instance->A; i++)
        for(int j = 0; j < instance->S; j++)
           y[i][j]= (instance->r[i][j]==0) ? f->falseVar() : f->newBoolVar("y",i,j);


     // declaration of meaningful reification variables s (i.e. not creating new vars for seqs of length 1)
    for(int i = 0; i < instance->A; i++)
        for(int q = 0; q < instance->SEQ.size(); q++)
            s[i][q] = (instance->SEQ[q].size()==1)
                    ? y[i][instance->SEQ[q][0]]
                    : f->newBoolVar("s",i,q);


    // 5) Reification of agents assignments to services (at some timeslot)
    //    y[i][j] <==> x[i][j][_]
    for(int i = 0; i < instance->A; i++){
        for(int j = 0; j < instance->S; j++){
            clause caux = !y[i][j];
            for(int k = 0; k < instance->TS; k++) {
                f->addClause(!x[i][j][k] | y[i][j]);
                caux |= x[i][j][k];
            }
            f->addClause(caux);
        }
    }




    //
   // 1) All services must be attended exactly by one agent and only at one timeslot
    for(int j = 0; j < instance->S; j++){
        vector<literal> vaux;
        for(int i = 0; i < instance->A; i++)
            vaux.insert(vaux.end(), x[i][j].begin(),  x[i][j].end());
        f->addEO(vaux);
    }


        // 2) At most one service per agent and timeslot

    for(int i = 0; i < instance->A; i++){
        for(int k = 0; k < instance->TS; k++) {
            vector<literal> vaux;
            for (int j = 0; j < instance->S; j++)
                vaux.push_back(x[i][j][k]);
            f->addAMO(vaux);
        }
    }


    // 3) Agents only perform services in their allowed timeslots

    // implicitly done in declaration of vars x

    // 4) At most one service per user and timeslot => (or are they timeslot disjoint?)

    // TODO

    // 6) Reification of agents assignments to services of seqs

    for(int i = 0; i < instance->A; i++){
        for(int q = 0; q < instance->SEQ.size(); q++) {
            if(instance->SEQ[q].size()!=1)  {
                clause caux = !s[i][q];
                for(int j:instance->SEQ[q]) {
                    f->addClause(s[i][q] | !y[i][j]);
                    caux |= y[i][j];
                }
                f->addClause(caux);
            }
        }
    }



    // 7) Consistency enforcement of assignemnts to SEQs using constraint networks and soft constraints

    // 8) Revenue of assignemnts of agents to services according to their expertise

    // 9) Adjustment of working hour per agents through sorting networks and soft constraints








}
