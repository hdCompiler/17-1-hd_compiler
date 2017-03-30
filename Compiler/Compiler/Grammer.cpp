#include <iostream>

#include "Grammer.h"
#include "NFA.h"

using namespace std;

eNFA* Grammer :: geteNFA()
{
	return parent;
}
void Grammer :: init_state(eNFA* state)
{
	int i = 0;
	for (i = 0; i<10; i++)
		state->edgeNum[i] = -1;
	state->isAccept = false;
	state->left = nullptr;
	state->right = nullptr;
}
