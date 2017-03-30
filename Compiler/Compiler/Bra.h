#pragma once

#include "Grammer.h"

extern int state_num;

class Bra : public Grammer
{
public:
	virtual string read(string s);
};