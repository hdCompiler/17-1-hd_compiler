#include "Bra.h"
#include "Grammer.h"


string Bra :: read(string s)
{
	init_state(parent);
	init_state(left);

	parent->state = state_num++;

	int i = 0;
	if (s[1] == '^')
	{
		left->isAccept = true;
		left->state = state_num++;
		parent->left = left;

		i = 3;
		int j = 0, k = 0;

		while (j<10)
		{
			if (j != (s.at(2) - '0'))
			{
				left->edgeNum[k] = j;
				k++;
			}
			j++;
		}
	}
	else
	{
		left->isAccept = true;
		left->state = state_num++;
		parent->left = left;

		i = 1;
		while (s.at(i) != ']')
		{
			left->edgeNum[i - 1] = s.at(i) - '0';
			i++;
		}
	}
	return s.substr(i + 1, s.length());
}
