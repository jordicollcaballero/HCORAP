#ifndef PARSER_DEFINITION
#define PARSER_DEFINITION

#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "hcorap.h"

using namespace std;

namespace parser
{

HCORAP * parseHCORAP(const string & filename);

void findChar(istream & str, char c);
int readAssingnment(istream & str);

}

#endif

