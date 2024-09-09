//
// Created by jordic on 06/02/24.
//

#include "HCORAPNServicesEncoding.h"
#include <vector>
#include <fstream>

using namespace std;

HCORAPNServicesEncoding::HCORAPNServicesEncoding(HCORAP * instance) : instance(instance){

}

SMTFormula *  HCORAPNServicesEncoding::encode(int lb, int ub) {

    SMTFormula * f = new SMTFormula();


    // x_a_s_h agent a is assigned to service s at timeslot h
    x=vector<vector<vector<literal>>>(instance->A,vector<vector<literal>>(instance->S,vector<literal>(instance->TS)));

    // y_a_s agent a is assigned to service s (at some time slot)
    y=vector<vector<literal>>(instance->A, vector<literal>(instance->S));


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
            for(int k = 0; k < instance->TS; k++)
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



    // 9) Adjustment of working hour per agents through sorting networks and soft constraints

    for(int i = 0; i < instance->A; i++) {
        int maxhours = instance->HN[i]+instance->HE[i];
        vector<literal> v; // filtered input variables for the sorted
        for(int j = 0; j < instance->S; j++){
            if(y[i][j].v.id!=f->falseVar().id)
                v.push_back(y[i][j]);
        }
        if(v.size()>maxhours){ // only deal with agents that can work more than their HN
            vector<literal> vout;
            f->addSorting(v,vout,true,true);
            f->addClause(!vout[maxhours]);
        }
    }

    totalSoftStrat=0;

    // 1.2) All services must be attended at some time slot
    for(int j = 0; j < instance->S; j++){
        vector<literal> v;
        for(int k=0; k < instance->TS; k++)
            if(su[j][k].v.id!=f->falseVar().id)
                v.push_back(su[j][k]);
        f->addSoftClause(v);
        softClauses.push_back(v);
        weights.push_back(1);
        totalSoftStrat+=1;
    }

    return f;

}

void HCORAPNServicesEncoding::checkSolution(const string & filename){

    int sim=0, con=0, cost=0, con2=0;

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
                ifs.close();
                cout << "SKIP: UNSATISFIABLE"  << endl;
                exit(0);
            }
            else if(auxstr=="OPTIMUM")
                solved=true;
        }
        else if (auxstr=="o"){
            ifs >> auxstr;
            if(auxstr!="18446744073709551615") //Evalmaxsat sometimes reports this unsigned long max o line
                optimal=std::stoi(auxstr);
        }
        else{
            model.clear();
            model.push_back(false); //Start indexing at 1
            ifs >> c;
            while(c=='1' || c=='0'){
                model.push_back(c=='1');
                ifs >> c;
            }
        }

        do { ifs >> auxstr; } while (!ifs.eof() && auxstr != "s" && auxstr != "o" && auxstr != "v");
    }
    ifs.close();
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
    encode();

    for(int j = 0; j < instance->S; j++){
        int nDone=0;
        for(int i=0; i < instance->A; i++){
            for(int k=0; k < instance->TS; k++){
                if(model[x[i][j][k].v.id])
                    nDone++;
            }
        }
        cout << "==> Check service " << j << " done at most once:" << (nDone<=1 ? "OK" : "FAIL") << endl;
        if(nDone==0) {
            nNotDone++;
            cout << "==> Service " << j << " not done." << endl;
        }
        else
            checkOptimal++;
    }

    bool ok=true;
    for(const vector<int> & v: instance->SU){
        for(int k = 0; k < instance->TS; k++) {
            int nDone=0;
            for (int j : v) {
                for(int i = 0; i < instance->A; i++) {
                    if(model[x[i][j][k].v.id]) {
                        nDone++;
                    }
                }
            }
            ok=ok&&nDone<=1;
        }
    }
    cout << "==> Check all users at most one service at a time:" << (ok ? "OK" : "FAIL") << endl;

    for(int i=0; i < instance->A; i++) {
        bool ok=true;
        int nWork=0;
        for (int k = 0; k < instance->TS; k++) {
            int nDone = 0;
            for (int j = 0; j < instance->S; j++) {
                if (model[x[i][j][k].v.id]) {
                    nDone++;
                    nWork++;
                }
            }
            ok=ok&&nDone<=1;
        }
        cout << "==> Agent " << i << " at most one service at a time:" << (ok ? "OK" : "FAIL") << endl;
        cout << "==> Agent " << i << " does not exceed his maximum working hours: " << (nWork<=instance->HN[i]+instance->HE[i] ? "OK" : "FAIL") << endl;
    }


    for(int j = 0; j < instance->S; j++){
        bool ok=true;
        for(int i=0; i < instance->A; i++){
            for(int k=0; k < instance->TS; k++){
                if(model[x[i][j][k].v.id]) {
                    ok = ok && instance->r[i][j] > 0;
                }
            }
        }
        cout << "==> Check service " << j << " done by skilled agent:" << (ok ? "OK" : "FAIL") << endl;
    }


    int solverSol = checkAllSoftClauses();
    if(solverSol==optimal) {
        cout << "==> Check solver solution: " << "OK" << endl;
    }
    else{
        cout << "==> Check solver solution: FAIL solver=" << optimal << " check=" << solverSol  << endl;
    }

    int optimalMax =  totalSoftStrat - optimal;
    if(optimalMax==checkOptimal)
        cout << "==> Check optimal: " << "OK" << endl;
    else
        cout << "==> Check optimal: " << "FAIL " << optimalMax << " " << checkOptimal << " " << optimal << endl;
    cout << "N Undone services: " << nNotDone << endl;
}


int HCORAPNServicesEncoding::checkAllSoftClauses(){
    int cost = 0;
    for(int i = 0; i < softClauses.size(); i++){
        bool sat=false;
        for(literal l : softClauses[i].v){
            if(l.sign==model[l.v.id]) {
                sat = true;
                break;
            }
        }
        if(!sat)
            cost+=weights[i];
    }
    return cost;
}