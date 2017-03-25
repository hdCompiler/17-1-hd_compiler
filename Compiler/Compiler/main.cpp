#include <iostream>
#include <string>
//#include <vector>

// namespace
using namespace std;

// struct
typedef struct eNFAs
{
	// e 으로 초기화
	//int state; 		// 어떻게 쓰지..?
	char edge = 'e';		// [] edge 가 여러 개 있어야 함..?
	bool isAccept = false;
	//vector<eNFAs*> child;
	struct eNFAs* left = NULL;
	struct eNFAs* right = NULL;
}eNFA;

// function declare
eNFA* RE2eNFA(string S);

// class declare
class Pa;
class Bra;

// class
class Grammer
{
public:
	//virtual void create() = 0;
	virtual string read(string) = 0;
	eNFA* geteNFA()
	{
		return parent;
	}
protected:
	eNFA* parent = (eNFA*)malloc(sizeof(eNFA));		// 각 인스턴스 만다 초기화 시켜서 사용할 거 (제일 부모)
	eNFA* left = (eNFA*)malloc(sizeof(eNFA));
	eNFA* right = (eNFA*)malloc(sizeof(eNFA));
};

class Bra : public Grammer		// brakets
{
public:
	virtual string read(string s)
	{
		//[]

		cout << "in Bra [] " << endl;
		s = s.substr(2, s.length());

		// ] 닫을 때 입실론 이랑 accept 처리
		return s;
	}
};

class Pa : public Grammer		// parentheses
{
public:
	virtual string read(string s)
	{
		cout << "start (" << endl;

		s = s.substr(1, s.length());

		// parent init
		//
		//

		if (s[0] == '[')
		{
			cout << "start ([" << endl;

			Bra b;
			s = b.read(s);

			cout << "end ([]" << endl;

			// ([] 까지 한 상태.
			if (s[0] == ')')
			{
				cout << "end ([])*" << endl;

				// ([])* 형태
				parent->left = b.geteNFA();
				// 이거 맨 마지막 노드를 parent에연결 하고 싶음..ㅠ
				parent->left->left = parent;

				return s.substr(2, s.length());
			}
			else if (s[0] == '|')
			{
				cout << "start ([]I" << endl;

				// ([]|) 형태
				parent->left = b.geteNFA();
				s = s.substr(1, s.length());

				// 오른쪽 [] read
				if (s[0] == '[')
				{
					cout << "start ([]|[ " << endl;

					// ([]|[]) 형태
					Bra b;
					s = b.read(s);
					parent->right = b.geteNFA();

					cout << "end ([]I[])" << endl;

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					cout << "start ([]|( " << endl;

					// ([] | ()) 형태
					Pa p;
					s = p.read(s);
					parent->right = p.geteNFA();

					cout << "end ([]I())" << endl;

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// ([]|e) 형태
					parent->right = right;

					return s.substr(2, s.length());
				}
			}
			else if (s[0] == '.')
			{
				// ([].) 형태
				parent->left = b.geteNFA();
				s = s.substr(1, s.length());
				if (s[0] == '[')
				{
					// ([].[]) 형태
					Bra b;
					s = b.read(s);
					parent->left->left = b.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					// ([].()) 형태
					Pa p;
					s = p.read(s);
					parent->left->left = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// ([].e) 형태
					parent->left->left = right;

					return s.substr(2, s.length());
				}
			}
		}
		else if (s[0] == '(')
		{
			cout << "start (" << endl;

			Pa p;
			s = p.read(s);

			// (() 까지 한 상태.
			if (s[0] == ')')
			{
				// (())* 형태
				parent->left = p.geteNFA();
				// parent 의 모든 자식 노드들이 다시 parent를 가르치게 해야함......?
				// 맨 마지막 노드의 left가 있으면 안에서 다ㅣㅅ 다른거 가르키느 ㄴ거라서 right에 해주고 아니면 left에 연결

				return s.substr(2, s.length());
			}
			else if (s[0] == '|')
			{
				// (()|) 형태
				parent->left = p.geteNFA();
				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					// (()|[]) 형태
					Bra b;
					s = b.read(s);
					parent->right = b.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					// (()|()) 형태
					Pa p;
					s = p.read(s);
					parent->right = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					//(()|e) 형태
					parent->right = right;

					return s.substr(2, s.length());
				}

			}
			else if (s[0] == '.')
			{
				// (().) 형태
				parent->left = p.geteNFA();
				s = s.substr(1, s.length());
				if (s[0] == '[')
				{
					//(().[]) 형태
					Bra b;
					s = b.read(s);
					parent->left->left = b.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					// (().()) 형태
					Pa p;
					s = p.read(s);
					parent->left->left = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// (().e) 형태
					parent->left->left = right;

					return s.substr(2, s.length());
				}
			}
		}
		else if (s[0] == 'e')
		{
			// (e 형태....

			s = s.substr(1, s.length());
			if (s[0] == ')')
			{
				// (e)* 형태
				parent->left = left;
				parent->left->left = parent;

				return s.substr(2, s.length());
			}
			else if (s[0] == '|')
			{
				// (e| 형태
				parent->left = left;
				s = s.substr(1, s.length());
				if (s[0] == '[')
				{
					// (e|[]) 형태
					Bra b;
					s = b.read(s);
					parent->right = b.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					// (e|()) 형태
					Pa p;
					s = p.read(s);
					parent->right = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// (e|e) 형태
					parent->right = right;

					return s.substr(2, s.length());
				}
			}
			else if (s[0] == '.')
			{
				// (e. 형태
				parent->left = left;
				s = s.substr(1, s.length());
				if (s[0] == '[')
				{
					// (e.[]) 형태
					Bra b;
					s = b.read(s);
					parent->left->left = b.geteNFA();

					return s.substr(1, s.length());

				}
				else if (s[0] == '(')
				{
					// (e.()) 형태
					Pa p;
					s = p.read(s);
					parent->left->left = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// (e.e) 형태
					parent->left->left = right;

					return s.substr(2, s.length());
				}
			}
		}
	}
};

// epsilon 뭘로 나타내지...?  >> 'e'

int main()
{
	// RE : regular expression
	string RE;

	// input RE
	cout << "Input Regular Expression :" << endl;
	getline(cin, RE);

	// RE -> eNFA
	if (RE[0] != '(' && RE[0] != '[' && RE[0] != 'e')
	{
		cout << "Valid input \n Program Exit." << endl;
		return 0;
	}
	else
		eNFA* eNFA = RE2eNFA(RE);

	// eNFA -> DFA


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
		// epsilon..input.... 처리
		eNFA* state = (eNFA*)malloc(sizeof(eNFA));
		//state->state = 0;
		state->edge = 'e';
		state->isAccept = true;
		state->left = NULL;
		state->right = NULL;
		cout << "input is epsilon" << endl;

		return state;
	}

	cout << "valid check err" << endl;
	//return NULL;
}
