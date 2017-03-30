#pragma once
#include <iostream>
#include <vector>

#include "NFA.h"

using namespace std;

extern int state_num;
extern void invalid();

typedef struct DFAs
{
	vector<int> curState;
	vector<int> outState[10];

	bool isAccept = false;
}DFA;

//function for eclose
void Eclose(eNFA* enfa);
void Eclose_final();
bool dupliCheck(vector<int> test, int elem); //각 epsilon vector에서 중복된 값이 있는지 check 해서 제거 
bool dupliCheck(vector<DFA> test, vector<int>* state);
vector<int> EcloseCal(eNFA* enfaSub);        //각 node에서 left, right의 edge가 epsilon인지 검사
void NFATran(int** NfaTable, int height, int width, eNFA* enfa);

// nfa2dfa
vector<DFA> NFA2DFA(int** nfaTrans);
vector<int> DFA_input(int** nfaTable, vector<int> curState, int input);

// dfa string valid check
void findAcceptState(eNFA* enfa, int* state);
vector<DFA> acceptSet(vector<DFA> dfa, int state);
vector<int> findState(vector<DFA> dfa, vector<int> state, char c);