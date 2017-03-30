#include <iostream>
#include <vector>
#include <algorithm>

#include "DFA.h"
#include "NFA.h"

using namespace std;

// eclose vector
vector<vector<int> > ecloses(state_num, vector<int>(state_num));
vector<vector<int>> temp;      //final eclosure

void Eclose(eNFA* enfa)
{
	static int num = 0;

	eNFA* temp_l = enfa;
	eNFA* temp_r = enfa;
	static int flag[9999];
	if (enfa != nullptr)
	{
		if (flag[enfa->state] != 1)
		{
			flag[enfa->state] = 1;
			if (enfa->left != nullptr)
			{
				Eclose(enfa->left);
			}
			if (enfa->right != nullptr)
			{
				Eclose(enfa->right);
			}
			ecloses.push_back(EcloseCal(enfa));
		}
	}
}

vector<int> EcloseCal(eNFA* enfaSub)
{
	vector<int> eclose_elem;
	if (enfaSub != nullptr)
	{
		eclose_elem.push_back(enfaSub->state);
		if (enfaSub->left != nullptr)
		{
			if (enfaSub->left->edgeNum[0] == -1)
			{
				if (!dupliCheck(eclose_elem, enfaSub->left->state))
					eclose_elem.push_back(enfaSub->left->state);
			}
		}
		if (enfaSub->right != nullptr)
		{
			if (enfaSub->right->edgeNum[0] == -1)
			{
				if (!dupliCheck(eclose_elem, enfaSub->right->state))
					eclose_elem.push_back(enfaSub->right->state);
			}
		}
	}
	return eclose_elem;
}

void Eclose_final()
{
	int order = 0;
	int i = 0;

	while (temp.size() != ecloses.size())
	{
		if (order == ecloses[i%ecloses.size()][0])
		{
			temp.push_back(ecloses[i%ecloses.size()]);
			order++;
		}
		i++;
	}
	
	int j, k = 0;
	
	for (int i = 0; i < temp.size(); i++)
	{
		for (j = 1; j < temp[i].size(); j++)
		{
			int num = temp[i][j];           

			for (k = 1; k < temp[num].size(); k++)
			{
				if (!dupliCheck(temp[i], temp[num][k]))
					temp[i].push_back(temp[num][k]);
			}

		}
	}
}

//vector�� push�ϱ� ���� element�� vector�� �̹� �ִ��� Ȯ���ϴ� �Լ�
bool dupliCheck(vector<int> test, int elem)
{
	for (int i = 0; i < test.size(); i++)
	{
		if (test[i] == elem)
			return true;
	}
	return false;
}

void NFATran(int** NfaTable, int height, int width, eNFA* enfa)
{
	static int num = 0;

	eNFA* temp_l = enfa;
	eNFA* temp_r = enfa;
	static int flag[9999];

	if (num == 0)
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
				NfaTable[i][j] = 999;         //999�� �ʱ�ȭ
		}
		num++;
	}

	if (enfa != nullptr)
	{
		if (flag[enfa->state] != 1)
		{
			flag[enfa->state] = 1;
			if (enfa->left != nullptr)
			{
				NFATran(NfaTable, height, width, enfa->left);
			}
			if (enfa->right != nullptr)
			{
				NFATran(NfaTable, height, width, enfa->right);
			}

			//NfaTable
			if (enfa->left != nullptr)
			{
				//���� left state�� edge num �� Ȯ�� -> -1�� �ƴѰ��
				if (enfa->left->edgeNum[0] == -1)
				{
					//(NULL ���� �� ���ִ��� Ȯ���ϱ�)
				}
				else
				{
					int j = 0;
					while (enfa->left->edgeNum[j] != -1)
					{
						NfaTable[enfa->state][enfa->left->edgeNum[j]] = enfa->left->state;
						j++;
					}
				}
			}

			if (enfa->right != nullptr)
			{
				if (enfa->right->edgeNum[0] == -1)
				{
					;
				}
				else
				{
					int j = 0;
					while (enfa->right->edgeNum[j] != -1)
					{
						NfaTable[enfa->state][enfa->right->edgeNum[j]] = enfa->right->state;
						j++;
					}
				}

			}
		}
	}
}

vector<DFA> NFA2DFA(int** nfaTable)
{
	vector<DFA> dfa;
	vector<int> outState;
	DFA DFAtemp;
	int num = 0;
	int tupleNum = 1;
	DFAtemp.curState = temp[0];
	while (num<tupleNum * 10)
	{
		for (int j = 0; j < 10; j++)
		{
			DFAtemp.outState[j] = DFA_input(nfaTable, DFAtemp.curState, j);
		}
		dfa.push_back(DFAtemp);
		//�ߺ��� state�� �ִ��� �˻�
		while (num < tupleNum * 10 && (dfa[num / 10].outState[num % 10][0] == 999 || dupliCheck(dfa, &dfa[num / 10].outState[num % 10])))
		{
			num++;
		}
		if (num >= tupleNum * 10)
		{
			break;
		}
		if (!dupliCheck(dfa, &dfa[num / 10].outState[num % 10]))
		{
			DFAtemp.curState = dfa[num / 10].outState[num % 10];
			tupleNum++;
			//�� ������ ��ĥ ������ 10���� �þ��.
		}
		else
		{
			//�ߺ��Ǹ�
		}
		num++;
	}
	//vector�� �ִ´�
	//start state�� ���� input �� ���� ����� ���� ���Ѵ� 
	//����� �����Ѵ�
	return dfa;
}

vector<int> DFA_input(int** nfaTable, vector<int> curState, int input)
{
	vector<int> eTemp;

	for (int i = 0; i < curState.size(); i++)
	{
		//nfaTable[i][j] (i)state���� (j)input�� ���� �� state
		int elemCurState = curState[i];         //current state ���ϴ� �ϳ��� state
		int nfaResult = nfaTable[elemCurState][input];      //nfa transition function�� ���� ���
		if (nfaResult != 999)
		{
			//curstate ������ �ϳ��� ������ NFA transition ����� ���� eclosure�� ���Ѵ�
			for (int i = 0; i < temp[nfaResult].size(); i++)
			{
				if (!dupliCheck(eTemp, temp[nfaResult][i]))
					eTemp.push_back(temp[nfaResult][i]);
			}
		}
	}
	if (eTemp.size() < 1)
		eTemp.push_back(999);

	return eTemp;
}

bool dupliCheck(vector<DFA> dfa, vector<int>* elem)
{
	sort(elem->begin(), elem->end());

	//sorting �� ��
	for (int i = 0; i< dfa.size(); i++)
	{
		sort(dfa[i].curState.begin(), dfa[i].curState.end());
		if (dfa[i].curState == *elem)
			return true;
	}
	return false;
}

void findAcceptState(eNFA* enfa, int* state)
{
	if (enfa != nullptr && *state == 0)
	{
		if (enfa->isAccept == true)
		{
			*state = enfa->state;
		}

		findAcceptState(enfa->left, state);
		findAcceptState(enfa->right, state);
	}
}

vector<DFA> acceptSet(vector<DFA> dfa, int state)
{
	for (int i = 0; i < dfa.size(); i++)
	{
		for (int j = 0; j < dfa[i].curState.size(); j++)
		{
			if (dfa[i].curState[j] == state)
				dfa[i].isAccept = true;
		}
	}
	return dfa;
}

vector<int> findState(vector<DFA> dfa, vector<int> state, char c)
{
	vector<int> result;
	int num = (int)c - 48;

	if (num < 0 || num > 9)
	{
		result.clear();
		invalid();
	}
	else
	{
		if (state.empty())
			result = dfa[0].outState[num];
		else
		{
			sort(state.begin(), state.end());

			for (int i = 0; i < dfa.size(); i++)
			{
				sort(dfa[i].curState.begin(), dfa[i].curState.end());
				if (dfa[i].curState == state)
					result = dfa[i].outState[num];
			}
		}
		if (result.empty())
		{
			if (state.at(0) == 999)
				result = state;
			else
				invalid();
		}
	}
	return result;
}
