#include <iostream>
#include <vector>

#include "NFA.h"
#include "Bra.h"
#include "Pa.h"

using namespace std;

eNFA* RE2eNFA(string S)
{
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

		return state;
	}

	return nullptr;
}

bool isValidRE(std::string str)
{
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
		std::cout << "not RE" << std::endl;
		return false;
	}
	else
	{
		return true;
	}
}