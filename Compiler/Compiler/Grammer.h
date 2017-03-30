#pragma once

#include <iostream>
#include <cstdlib>

#include "NFA.h"

using namespace std;

class Grammer
{
public:
	virtual string read(string) = 0;
	eNFA* geteNFA();
	void init_state(eNFA* state);

	eNFA* parent = (eNFA*)malloc(sizeof(eNFA));
	eNFA* left = (eNFA*)malloc(sizeof(eNFA));
	eNFA* state = (eNFA*)malloc(sizeof(eNFA));
};