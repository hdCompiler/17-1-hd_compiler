#pragma once

#include "Grammer.h"
#include "NFA.h"

extern int state_num;

class Pa : public Grammer
{
public:
	virtual string read(string s);
	void find_accept(eNFA* root);


};