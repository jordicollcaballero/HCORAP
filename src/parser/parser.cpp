#include "parser.h"
#include "errors.h"
#include "util.h"


using namespace std;

namespace parser
{

    HCORAP * parseHCORAP(const string & filename){
        ifstream f;
        f.open(filename.c_str());
        if (!f.is_open()) {
            cerr << "Could not open file " << filename << endl;
            exit(BADFILE_ERROR);
        }
        string aux;
        int auxint;
        string line_s;

        int U;
        int S;
        int A;
        int TS;
        f >> aux >> U;
        f >> aux >> S;
        f >> aux >> A;
        f >> aux >> TS;
        f >> aux;
        getline(f,line_s);

        HCORAP *rlsat = new HCORAP(U, S, A, TS);

        getline(f,line_s);
        while(line_s!="#SEQ"){
            if(line_s.empty()) {
                getline(f,line_s);
                continue;
            }
            rlsat->SU.push_back(vector<int>());
            stringstream str(line_s);
            int service;
            while(str>>service)
                rlsat->SU.back().push_back(service);
            getline(f,line_s);
        }

        getline(f,line_s);
        while(line_s!="#TSA(i)"){
            rlsat->SEQ.push_back(vector<int>());
            stringstream str(line_s);
            int service;
            while(str>>service)
                rlsat->SEQ.back().push_back(service);
            getline(f,line_s);
        }

        for(int i = 0; i < A; i++)
            for(int j = 0; j < TS; j++) {
                f >> auxint;
                rlsat->TSA[i][j]=auxint==1;
            }
        f >> aux;
        for(int i = 0; i < S; i++)
            for(int j = 0; j < TS; j++) {
                f >> auxint;
                rlsat->TSS[i][j]=auxint==1;
            }
        f >> aux;
        for(int i = 0; i < A; i++)
            for(int j = 0; j < S; j++)
                f >> rlsat->r[i][j];
        f>>aux;
        f>>rlsat->P;
        f >> aux;
        for(int i = 0; i < A; i++)
            f >> rlsat->HN[i];

        f >> aux;
        for(int i = 0; i < A; i++)
            f >> rlsat->HE[i];

        return rlsat;
    }


















void findChar(istream & str, char c){
	char aux;
	do{
		str >> aux;
	}while(aux!=c && !str.eof());
}

int readAssingnment(istream & str){
	findChar(str,':');
	int i;
	str >> i;
	return i;
}
}


