#ifndef RLSAT_HCORAP_H
#define RLSAT_HCORAP_H
#include <vector>
using namespace std;
class HCORAP {
public:
    int U; // Number of users
    int S; // Number of services
    int A; // Number of agents
    int TS; // Number of timeslots
    int P; // Penalization of extra hour
    vector<vector<int>> SU; // Sets of services that cannot be performed at the same time
    vector<vector<int>> SEQ; // Sets of services ideally assigned to a same agent
    vector<vector<bool>> TSA; // Matrix of available working timeslots per agent (A x TS)
    vector<vector<bool>> TSS; // Matrix of suitable timeslots per service (S x TS)
    vector<vector<int>> r; // Matrix of reward (1,2,3,4) of assigning to agent i the service j, 0 meaning forbidden assignment (A x S)
    vector<int> HN; // Weekly working hours per agent
    vector<int> HE; // Allowed weekly extra working hours per agent

    void print();

    HCORAP(int U, int S, int A, int TS);
};


#endif //RLSAT_HCORAP_H
