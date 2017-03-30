#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "NFA.h"
#include "DFA.h"
#include "Grammer.h"
#include "Bra.h"
#include "Pa.h"

// namespace
using namespace std;

// global variable
int state_num = 0;

void invalid()
{
	cout << "This string is invalid." << endl;
}

int main()
{
	string RE;
	eNFA* eNFA;

	while (true)
	{
		cout << "Input Regular Expression :" << endl;
		getline(cin, RE);

		// RE -> eNFA
		if (isValidRE(RE) == false)
		{
			cout << "inValid input." << endl;
		}
		else
		{
			cout << "valid RE" << endl;
			eNFA = RE2eNFA(RE);
			break;
		}
	}
	// eNFA -> DFA
	//input이 epsilon일 때는 eclose 구하지 않는다.
	if (state_num != 1)
	{
		Eclose(eNFA);
		Eclose_final();
	}

	int width = 10;
	int height = state_num;
	int** NfaTable = (int**)malloc(sizeof(int*) * height);
	NfaTable = (int **)malloc(sizeof(int *) * height);            //NFA transition 결과 담은 배열
	NfaTable[0] = (int *)malloc(sizeof(int) * width*height);

	for (int i = 1; i < height; i++) {
		NfaTable[i] = NfaTable[i - 1] + width;
	}

	NFATran(NfaTable, height, width, eNFA);

	vector<DFA> DFA_table;
	DFA_table = NFA2DFA(NfaTable);

	// accepting set
	int accept =0;
	findAcceptState(eNFA, &accept);
	DFA_table = acceptSet(DFA_table, accept);

	// loop
	while (1)
	{
		string S;
		bool isAccept = false;

		cout << "Input String : " << endl;
		getline(cin, S);

		// input no!
		if (S == "")
			invalid();

		// valid check
		vector<int> state;
		
		for (int i = 0; i < S.length(); i++)
		{
			state = findState(DFA_table, state, S[i]);
			if (state.empty())
				break;
		}
		if (!state.empty())
		{
			sort(state.begin(), state.end());

			for (int i = 0; i < DFA_table.size(); i++)
			{
				sort(DFA_table[i].curState.begin(), DFA_table[i].curState.end());
				if (DFA_table[i].curState == state)
				{
					if (DFA_table[i].isAccept)
						isAccept = true;
				}
			}
			if (!isAccept)
				invalid();
			else
				cout << "This string is valid input" << endl;
		}
	}

	free(NfaTable[0]);
	free(NfaTable);

	return 0;
}