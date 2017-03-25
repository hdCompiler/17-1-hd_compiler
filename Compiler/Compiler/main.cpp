#include <iostream>
#include <string>
//#include <vector>

// namespace
using namespace std;

// struct
typedef struct eNFAs
{
	// e ���� �ʱ�ȭ
	//int state; 		// ��� ����..?
	char edge = 'e';		// [] edge �� ���� �� �־�� ��..?
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
	eNFA* parent = (eNFA*)malloc(sizeof(eNFA));		// �� �ν��Ͻ� ���� �ʱ�ȭ ���Ѽ� ����� �� (���� �θ�)
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

		// ] ���� �� �ԽǷ� �̶� accept ó��
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

			// ([] ���� �� ����.
			if (s[0] == ')')
			{
				cout << "end ([])*" << endl;

				// ([])* ����
				parent->left = b.geteNFA();
				// �̰� �� ������ ��带 parent������ �ϰ� ����..��
				parent->left->left = parent;

				return s.substr(2, s.length());
			}
			else if (s[0] == '|')
			{
				cout << "start ([]I" << endl;

				// ([]|) ����
				parent->left = b.geteNFA();
				s = s.substr(1, s.length());

				// ������ [] read
				if (s[0] == '[')
				{
					cout << "start ([]|[ " << endl;

					// ([]|[]) ����
					Bra b;
					s = b.read(s);
					parent->right = b.geteNFA();

					cout << "end ([]I[])" << endl;

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					cout << "start ([]|( " << endl;

					// ([] | ()) ����
					Pa p;
					s = p.read(s);
					parent->right = p.geteNFA();

					cout << "end ([]I())" << endl;

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// ([]|e) ����
					parent->right = right;

					return s.substr(2, s.length());
				}
			}
			else if (s[0] == '.')
			{
				// ([].) ����
				parent->left = b.geteNFA();
				s = s.substr(1, s.length());
				if (s[0] == '[')
				{
					// ([].[]) ����
					Bra b;
					s = b.read(s);
					parent->left->left = b.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					// ([].()) ����
					Pa p;
					s = p.read(s);
					parent->left->left = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// ([].e) ����
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

			// (() ���� �� ����.
			if (s[0] == ')')
			{
				// (())* ����
				parent->left = p.geteNFA();
				// parent �� ��� �ڽ� ������ �ٽ� parent�� ����ġ�� �ؾ���......?
				// �� ������ ����� left�� ������ �ȿ��� �٤Ӥ� �ٸ��� ����Ű�� ���Ŷ� right�� ���ְ� �ƴϸ� left�� ����

				return s.substr(2, s.length());
			}
			else if (s[0] == '|')
			{
				// (()|) ����
				parent->left = p.geteNFA();
				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					// (()|[]) ����
					Bra b;
					s = b.read(s);
					parent->right = b.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					// (()|()) ����
					Pa p;
					s = p.read(s);
					parent->right = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					//(()|e) ����
					parent->right = right;

					return s.substr(2, s.length());
				}

			}
			else if (s[0] == '.')
			{
				// (().) ����
				parent->left = p.geteNFA();
				s = s.substr(1, s.length());
				if (s[0] == '[')
				{
					//(().[]) ����
					Bra b;
					s = b.read(s);
					parent->left->left = b.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					// (().()) ����
					Pa p;
					s = p.read(s);
					parent->left->left = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// (().e) ����
					parent->left->left = right;

					return s.substr(2, s.length());
				}
			}
		}
		else if (s[0] == 'e')
		{
			// (e ����....

			s = s.substr(1, s.length());
			if (s[0] == ')')
			{
				// (e)* ����
				parent->left = left;
				parent->left->left = parent;

				return s.substr(2, s.length());
			}
			else if (s[0] == '|')
			{
				// (e| ����
				parent->left = left;
				s = s.substr(1, s.length());
				if (s[0] == '[')
				{
					// (e|[]) ����
					Bra b;
					s = b.read(s);
					parent->right = b.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					// (e|()) ����
					Pa p;
					s = p.read(s);
					parent->right = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// (e|e) ����
					parent->right = right;

					return s.substr(2, s.length());
				}
			}
			else if (s[0] == '.')
			{
				// (e. ����
				parent->left = left;
				s = s.substr(1, s.length());
				if (s[0] == '[')
				{
					// (e.[]) ����
					Bra b;
					s = b.read(s);
					parent->left->left = b.geteNFA();

					return s.substr(1, s.length());

				}
				else if (s[0] == '(')
				{
					// (e.()) ����
					Pa p;
					s = p.read(s);
					parent->left->left = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// (e.e) ����
					parent->left->left = right;

					return s.substr(2, s.length());
				}
			}
		}
	}
};

// epsilon ���� ��Ÿ����...?  >> 'e'

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
		// epsilon..input.... ó��
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
