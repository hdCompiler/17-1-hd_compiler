#include "Grammer.h"
#include "Bra.h"
#include "Pa.h"
#include "NFA.h"

void Pa :: find_accept(eNFA* root)
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

string Pa :: read(string s)
	{
		init_state(parent);
		init_state(left);
		init_state(state);

		s = s.substr(1, s.length());

#pragma region "([])"
		if (s[0] == '[')
		{
			parent->state = state_num++;

			Bra b;
			s = b.read(s);

			if (s[0] == ')')
			{
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
				parent->left = b.geteNFA();

				left->isAccept = true;
				left->state = state_num++;

				find_accept(parent->left);

				state->left = left;

				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					Bra b;
					s = b.read(s);
					parent->right = b.geteNFA();
					find_accept(parent->right);

					state->left = left;

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					Pa p;
					s = p.read(s);
					parent->right = p.geteNFA();
					find_accept(parent->right);

					state->left = left;

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					parent->right = left;

					return s.substr(2, s.length());
				}
			}
#pragma endregion

#pragma region "([].)"
			else if (s[0] == '.')
			{
				parent->left = b.geteNFA();

				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					Bra b;
					s = b.read(s);
					find_accept(parent->left);
					state->left = b.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					Pa p;
					s = p.read(s);
					find_accept(parent->left);
					state->left = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
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
			parent->state = state_num++;

			Pa p;
			s = p.read(s);

			if (s[0] == ')')
			{
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
				parent->left = p.geteNFA();

				left->isAccept = true;
				left->state = state_num++;

				find_accept(parent->left);
				state->left = left;

				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					Bra b;
					s = b.read(s);
					parent->right = b.geteNFA();
					find_accept(parent->right);
					state->left = left;

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					Pa p;
					s = p.read(s);
					parent->right = p.geteNFA();
					find_accept(parent->right);
					state->left = left;

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
					parent->right = left;

					return s.substr(2, s.length());
				}

			}
#pragma endregion 

#pragma region "(().)"
			else if (s[0] == '.')
			{
				parent->left = p.geteNFA();

				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					Bra b;
					s = b.read(s);
					find_accept(parent->left);
					state->left = b.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == '(')
				{
					Pa p;
					s = p.read(s);
					find_accept(parent->left);
					state->left = p.geteNFA();

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
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
			parent->state = state_num++;

			left->isAccept = true;
			left->state = state_num++;

			s = s.substr(1, s.length());

			if (s[0] == ')')
			{
				parent->left = left;
				parent->left->left = left;

				return s.substr(2, s.length());
			}
			else if (s[0] == '|')
			{
				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
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
					parent->left = left;

					state->isAccept = true;
					state->state++;
					parent->right = state;

					return s.substr(2, s.length());
				}
			}
			else if (s[0] == '.')
			{
				s = s.substr(1, s.length());

				if (s[0] == '[')
				{
					Bra b;
					s = b.read(s);
					parent->left = b.geteNFA();
					find_accept(parent->left);
					state->left = left;

					return s.substr(1, s.length());

				}
				else if (s[0] == '(')
				{
					Pa p;
					s = p.read(s);
					parent->left = p.geteNFA();
					find_accept(parent->left);
					state->left = left;

					return s.substr(1, s.length());
				}
				else if (s[0] == 'e')
				{
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
