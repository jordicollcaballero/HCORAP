#include "parser.h"
#include "basiccontroller.h"
#include "RLSATEncoding.h"
#include "solvingarguments.h"


using namespace std;
using namespace util;


int main(int argc, char **argv) {

    Arguments<int> * pargs
            = new Arguments<int>(
                    //Program arguments
                    {
                            arguments::arg("filename","Instance file path.")
                    },1,{},"Solve the resource location home care problem.");

    SolvingArguments * sargs = SolvingArguments::readArguments(argc,argv,pargs);

    RLSAT * instance = parser::parseRLSAT(pargs->getArgument(0));
    RLSATEncoding * encoding = new RLSATEncoding(instance);
    BasicController c(sargs,encoding,false,0,INT_MAX);
    c.run();

    return 0;
}
