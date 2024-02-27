//
// Created by jordic on 06/02/24.
//

#include "RLSATEncoding.h"
#include <vector>
#include <fstream>

using namespace std;

RLSATEncoding::RLSATEncoding(RLSAT * instance, bool strat) : strat(strat), instance(instance){

}

SMTFormula *  RLSATEncoding::encode(int lb, int ub) {

    SMTFormula * f = new SMTFormula();

    totalSoft=0, konstantrevenue=0; // constant to deal with "extra" reward because the number of agents or services is limited.

    // x_a_s_h agent a is assigned to service s at timeslot h
    x=vector<vector<vector<literal>>>(instance->A,vector<vector<literal>>(instance->S,vector<literal>(instance->TS)));

    // y_a_s agent a is assigned to service s (at some time slot)
    y=vector<vector<literal>>(instance->A, vector<literal>(instance->S));

    // s_a_seq agent a is assigned to some service of seq "seq"
    s=vector<vector<literal>>(instance->A, vector<literal>(instance->SEQ.size()));

    // su_s_h service s is done at time h
    su=vector<vector<literal>>(instance->S, vector<literal>(instance->TS));


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


    // declaration of meaningful su variables (i.e. only considering allowed assignments)
    for(int j = 0; j < instance->S; j++)
        for(int k = 0; k < instance->TS; k++)
            su[j][k]= (instance->TSS[j][k]) ? f->newBoolVar("su",j,k) : f->falseVar();


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
                if(x[i][j][k].v.id!=f->falseVar().id) {
                    f->addClause(!x[i][j][k] | y[i][j]);
                    caux |= x[i][j][k];
                }
            }
            if(y[i][j].v.id!=f->falseVar().id)
                f->addClause(caux);
        }
    }


    // 5b) Reification of time in which is done each service
    //    su[j][k] <==> x[_][j][k]

    for(int j = 0; j < instance->S; j++){
        for(int k = 0; k < instance->TS; k++) {
            clause caux = !su[j][k];
            for(int i = 0; i < instance->A; i++){
                if(x[i][j][k].v.id!=f->falseVar().id) {
                    f->addClause(!x[i][j][k] | su[j][k]);
                    caux |= x[i][j][k];
                }
            }
            if(su[j][k].v.id!=f->falseVar().id)
                f->addClause(caux);
        }
    }

    //
   // 1) All services must be attended by most one agent and at most at one timeslot
    for(int j = 0; j < instance->S; j++){
        vector<literal> vaux;
        for(int i = 0; i < instance->A; i++)
            for(int k = 0; k < x[i][j].size(); k++)
                if(x[i][j][k].v.id!=f->falseVar().id)
                    vaux.push_back(x[i][j][k]);
        f->addAMO(vaux);
    }


        // 2) At most one service per agent and timeslot

    for(int i = 0; i < instance->A; i++){
        for(int k = 0; k < instance->TS; k++) {
            vector<literal> vaux;
            for (int j = 0; j < instance->S; j++)
                if(x[i][j][k].v.id!=f->falseVar().id)
                    vaux.push_back(x[i][j][k]);
            f->addAMO(vaux);
        }
    }

    // 3) Agents only perform services in their allowed timeslots

    // implicitly done in declaration of vars x

    // 4) At most one service per user and timeslot

    for(const vector<int> & v: instance->SU){
        for(int k = 0; k < instance->TS; k++) {
            vector<literal> vaux;
            for (int j : v)
                if(su[j][k].v.id!=f->falseVar().id)
                    vaux.push_back(su[j][k]);
            f->addAMO(vaux);
        }
    }
    // 6) Reification of agents assignments to services of seqs

    for(int i = 0; i < instance->A; i++){
        for(int q = 0; q < instance->SEQ.size(); q++) {
            if(instance->SEQ[q].size()!=1)  {
                clause caux = !s[i][q];
                for(int j:instance->SEQ[q]) {
                    if(y[i][j].v.id!=f->falseVar().id) {
                        f->addClause(s[i][q] | !y[i][j]);
                        caux |= y[i][j];
                    }
                }
                if(s[i][q].v.id!=f->falseVar().id)
                    f->addClause(caux);
            }
        }
    }


    // 7) Consistency enforcement of assignemnts to SEQs using constraint networks and soft constraints


    for(int q = 0; q < instance->SEQ.size(); q++) {
        if(instance->SEQ[q].size()!=1){
            vector<literal> v;
            for(int i = 0; i< instance->A; i++){
                v.push_back(s[i][q]);
            }
            vector<literal> vout;
            f->addSorting(v,vout,true,true);
            int p = min(instance->A,(int)instance->SEQ[q].size());
            for(int i = 0; i < p; i++){
                f->addSoftClause(!vout[i]);
                totalSoft++;
            }
            konstantrevenue += instance->SEQ[q].size() - p;
        }
    }



    // 8) Revenue of assignemnts of agents to services according to their expertise

    for(int i = 0; i < instance->A; i++)
        for(int j = 0; j < instance->S; j++)
           if(instance->r[i][j]!=0) {
               f->addSoftClause(y[i][j], instance->r[i][j]);
               totalSoft+=instance->r[i][j];
           }


    // 9) Adjustment of working hour per agents through sorting networks and soft constraints

    for(int i = 0; i < instance->A; i++) {
        vector<literal> v; // filtered input variables for the sorted
        for(int j = 0; j < instance->S; j++){
            if(y[i][j].v.id!=f->falseVar().id)
                v.push_back(y[i][j]);
        }
        if(v.size()>instance->HN[i]){ // only deal with agents that can work more than their HN
            vector<literal> vout;
            int maxhours = instance->HN[i]+instance->HE[i];
            f->addSorting(v,vout,true,true);
            if(v.size()>maxhours) {
                f->addClause(!vout[maxhours]);
            }
            int p = min(maxhours,(int)vout.size());
            for(int k = instance->HN[i]; k < p; k++){
                f->addSoftClause(!vout[k],-instance->P);
                totalSoft+=-instance->P;
            }
            konstantrevenue -= p - instance->HN[i] ;
        }
    }


    // 1.2) All services must be attended at some time slot
    for(int j = 0; j < instance->S; j++){
        vector<literal> v;
        for(int k=0; k < instance->TS; k++)
            if(su[j][k].v.id!=f->falseVar().id)
                v.push_back(su[j][k]);
        if(strat)
            f->addSoftClause(v,totalSoft+1);
        else
            f->addClause(v);
    }

    cout << "c " << totalSoft << " " << konstantrevenue << endl;

    return f;

}

void RLSATEncoding::checkSolution(const string & filename){
    vector<bool> model;

    ifstream ifs;
    ifs.open(filename);
    string auxstr;
    int optimal=-1;
    bool solved=false;
    char c;
    do { ifs >> auxstr; } while (!ifs.eof() && auxstr != "s" && auxstr != "o" && auxstr != "v");
    while(!ifs.eof()) {
        if(auxstr == "s"){
            ifs >> auxstr;
            if(auxstr=="UNSATISFIABLE"){
                cout << "SKIP: UNSATISFIABLE"  << endl;
                exit(0);
            }
            else if(auxstr=="OPTIMUM")
                solved=true;
        }
        else if (auxstr=="o"){
            ifs >> optimal;
        }
        else{
            model.clear();
            model.push_back(false); //Start indexing at 1
            ifs >> c;
            ifs >> c;
            while(c=='1' || c=='0'){
                model.push_back(c=='1');
                ifs >> c;
            }
        }

        do { ifs >> auxstr; } while (!ifs.eof() && auxstr != "s" && auxstr != "o" && auxstr != "v");
    }
    if(!solved){
        cout << "SKIP: NOT SOLVED"  << endl;
        exit(0);
    }
    if(optimal<0){
        cout << "ERROR: optimal not read" << endl;
        exit(0);
    }
    if(model.empty()){
        cout << "ERROR: model not read" << endl;
        exit(0);
    }

    int checkOptimal=0;
    int nNotDone=0;

    // x_a_s_h agent a is assigned to service s at timeslot h
    // y_a_s agent a is assigned to service s (at some time slot)
    // s_a_seq agent a is assigned to some service of seq "seq"
    // su_s_h service s is done at time h

    for(int j = 0; j < instance->S; j++){
        int nDone=0;
        for(int i=0; i < instance->A; i++){
            for(int k=0; k < instance->TS; k++){
                if(model[x[i][j][k].v.id])
                    nDone++;
            }
        }
        cout << "==> Check service " << j << " done at most once:" << (nDone<=1 ? "OK" : "FAIL") << endl;
        if(nDone==0)
            nNotDone++;
    }

    bool ok=true;
    for(const vector<int> & v: instance->SU){
        for(int k = 0; k < instance->TS; k++) {
            int nDone=0;
            for (int j : v) {
                for(int i = 0; i < instance->A; i++) {
                    if(model[x[i][j][k].v.id])
                        nDone++;
                }
            }
            ok=ok&&nDone<=1;
        }
    }
    cout << "==> Check all users at most one service at a time" << (ok ? "OK" : "FAIL") << endl;

    for(int i=0; i < instance->A; i++) {
        bool ok=true;
        int nWork=0;
        for (int k = 0; k < instance->TS; k++) {
            int nDone = 0;
            for (int j = 0; j < instance->S; j++) {
                if (model[x[j][i][k].v.id]) {
                    nDone++;
                    nWork++;
                }
            }
            ok=ok&&nDone<=1;
        }
        cout << "==> Agent " << i << " at most one service at a time:" << (ok ? "OK" : "FAIL") << endl;
        cout << "==> Agent " << i << " does not exceed his maximum working hours: " << (nWork<=instance->HN[i]+instance->HE[i] ? "OK" : "FAIL_H") << endl;
        if(nWork>instance->HN[i])
            checkOptimal+=(nWork-instance->HN[i])*instance->P;
    }


    for(int j = 0; j < instance->S; j++){
        bool ok=true;
        for(int i=0; i < instance->A; i++){
            for(int k=0; k < instance->TS; k++){
                if(model[x[i][j][k].v.id]) {
                    ok = ok && instance->r[i][j] > 0;
                    checkOptimal+= instance->r[i][j];
                }
            }
        }
        cout << "==> Check service " << j << " done by skilled agent:" << (ok ? "OK" : "FAIL") << endl;
    }



    for(int q = 0; q < instance->SEQ.size(); q++) {
        int nWorking=0;
        for(int i = 0; i< instance->A; i++){
            if(model[s[i][q].v.id]){
                nWorking++;
            }
        }
        checkOptimal+=instance->SEQ[q].size()-nWorking;
        cout << "==> Check not too many agents woring in SEQ " << q << ":" << (nWorking<=instance->SEQ[q].size() ? "OK" : "FAIL") << endl;
    }
    int optimalMax = totalSoft - (optimal%(totalSoft+1)) + konstantrevenue;
    cout << "==> Check optimal :" << (optimalMax==checkOptimal?"OK":"FAIL_H") << endl;
    cout << "N Undone services: " << nNotDone << endl;
    cout << "Cost: " << optimalMax << endl;

}
