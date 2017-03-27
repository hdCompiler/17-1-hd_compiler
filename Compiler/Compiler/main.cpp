#include <iostream>
#include <string>

// namespace
using namespace std;

// struct
typedef struct eNFAs
{
	int state;
	int edgeNum[10]; // = { -1 };		// 전부 -1 이면 'e' 라고 생각?
	bool isAccept;  // = false;
	struct eNFAs* left;  // = NULL;
	struct eNFAs* right;  // = NULL;
}eNFA;

// global variable..?
extern int state_num = 0;

// function declare
eNFA* RE2eNFA(string S);
bool isValidRE(std::string str);

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
			state->edgeNum[i] = -1;		// 전부 -1 이면 'e' 라고 생각?
		state->isAccept = false;
		state->left = NULL;
		state->right = NULL;
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
		cout << "in Bra [ :: " << endl;

		init_state(parent);
		init_state(left);

		parent->state = state_num++;

		int i = 0;
		if (s[1] == '^')
		{
			cout << "in Bra [^] :: " << endl;

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
			cout << "in Bra [] :: " << endl;

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

class Pa : public Grammer		// parentheses
{
public:
	void find_accept(eNFA* root)
	{
		if (root != NULL)
		{
			if (root->isAccept)
			{
				root->isAccept = false;
				state = root;
			}
			else
			{
				find_accept(root->left);
				if (root->left != NULL && root->right != NULL)
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
			cout << "start ([" << endl;

			// parent init
			parent->state = state_num++;

			Bra b;
			s = b.read(s);

			if (s[0] == ')')
			{
				cout << "start ([])*" << endl;

				// parent->left -> e 일 때 accepting
				left->isAccept = true;
				left->state = state_num++;
				parent->left = left;

				// right set
				parent->right = b.geteNFA();
				find_accept(parent->right);
				////state->flag = 0;
				state->left = left;
				state->right = parent->right;

				return s.substr(2, s.length());
			}
#pragma region "([]|)"
			else if (s[0] == '|')
			{
				cout << "start ([]I" << endl;

				// left set
				parent->left = b.geteNFA();

				// accept state 설정
				left->isAccept = true;
				left->state = state_num++;

				// state 연결
				find_accept(parent->left);
				////state->flag = 0;
				state->left = left;

				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					cout << "start ([]|[]) " << endl;

					Bra b;
					s = b.read(s);
					parent->right = b.geteNFA();
					find_accept(parent->right);
					//state->flag = 0;
					state->left = left;

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					cout << "start ([]|()) " << endl;

					Pa p;
					s = p.read(s);
					parent->right = p.geteNFA();
					find_accept(parent->right);
					//state->flag = 0;
					state->left = left;

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					cout << "start ([]|e) " << endl;

					parent->right = left;

					return s.substr(2, s.length());
				}
			}
#pragma endregion

#pragma region "([].)"
			else if (s[0] == '.')
			{
				cout << "start ([]." << endl;

				// parent init
				parent->left = b.geteNFA();

				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					cout << "start ([].[])" << endl;

					Bra b;
					s = b.read(s);
					find_accept(parent->left);
					//state->flag = 0;
					state->left = b.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					cout << "start ([].())" << endl;

					Pa p;
					s = p.read(s);
					find_accept(parent->left);
					//state->flag = 0;
					state->left = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					cout << "start ([].e)" << endl;

					left->isAccept = true;
					left->state = state_num++;

					find_accept(parent->left);
					//state->flag = 0;
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
			cout << "start ((" << endl;

			// parent init
			parent->state = state_num++;

			Pa p;
			s = p.read(s);

			if (s[0] == ')')
			{
				cout << "start (())*" << endl;

				// parent->left -> e 일 때 accepting
				left->isAccept = true;
				left->state = state_num++;
				parent->left = left;

				// right set
				parent->right = p.geteNFA();
				/*
				if (parent->right->left->isAccept)
				{
					parent->right->left->isAccept = false;
					parent->right->left->left = left;
					
				}*/
				find_accept(parent->right);
				//state->flag = 0;
				state->left = left;
				state->right = parent->right;

				return s.substr(2, s.length());
			}
#pragma region "(()|)"
			else if (s[0] == '|')
			{
				cout << "start (()I" << endl;

				parent->left = p.geteNFA();

				// accept state 설정
				left->isAccept = true;
				left->state = state_num++;

				// state 연결
				find_accept(parent->left);
				//state->flag = 0;
				state->left = left;

				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					cout << "start (()|[]) " << endl;

					Bra b;
					s = b.read(s);
					parent->right = b.geteNFA();
					find_accept(parent->right);
					//state->flag = 0;
					state->left = left;

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					cout << "start (()|()) " << endl;

					Pa p;
					s = p.read(s);
					parent->right = p.geteNFA();
					find_accept(parent->right);
					//state->flag = 0;
					state->left = left;

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					cout << "start (()|e) " << endl;
					parent->right = left;

					return s.substr(2, s.length());
				}

			}
#pragma endregion 

#pragma region "(().)"
			else if (s[0] == '.')
			{
				cout << "start (()." << endl;

				// parent init
				parent->left = p.geteNFA();

				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					cout << "start (().[])" << endl;

					Bra b;
					s = b.read(s);
					find_accept(parent->left);
					//state->flag = 0;
					state->left = b.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					cout << "start (().())" << endl;
					Pa p;
					s = p.read(s);
					find_accept(parent->left);
					//state->flag = 0;
					state->left = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					cout << "start (().e)" << endl;

					left->isAccept = true;
					left->state = state_num++;

					find_accept(parent->left);
					//state->flag = 0;
					state->left = left;

					return s.substr(2, s.length());
				}
			}
		}
#pragma endregion

#pragma endregion

#pragma region "(e"
		else if (s[0] == 'e')
		{
			cout << "start (e" << endl;

			// parent init
			parent->state = state_num++;

			left->isAccept = true;
			left->state = state_num++;
			
			s = s.substr(1, s.length());

			if (s[0] == ')')
			{
				cout << "start (e)*" << endl;

				parent->left = left;
				parent->left->left = left;

				return s.substr(2, s.length());
			}
			else if (s[0] == '|')
			{
				cout << "start (e| " << endl;

				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					cout << "start (e|[]) " << endl;

					Bra b;
					s = b.read(s);
					parent->left = b.geteNFA();
					find_accept(parent->left);
					//state->flag = 0;
					state->left = left;

					parent->right = left;

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					cout << "start (e|()) " << endl;

					Pa p;
					s = p.read(s);
					parent->left = p.geteNFA();
					find_accept(parent->left);
					//state->flag = 0;
					state->left = left;

					parent->right = left;

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					cout << "start (e|e) " << endl;
					parent->left = left;

					state->isAccept = true;
					state->state++;
					parent->right = state;

					return s.substr(2, s.length());
				}
			}
			else if (s[0] == '.')
			{
				cout << "start (e." << endl;

				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					cout << "start (e.[])" << endl;

					Bra b;
					s = b.read(s);
					parent->left = b.geteNFA();
					find_accept(parent->left);
					//state->flag = 0;
					state->left = left;

					return s.substr(1, s.length());

				}
				else if (s[0] == '(')
				{
					cout << "start (e.())" << endl;

					Pa p;
					s = p.read(s);
					parent->left = p.geteNFA();
					find_accept(parent->left);
					//state->flag = 0;
					state->left = left;

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					cout << "start (e.e)" << endl;

					left->isAccept = false;
					parent->left = left;
					state->isAccept = true;
					state->state = state_num++;
					parent->left->left = state;

					return s.substr(2, s.length());
				}
			}
		}
	}
};






int main()
{
	// RE : regular expression
	string RE;
	eNFA* eNFA;

	// input RE
	cout << "Input Regular Expression :" << endl;
	getline(cin, RE);

	// RE -> eNFA
	if (isValidRE(RE) == false)
	{
		cout << "inValid input \n Program Exit." << endl;
		return 0;
	}
	else
	{
		cout << " valid iput" << endl;
		eNFA = RE2eNFA(RE);
	}

	// eNFA -> DFA
	cout << eNFA << endl;

	// loop
	while (1)
	{
		// S : input string
		string S;

		// input string
		cout << "Input String : " << endl;
		getline(cin, S);

		// ctrl + c exit
	}

	return 0;
}




// function
eNFA* RE2eNFA(string S)
{
	cout << "RE2eNFA excute" << endl;

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
	else if (S[0] == 'e')		// epsilon
	{
		// epsilon..input.... Ã³¸®
		eNFA* state = (eNFA*)malloc(sizeof(eNFA));
		state->isAccept = true;
		state->state = state_num++;
		state->left = NULL;
		state->right = NULL;
		cout << "input is epsilon" << endl;

		return state;
	}

	cout << "valid check err" << endl;
	return NULL;
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
	int braFlag = false;

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
				validCheck == false;
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

		std::cout << "not RE" << std::endl;
		return false;
	}
	else
	{
		return true;
	}
}