//
// Created by jordic on 06/02/24.
//

#include "hcorap.h"
#include <iostream>

using namespace std;

HCORAP::HCORAP(int U, int S, int A, int TS):
    U(U),
    S(S),
    A(A),
    TS(TS),
    TSA(A,vector<bool>(TS,false)),
    TSS(S,vector<bool>(TS,false)),
    r(A,vector<int>(S,0)),
    HN(A,0),
    HE(A,0) { }

void HCORAP::print() {
    cout << "#U" << endl << U << endl;
    cout << "#S" << endl << S << endl;
    cout << "#A" << endl << A << endl;
    cout << "#TS" << endl << TS << endl;
    cout << "#SU" << SU.size() << endl;
    for(const vector<int> & v : SU){
        for(int i : v)
            cout << i << " ";
        cout << endl;
    }
    cout << "#SEQ" << endl;
    for(const vector<int> & v : SEQ){
        for(int i : v)
            cout << i << " ";
        cout << endl;
    }
    cout << "#TSA" << endl;
    for(const vector<bool> & v : TSA){
        for(bool i : v)
            cout << (i? 1 : 0) << " ";
        cout << endl;
    }
    cout << "#TSS" << endl;
    for(const vector<bool> & v : TSS){
        for(bool i : v)
            cout << (i? 1 : 0) << " ";
        cout << endl;
    }
    cout << "#r" << endl;
    for(const vector<int> & v : r){
        for(int i : v)
            cout << i << " ";
        cout << endl;
    }
    cout << "#P" << P << endl;
    cout << "#HN" << endl;
    for(int i : HN)
        cout << i << endl;

    cout << "#HE" << endl;
    for(int i : HE)
        cout << i << endl;
}