#include <iostream>
#include <string>
//#include <vector>

// namespace
using namespace std;

// struct
typedef struct eNFAs
{
	// e À¸·Î ÃÊ±âÈ­
	int state; 		// ¾î¶»°Ô ¾²Áö..?
	char edge = 'e';		// [] edge °¡ ¿©·¯ °³ ÀÖ¾î¾ß ÇÔ..?
	int edgeNum[10] = { -1 };
	bool isAccept = false;
	struct eNFAs* left = NULL;
	struct eNFAs* right = NULL;
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
	//virtual void create() = 0;
	virtual string read(string) = 0;
	eNFA* geteNFA()
	{
		return parent;
	}
protected:
	eNFA* parent = (eNFA*)malloc(sizeof(eNFA));		// °¢ ÀÎ½ºÅÏ½º ¸¸´Ù ÃÊ±âÈ­ ½ÃÄÑ¼­ »ç¿ëÇÒ °Å (Á¦ÀÏ ºÎ¸ð)
	eNFA* left = (eNFA*)malloc(sizeof(eNFA));
	eNFA* right = (eNFA*)malloc(sizeof(eNFA));
};

class Bra : public Grammer		// brakets
{
public:
	virtual string read(string s)
	{
		//[]
		parent->state = state_num++;

		cout << "in Bra [ :: " << parent->state << endl;

		int i = 0;
		if (s[1] == '^')
		{
			parent->right = right;
			
			right->state = state_num++;	// state ..? right? left?


			cout << "in Bra [] :: " << right->state << endl;
			
			i = 2;
			while (s.at(i) != ']')
			{
				//edge = null 이나 특정한 문자로 초기화 한다.
				right->edgeNum[i - 2] = s.at(i) - '0';
				i++;
			}
			left->edge = '^';
		}	
		else
		{
			parent->left = left;

			left->state = state_num++;

			cout << "in Bra [] :: " << left->state << endl;
			
			i = 1;
			while (s.at(i) != ']')
			{
				//edge = null 이나 특정한 문자로 초기화 한다.
				left->edgeNum[i - 2] = s.at(i) - '0';
				i++;
			}
		}

		s = s.substr(i+1, s.length());

		return s;
	}
};

class Pa : public Grammer		// parentheses
{
public:
	virtual string read(string s)
	{

		s = s.substr(1, s.length());

		// parent init
		parent->state = state_num++;

		cout << "start ( :: " << parent->state << endl;

		if (s[0] == '[')
		{
			cout << "start ([" << endl;

			Bra b;
			s = b.read(s);

			cout << "end ([]" << endl;

			// ([] ±îÁö ÇÑ »óÅÂ.
			if (s[0] == ')')
			{
				cout << "end ([])*" << endl;

				// ([])* ÇüÅÂ
				parent->left = b.geteNFA();
				// ÀÌ°Å ¸Ç ¸¶Áö¸· ³ëµå¸¦ parent¿¡¿¬°á ÇÏ°í ½ÍÀ½..¤Ð
				parent->left->left = parent;

				return s.substr(2, s.length());
			}
			else if (s[0] == '|')
			{
				cout << "start ([]I" << endl;

				// ([]|) ÇüÅÂ
				parent->left = b.geteNFA();
				s = s.substr(1, s.length());

				// ¿À¸¥ÂÊ [] read
				if (s[0] == '[')
				{
					cout << "start ([]|[ " << endl;

					// ([]|[]) ÇüÅÂ
					Bra b;
					s = b.read(s);
					parent->right = b.geteNFA();

					cout << "end ([]I[])" << endl;

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					cout << "start ([]|( " << endl;

					// ([] | ()) ÇüÅÂ
					Pa p;
					s = p.read(s);
					parent->right = p.geteNFA();

					cout << "end ([]I())" << endl;

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// ([]|e) ÇüÅÂ
					parent->right = right;

					return s.substr(2, s.length());
				}
			}
			else if (s[0] == '.')
			{
				// ([].) ÇüÅÂ
				parent->left = b.geteNFA();
				s = s.substr(1, s.length());
				if (s[0] == '[')
				{
					// ([].[]) ÇüÅÂ
					Bra b;
					s = b.read(s);
					parent->left->left = b.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					// ([].()) ÇüÅÂ
					Pa p;
					s = p.read(s);
					parent->left->left = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// ([].e) ÇüÅÂ
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

			// (() ±îÁö ÇÑ »óÅÂ.
			if (s[0] == ')')
			{
				// (())* ÇüÅÂ
				parent->left = p.geteNFA();
				// parent ÀÇ ¸ðµç ÀÚ½Ä ³ëµåµéÀÌ ´Ù½Ã parent¸¦ °¡¸£Ä¡°Ô ÇØ¾ßÇÔ......?
				// ¸Ç ¸¶Áö¸· ³ëµåÀÇ left°¡ ÀÖÀ¸¸é ¾È¿¡¼­ ´Ù¤Ó¤µ ´Ù¸¥°Å °¡¸£Å°´À ¤¤°Å¶ó¼­ right¿¡ ÇØÁÖ°í ¾Æ´Ï¸é left¿¡ ¿¬°á

				return s.substr(2, s.length());
			}
			else if (s[0] == '|')
			{
				// (()|) ÇüÅÂ
				parent->left = p.geteNFA();
				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					// (()|[]) ÇüÅÂ
					Bra b;
					s = b.read(s);
					parent->right = b.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					// (()|()) ÇüÅÂ
					Pa p;
					s = p.read(s);
					parent->right = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					//(()|e) ÇüÅÂ
					parent->right = right;

					return s.substr(2, s.length());
				}

			}
			else if (s[0] == '.')
			{
				// (().) ÇüÅÂ
				parent->left = p.geteNFA();
				s = s.substr(1, s.length());
				if (s[0] == '[')
				{
					//(().[]) ÇüÅÂ
					Bra b;
					s = b.read(s);
					parent->left->left = b.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					// (().()) ÇüÅÂ
					Pa p;
					s = p.read(s);
					parent->left->left = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// (().e) ÇüÅÂ
					parent->left->left = right;

					return s.substr(2, s.length());
				}
			}
		}
		else if (s[0] == 'e')
		{
			// (e ÇüÅÂ....

			s = s.substr(1, s.length());
			if (s[0] == ')')
			{
				// (e)* ÇüÅÂ
				parent->left = left;
				parent->left->left = parent;

				return s.substr(2, s.length());
			}
			else if (s[0] == '|')
			{
				// (e| ÇüÅÂ
				parent->left = left;
				s = s.substr(1, s.length());
				if (s[0] == '[')
				{
					// (e|[]) ÇüÅÂ
					Bra b;
					s = b.read(s);
					parent->right = b.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					// (e|()) ÇüÅÂ
					Pa p;
					s = p.read(s);
					parent->right = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// (e|e) ÇüÅÂ
					parent->right = right;

					return s.substr(2, s.length());
				}
			}
			else if (s[0] == '.')
			{
				// (e. ÇüÅÂ
				parent->left = left;
				s = s.substr(1, s.length());
				if (s[0] == '[')
				{
					// (e.[]) ÇüÅÂ
					Bra b;
					s = b.read(s);
					parent->left->left = b.geteNFA();

					return s.substr(1, s.length());

				}
				else if (s[0] == '(')
				{
					// (e.()) ÇüÅÂ
					Pa p;
					s = p.read(s);
					parent->left->left = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					// (e.e) ÇüÅÂ
					parent->left->left = right;

					return s.substr(2, s.length());
				}
			}
		}
	}
};

// epsilon ¹»·Î ³ªÅ¸³»Áö...?  >> 'e'

int main()
{
	// RE : regular expression
	string RE;

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
		eNFA* eNFA = RE2eNFA(RE);
	}
	
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
		// epsilon..input.... Ã³¸®
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


bool isValidRE(std::string str)
{
	//parentheses 관련 변수
	int binOp = 0;		//operator *  개수
	int unaOp = 0;		//operator . |  개수
	int parFlag = 0;	//앞에서부터 읽을 때 '(' 의 개수가 ')' 보다 항상 많거나 같아야 한다.
						//예를 들어 )))((( -> invalid

	int parOpenCnt = 0;
	int parCloseCnt = 0;	//최종 parentheses 개수 세기

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
				if (str.at(i - 1) != 'e' && str.at(i - 1) != ')' && str.at(i - 1) != ']')
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
			if (braFlag != true || (number == false && exclude && (!number)))
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
			if (braFlag == false || (exclude && number))		//if bracket is not open
				validCheck = false;		//invalid
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