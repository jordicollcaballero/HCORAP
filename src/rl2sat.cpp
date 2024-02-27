#include "parser.h"
#include "basiccontroller.h"
#include "RLSATEncoding.h"
#include "solvingarguments.h"


using namespace std;
using namespace util;

enum ProgramArg {
    STRATIFIED
};


int main(int argc, char **argv) {

    Arguments<ProgramArg> * pargs
            = new Arguments<ProgramArg>(
                    //Program arguments
                    {
                            arguments::arg("filename","Instance file path."),
                            arguments::arg("solutionname","Solution file path.")
                    },1,{
                    arguments::bop("S","strat",STRATIFIED,false,
                                           "If true, allow to not do some services (soft constraint). Default: false.")
                    },"Solve the resource location home care problem.");

    SolvingArguments * sargs = SolvingArguments::readArguments(argc,argv,pargs);

    RLSAT * instance = parser::parseRLSAT(pargs->getArgument(0));
    RLSATEncoding * encoding = new RLSATEncoding(instance, pargs->getBoolOption(STRATIFIED));
    if(pargs->getNArguments()==1) {
        BasicController c(sargs, encoding, false, 0, INT_MAX);
        c.run();
    }
    else
        encoding->checkSolution(pargs->getArgument(1));

    return 0;
}
