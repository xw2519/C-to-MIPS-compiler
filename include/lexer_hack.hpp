#ifndef LEXHACK
#define LEXHACK

#include<string>
#include<vector>

using namespace std;

struct LexerContext
{
  vector<string> typeIdentifiers;
	bool InTypedef = false;
};

#endif
