#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
// namespace
using namespace std;

// struct
typedef struct eNFAs
{
	int state;
	int edgeNum[10];
	bool isAccept;
	struct eNFAs* left;
	struct eNFAs* right;
}eNFA;

typedef struct DFAs
{
	vector<int> curState;
	vector<int> outState[10];

	bool isAccept = false;
}DFA;

// global variable
extern int state_num = 0;

// eclose vector
vector<vector<int> > ecloses(state_num, vector<int>(state_num));
vector<vector<int>> temp;      //final eclosure
vector<vector<int>> NfaTable;

// function declare
eNFA* RE2eNFA(string S);
bool isValidRE(std::string str);

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
void invalid();

// class declare
class Pa;
class Bra;

// class
class Grammer
{
public:
	virtual string read(string) = 0;
	eNFA* geteNFA()
	{
		return parent;
	}
	void init_state(eNFA* state)
	{
		int i = 0;
		for (i = 0; i<10; i++)
			state->edgeNum[i] = -1;
		state->isAccept = false;
		state->left = nullptr;
		state->right = nullptr;
	}
protected:
	eNFA* parent = (eNFA*)malloc(sizeof(eNFA));
	eNFA* left = (eNFA*)malloc(sizeof(eNFA));
	eNFA* state = (eNFA*)malloc(sizeof(eNFA));
};

class Bra : public Grammer      // brakets
{
public:
	virtual string read(string s)
	{
		// cout << "in Bra [ :: " << endl;

		init_state(parent);
		init_state(left);

		parent->state = state_num++;

		int i = 0;
		if (s[1] == '^')
		{
			// cout << "in Bra [^] :: " << endl;

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
			// cout << "in Bra [] :: " << endl;

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
};

class Pa : public Grammer      // parentheses
{
public:
	void find_accept(eNFA* root)
	{
		if (root != nullptr)
		{
			if (root->isAccept)
			{
				root->isAccept = false;
				state = root;
			}
			else
			{
				find_accept(root->left);
				if (root->left != nullptr && root->right != nullptr)
					return;
				else
					find_accept(root->right);
			}
		}
	}

	virtual string read(string s)
	{
		init_state(parent);
		init_state(left);
		init_state(state);

		s = s.substr(1, s.length());

#pragma region "([])"
		if (s[0] == '[')
		{
			// cout << "start ([" << endl;

			// parent init
			parent->state = state_num++;

			Bra b;
			s = b.read(s);

			if (s[0] == ')')
			{
				// cout << "start ([])*" << endl;

				left->isAccept = true;
				left->state = state_num++;
				parent->left = left;

				parent->right = b.geteNFA();
				find_accept(parent->right);

				state->left = left;
				state->right = parent->right;

				return s.substr(2, s.length());
			}
#pragma region "([]|)"
			else if (s[0] == '|')
			{
				// cout << "start ([]I" << endl;

				parent->left = b.geteNFA();

				left->isAccept = true;
				left->state = state_num++;

				find_accept(parent->left);
				//
				state->left = left;

				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					// cout << "start ([]|[]) " << endl;

					Bra b;
					s = b.read(s);
					parent->right = b.geteNFA();
					find_accept(parent->right);

					state->left = left;

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					// cout << "start ([]|()) " << endl;

					Pa p;
					s = p.read(s);
					parent->right = p.geteNFA();
					find_accept(parent->right);

					state->left = left;

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// cout << "start ([]|e) " << endl;

					parent->right = left;

					return s.substr(2, s.length());
				}
			}
#pragma endregion

#pragma region "([].)"
			else if (s[0] == '.')
			{
				// cout << "start ([]." << endl;

				parent->left = b.geteNFA();

				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					// cout << "start ([].[])" << endl;

					Bra b;
					s = b.read(s);
					find_accept(parent->left);
					state->left = b.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					// cout << "start ([].())" << endl;

					Pa p;
					s = p.read(s);
					find_accept(parent->left);
					state->left = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// cout << "start ([].e)" << endl;

					left->isAccept = true;
					left->state = state_num++;

					find_accept(parent->left);
					state->left = left;

					return s.substr(2, s.length());
				}
			}
		}
#pragma endregion

#pragma endregion

#pragma region "(())"
		else if (s[0] == '(')
		{
			// cout << "start ((" << endl;

			parent->state = state_num++;

			Pa p;
			s = p.read(s);

			if (s[0] == ')')
			{
				// cout << "start (())*" << endl;

				left->isAccept = true;
				left->state = state_num++;
				parent->left = left;

				parent->right = p.geteNFA();
				find_accept(parent->right);
				state->left = left;
				state->right = parent->right;

				return s.substr(2, s.length());
			}
#pragma region "(()|)"
			else if (s[0] == '|')
			{
				// cout << "start (()I" << endl;

				parent->left = p.geteNFA();

				left->isAccept = true;
				left->state = state_num++;

				find_accept(parent->left);
				state->left = left;

				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					// cout << "start (()|[]) " << endl;

					Bra b;
					s = b.read(s);
					parent->right = b.geteNFA();
					find_accept(parent->right);
					state->left = left;

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					// cout << "start (()|()) " << endl;

					Pa p;
					s = p.read(s);
					parent->right = p.geteNFA();
					find_accept(parent->right);
					state->left = left;

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// cout << "start (()|e) " << endl;
					parent->right = left;

					return s.substr(2, s.length());
				}

			}
#pragma endregion 

#pragma region "(().)"
			else if (s[0] == '.')
			{
				// cout << "start (()." << endl;

				parent->left = p.geteNFA();

				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					// cout << "start (().[])" << endl;

					Bra b;
					s = b.read(s);
					find_accept(parent->left);
					state->left = b.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					// cout << "start (().())" << endl;
					Pa p;
					s = p.read(s);
					find_accept(parent->left);
					state->left = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// cout << "start (().e)" << endl;

					left->isAccept = true;
					left->state = state_num++;

					find_accept(parent->left);
					state->left = left;

					return s.substr(2, s.length());
				}
			}
		}
#pragma endregion

#pragma endregion

#pragma region "(e)"
		else if (s[0] == 'e')
		{
			// cout << "start (e" << endl;

			parent->state = state_num++;

			left->isAccept = true;
			left->state = state_num++;

			s = s.substr(1, s.length());

			if (s[0] == ')')
			{
				// cout << "start (e)*" << endl;

				parent->left = left;
				parent->left->left = left;

				return s.substr(2, s.length());
			}
			else if (s[0] == '|')
			{
				// cout << "start (e| " << endl;

				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					// cout << "start (e|[]) " << endl;

					Bra b;
					s = b.read(s);
					parent->left = b.geteNFA();
					find_accept(parent->left);
					state->left = left;

					parent->right = left;

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					// cout << "start (e|()) " << endl;

					Pa p;
					s = p.read(s);
					parent->left = p.geteNFA();
					find_accept(parent->left);
					state->left = left;

					parent->right = left;

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// cout << "start (e|e) " << endl;
					parent->left = left;

					state->isAccept = true;
					state->state++;
					parent->right = state;

					return s.substr(2, s.length());
				}
			}
			else if (s[0] == '.')
			{
				// cout << "start (e." << endl;

				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					// cout << "start (e.[])" << endl;

					Bra b;
					s = b.read(s);
					parent->left = b.geteNFA();
					find_accept(parent->left);
					state->left = left;

					return s.substr(1, s.length());

				}
				else if (s[0] == '(')
				{
					// cout << "start (e.())" << endl;

					Pa p;
					s = p.read(s);
					parent->left = p.geteNFA();
					find_accept(parent->left);
					state->left = left;

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// cout << "start (e.e)" << endl;

					left->isAccept = false;
					parent->left = left;
					state->isAccept = true;
					state->state = state_num++;
					parent->left->left = state;

					return s.substr(2, s.length());
				}
			}
		}
#pragma endregion
	}
};

int main()
{
	// RE : regular expression
	string RE;
	eNFA* eNFA;

	while (true)
	{
		// input RE
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

	/*
	//확인용 출력
	for (int i = 0; i < height; i++)
	{
		std::// cout << i << " - " << " : ";
		for (int j = 0; j < width; j++)
		{
			std::// cout << " input: " << j << " ";
			if (NfaTable[i][j] != 999)
				std::// cout << NfaTable[i][j] << " ";
			else
				std::// cout << "X";
		}
		std::// cout << std::endl;
	}
	*/

	vector<DFA> DFA_table;
	DFA_table = NFA2DFA(NfaTable);

	// accepting set
	int accept =0;
	findAcceptState(eNFA, &accept);
	DFA_table = acceptSet(DFA_table, accept);

	// loop
	while (1)
	{
		// S : input string
		string S;
		bool isAccept = false;

		// input string
		cout << "Input String : " << endl;
		getline(cin, S);

		// input no!
		if (S == "")
			invalid();

		// valid check
		vector<int> state;
		
		for (int i = 0; i < S.length(); i++)
			state = findState(DFA_table, state, S[i]);
		
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

		// ctrl + c exit

	}

	free(NfaTable[0]);
	free(NfaTable);

	return 0;
}

// function
eNFA* RE2eNFA(string S)
{
	// cout << "RE2eNFA excute" << endl;

	if (S[0] == '(')
	{
		Pa p;
		p.read(S);
		return p.geteNFA();
	}
	else if (S[0] == '[')
	{
		Bra b;
		b.read(S);
		return b.geteNFA();
	}
	else if (S[0] == 'e')
	{
		eNFA* state = (eNFA*)malloc(sizeof(eNFA));
		state->isAccept = true;
		state->state = state_num++;
		state->left = nullptr;
		state->right = nullptr;
		// cout << "input is epsilon" << endl;

		return state;
	}

	// cout << "valid check err" << endl;
	return nullptr;
}

bool isValidRE(std::string str)
{
	//parentheses 관련 변수
	int binOp = 0;      //operator *  개수
	int unaOp = 0;      //operator . |  개수
	int parFlag = 0;   //앞에서부터 읽을 때 '(' 의 개수가 ')' 보다 항상 많거나 같아야 한다.
					   //예를 들어 )))((( -> invalid

	int parOpenCnt = 0;
	int parCloseCnt = 0;   //최종 parentheses 개수 세기

						   //bracket 관련 변수
	bool braFlag = false;

	int repeatOp = 0;
	bool validCheck = true;
	bool exclude = false;
	bool number = false;

	//RE
	int numRE = 0;
	for (int i = 0; i < str.length(); i++)
	{
		if (validCheck == false)
			break;

		char ch = str.at(i);
		switch (ch)
		{
		case '(':
			if (braFlag == true)
				validCheck = false;
			parFlag++;
			parOpenCnt++;
			break;
		case ')':
			numRE++;
			parFlag--;
			if (braFlag == true || parFlag < 0)
				validCheck = false;
			parCloseCnt++;
			break;
		case '*':
			unaOp++;
			if (numRE < 1 || braFlag)
			{
				validCheck = false;
			}
			else
			{
				numRE--;
				if (str.at(i - 1) != ')')
					validCheck = false;
			}
			break;
		case '.': case '|':
			if (parFlag <= 0 || numRE < 1 || braFlag)
			{
				validCheck = false;
			}
			else
			{
				binOp++;
				numRE--;
				if (str.at(i - 1) != 'e' && str.at(i - 1) != ')' && str.at(i - 1) != ']' && str.at(i - 1) != '*')
					validCheck = false;
			}
			break;
		case'[':
			if (i > 1)
			{
				if (str.at(i - 1) != '(' && str.at(i - 1) != '.' && str.at(i - 1) != '|')
					validCheck = false;
			}
			if (braFlag == true)
				validCheck = false;
			else braFlag = true;
			break;
		case ']':
			if (braFlag != true || number == false || (exclude && (!number)))
				validCheck = false;
			else
				braFlag = false;
			exclude = false;
			number = false;
			if (parFlag > 0)
				numRE++;
			break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			if (braFlag == false || (exclude && number))      //if bracket is not open
				validCheck = false;      //invalid
			number = true;
			break;
		case '^':
			if (braFlag == false || number || exclude)
				validCheck = false;
			exclude = true;
			break;
		case 'e':
			if (i > 0)
			{
				if (str.at(i - 1) == '.' || str.at(i - 1) == '|' || str.at(i - 1) == '(')
				{
					;
				}
				else
				{
					validCheck = false;
				}
			}
			if (numRE > 0)
				validCheck = false;
			if (braFlag == true)
				validCheck = false;
			numRE++;
			break;
		default: validCheck = false;
			break;
		}
	}


	//()-> 연산자 개수와 관련
	if (parOpenCnt != parCloseCnt || parOpenCnt != (unaOp + binOp)
		|| braFlag == true || !validCheck)
	{

		std:: cout << "not RE" << std::endl;
		return false;
	}
	else
	{
		return true;
	}

}

void Eclose(eNFA* enfa)
{
	static int num = 0;

	// state 에서 1 빼준다.
	//state 0부터 state num 까지 
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
			//std::// cout << "Size of inner vector :" << EcloseCal(enfa).size() << std::endl;
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

	// temp[1].push_back(0);

	for (int j = 0; j < temp.size(); j++)
	{
		// std:: cout << "eclose state: " << temp[j][0] << " ";
		for (int k = 1; k < temp[j].size(); k++)
		{
			//std:: cout << temp[j][k] << " ";
		}
		// std:: cout << std::endl;
	}

	//std:: cout << "====================" << std::endl;

	int j, k = 0;
	for (int i = 0; i < temp.size(); i++)     //vector<int>  |  vector<int>  |  vector<int>  | ```
	{
		for (j = 1; j < temp[i].size(); j++)   // int | int | int |```
		{
			int num = temp[i][j];            // 두번째 element 부터

			for (k = 1; k < temp[num].size(); k++) // 두번째 element에 해당하는 state의 vector<int>
			{
				if (!dupliCheck(temp[i], temp[num][k]))
					temp[i].push_back(temp[num][k]);
			}

		}
	}

	for (int j = 0; j < temp.size(); j++)
	{
		// std:: cout << "eclose state: " << temp[j][0] << " ";

		for (int k = 1; k < temp[j].size(); k++)
		{
			//      ecloses[j][k] = temp[j][k];
			// std:: cout << temp[j][k] << " ";
			//      std::// cout << ecloses[j][k] << " ";
		}
		// std:: cout << std::endl;
	}
}

//vector에 push하기 전에 element가 vector에 이미 있는지 확인하는 함수
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

	// state 에서 1 빼준다.
	//state 0부터 state num 까지 
	eNFA* temp_l = enfa;
	eNFA* temp_r = enfa;
	static int flag[9999];

	if (num == 0)
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				NfaTable[i][j] = 999;         //999로 초기화
			}
		}
		num++;
	}

	//
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
			//std::// cout << "Size of inner vector :" << EcloseCal(enfa).size() << std::endl;

			//NfaTable
			if (enfa->left != nullptr)
			{
				//다음 left state의 edge num 을 확인 -> -1이 아닌경우
				if (enfa->left->edgeNum[0] == -1)
				{
					//(NULL 값이 다 들어가있는지 확인하기)
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
		//중복된 state가 있는지 검사
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
			//이 과정을 거칠 때마다 10개씩 늘어난다.
		}
		else
		{
			//중복되면

		}
		num++;
	}
	//vector에 넣는다
	//start state에 대해 input 을 넣은 결과를 각각 구한다 
	//결과를 저장한다
	return dfa;
}

vector<int> DFA_input(int** nfaTable, vector<int> curState, int input)
{
	vector<int> eTemp;

	for (int i = 0; i < curState.size(); i++)
	{
		//nfaTable[i][j] (i)state에서 (j)input이 들어올 때 state
		int elemCurState = curState[i];         //current state 속하는 하나의 state
		int nfaResult = nfaTable[elemCurState][input];      //nfa transition function에 대한 결과
		if (nfaResult != 999)
		{
			//curstate 집합의 하나의 원소의 NFA transition 결과에 대한 eclosure를 구한다
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

	//sorting 후 비교

	for (int i = 0; i< dfa.size(); i++)
	{
		sort(dfa[i].curState.begin(), dfa[i].curState.end());
		if (dfa[i].curState == *elem)
		{
			return true;
		}
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
		invalid();
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

void invalid()
{
	cout << "This string is invalid." << endl;
}
