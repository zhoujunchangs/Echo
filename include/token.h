#pragma once

#ifndef __TOKEN_H_
#define __TOKEN_H_

#include "util.h"
#include <queue>

enum TokenSet {
//  Tokentype:             Tokendata:

	Unknown = 0,           // All of the strings that cannot match the below tokens

	TokenEOF = 1,		   // End of Code; Only for internal use; This will not appear in the code

	Name = 16,             // Object Name or Type Name It is a valid name when it conatins only: a-z, A-Z, 0-9, _, and number cannot be the first char
						   // Eg: valid:   abcd, ab234, ab_cd_3
						   //     invalid: 32ad, @ef, (ecdf)
	
	_Literal_Begin = 32,
	  LInt,                // Int              Eg: -23    => Token{type = TokenSet::LFloat;  data = "-23";  }//已解决
	  LFloat,              // Float            Eg: 2.34   => Token{type = TokenSet::LFloat;  data = "2.34"; }//已解决
	  LBool,               // Bool(true/false) Eg: true   => Token{type = TokenSet::LBool;   data = "true"; }
	  LChar,               // Char             Eg: 'a'    => Token{type = TokenSet::LChar;   data = "a";    }
	  LString,             // String           Eg: "abcd" => Token{type = TokenSet::LString; data = "abcd"; }
	_Literal_End,
	
	_Comment_Begin = 64,
	  Comments,            // ////已解决
	  Comments_begin,      // /*//已解决
	  Comments_end,        // *///已解决
	_Comment_End,

	_Bracket_Begin = 128,
	  Left_parenthesis,    // (//已解决
	  Right_parenthesis,   // )//已解决
	  Left_bracket,        // [//已解决
	  Right_bracket,       // ]//已解决
	  Left_brace,          // {//已解决
	  Right_brace,         // }//已解决
	_Bracket_End,
	
	_Hint_Begin = 256,
	  If,                  // ?//已解决
	  Else,				   // |//已解决
	  Colon,               // ://已解决
	  Definition,		   // >>>//已解决
	  Comma,               // ,//已解决
	  Semicolon,           // ;//已解决
	  Deduction,           // =>//已解决
	  Out,                 // <<//已解决
	  In,                  // >>//已解决
	_Hint_End,

	_Operator_Begin = 512,

	  _Access_op_Begin,
	    Dot,               // .//已解决
	  _Access_op_End,

	  _Arthmetic_op_Begin,
	    Multiply,          // *//已解决
	    Devide,            // ///已解决
	    Plus,              // +//已解决
	    Minus,             // -
	  _Arthmetic_op_End,
	  
	  _Bool_op_Begin,
		Not,               // !//已解决
	    Equal,             // ==//已解决
	    Notequal,          // !=//已解决
	    Greater,           // >//已解决
	    Less,              // <//已解决
	    Greaterequal,      // >=//已解决
	    Lessequal,         // <=//已解决
	    And,               // &&//已解决
	    Or,                // ||//已解决
	  _Bool_op_End,		   
						   
	  _Assign_op_Begin,	   
	    Assign,			   // =//已解决
		Refer,             // &=//已解决
	  _Assign_op_end,

	_Operator_End,
};

struct Token {
	TokenSet type;
	std::string data;
	Token() {}
	Token(const TokenSet& ty, const string& dat)
		: type(ty), data(dat) {}
};

#define TOKEN_IN_RANGE(token, token_type_range) IN_RANGE(token, TokenSet::_##token_type_range##_Begin, TokenSet::_##token_type_range##_End)

#define EXPECT_TOKEN_EM(var, expectedtype, errorlog) \
	Token var = getCurrentToken(); \
	if(var.type != expectedtype) { \
		Result::output() << (errorlog); \
	}

#define EXPECT_TOKEN_RET_EM(var, expectedtype, errorlog) \
	Token var = getCurrentToken(); \
	if(var.type != expectedtype) { \
		Result::output() << (errorlog); \
		return nullptr; \
	}

#define EXPECT_TOKEN(var, expectedtype) \
	EXPECT_TOKEN_EM(var, expectedtype, string("Expected a '") + #expectedtype + "', found '" + var.data + "'.")

#define EXPECT_TOKEN_RET(var, expectedtype) \
	EXPECT_TOKEN_RET_EM(var, expectedtype, string("Expected a '") + #expectedtype + "', found '" + var.data + "'.")


std::queue<Token> tokenbuf;
static Token lasttoken = Token(TokenSet::_Operator_Begin, "");

void addToken(const Token& token) {
	tokenbuf.push(token);
}

Token getNextToken() {
	if (tokenbuf.empty()) return Token(TokenSet::TokenEOF, "@EOF");
	Token cur = tokenbuf.front();
	lasttoken = cur;
	tokenbuf.pop();
	return cur;
}

Token getCurrentToken() {
	if (tokenbuf.empty()) return Token(TokenSet::TokenEOF, "@EOF");
	return tokenbuf.front();
}

static inline int getTokenPrior(const TokenSet& type) {
	switch (type) {
	default:
		return -1;
	case TokenSet::Assign:
	case TokenSet::Refer:
		return 10;
	case TokenSet::And:
	case TokenSet::Or:
		return 20;
	case TokenSet::Equal:
	case TokenSet::Notequal:
	case TokenSet::Greater:
	case TokenSet::Less:
	case TokenSet::Greaterequal:
	case TokenSet::Lessequal:
		return 30;
	case TokenSet::Not:
		return 40;
	case TokenSet::Plus:
	case TokenSet::Minus:
		return 50;
	case TokenSet::Multiply:
	case TokenSet::Devide:
		return 60;
	case TokenSet::Dot:
		return 70;
	case TokenSet::Left_parenthesis:
	case TokenSet::Left_bracket:
		return 80;
	}
}

static inline bool isSingleOperatorLHS(const TokenSet& type) {
	return type == TokenSet::Not;
}

static inline bool isSingleOperatorRHS(const TokenSet& type) {
	return
		type == TokenSet::Left_parenthesis ||
		type == TokenSet::Left_bracket;
}


static std::string codebuf; // The code input

void lexToken(string a) {
	vector<Token> v;
	a += "@@@";
	int m = 0;
	for (int i = 0; a[i] != '@';)
	{
		if (a[i] == '\n' || a[i] == '\t' || a[i] == ' ') {
			i += 1;
			continue;
		}
		if (a[i] == '/')
		{
			if (i + 1 < a.size() && a[i + 1] == '/')
			{
				int j, k = 0;
				for (j = 0; i + 1 + j < a.size() && a[i + 1 + j] != '\n'; j++);
				i += 1 + j + 1;
				continue;
			}
			else if (a[i + 1] == '*')
			{
				int j = 1;
				while (1)
				{
					if (i + 1 + j < a.size() && a[i + 1 + j] == '*')
					{
						if (i + 1 + j + 1 < a.size() && a[i + 1 + j + 1] == '/')
						{
							i += 1 + j + 2;
							break;
						}
					}
					j++;
					if (j + 1 + i >= a.size()) break;
				}
				continue;
			}
		}
		v.push_back(Token{});
		if (a[i] >= 48 && a[i] <= 57)//数字打头一定是int或float
		{
			int j, k;
			for (j = 0; a[i + j] >= 48 && a[i + j] <= 57; j++);
			if (a[i + j] != 46)
			{
				v[m].type = LInt;
				v[m].data = a.substr(i, j);
				i += j;
				m++;
			}
			if (a[i + j] == 46)
			{
				for (k = 1; a[i + j + k] >= 48 && a[i + j + k] <= 57; k++);
				v[m].type = LFloat;
				v[m].data = a.substr(i, j + k);
				i += j + k;
				m++;
			}
		}
		else if (a[i] == '/')
		{
			v[m].type = Devide;
			v[m].data = '/';
			i++;
			m++;
		}
		else if (a[i] == '(')
		{
			v[m].type = Left_parenthesis;
			v[m].data = '(';
			i++;
			m++;
		}
		else if (a[i] == ')')
		{
			v[m].type = Right_parenthesis;
			v[m].data = ')';
			i++;
			m++;
		}
		else if (a[i] == '[')
		{
			v[m].type = Left_bracket;
			v[m].data = '[';
			i++;
			m++;
		}
		else if (a[i] == ']')
		{
			v[m].type = Right_bracket;
			v[m].data = ']';
			i++;
			m++;
		}
		else if (a[i] == '{')
		{
			v[m].type = Left_brace;
			v[m].data = '{';
			i++;
			m++;
		}
		else if (a[i] == '}')
		{
			v[m].type = Right_brace;
			v[m].data = '}';
			i++;
			m++;
		}
		else if (a[i] == '?')
		{
			v[m].type = If;
			v[m].data = '?';
			i++;
			m++;
		}
		else if (a[i] == ':')
		{
			v[m].type = Colon;
			v[m].data = ':';
			i++;
			m++;
		}
		else if (a[i] == ',')
		{
			v[m].type = Comma;
			v[m].data = ',';
			i++;
			m++;
		}
		else if (a[i] == ';')
		{
			v[m].type = Semicolon;
			v[m].data = ';';
			i++;
			m++;
		}
		else if (a[i] == '|')
		{
			if (a[i + 1] == '|')
			{
				v[m].type = Or;
				v[m].data = "||";
				i = i + 2;
				m++;
			}
			else
			{
				v[m].type = Else;
				v[m].data = '|';
				i++;
				m++;
			}
		}
		else if (a[i] == '.')
		{
			v[m].type = Dot;
			v[m].data = '.';
			i++;
			m++;
		}
		else if (a[i] == '*')
		{
			v[m].type = Multiply;
			v[m].data = '*';
			i++;
			m++;
		}
		else if (a[i] == '+')
		{
			v[m].type = Plus;
			v[m].data = '+';
			i++;
			m++;
		}
		else if (a[i] == '>')
		{
			if (a[i + 1] == '=')
			{
				v[m].type = Greaterequal;
				v[m].data = ">=";
				i = i + 2;
				m++;
			}
			else if (a[i + 1] == '>')
			{
				if (a[i + 2] == '>')
				{
					v[m].type = Definition;
					v[m].data = ">>>";
					i = i + 3;
					m++;
				}
				else
				{
					v[m].type = In;
					v[m].data = ">>";
					i = i + 2;
					m++;
				}
			}
			else
			{
				v[m].type = Greater;
				v[m].data = '>';
				i++;
				m++;
			}
		}
		else if (a[i] == '<')
		{
			if (a[i + 1] == '=')
			{
				v[m].type = Lessequal;
				v[m].data = "<=";
				i = i + 2;
				m++;
			}
			else if (a[i + 1] == '<')
			{
				v[m].type = Out;;
				v[m].data = "<<";
				i = i + 2;
				m++;
			}
			else
			{
				v[m].type = Less;
				v[m].data = '<';
				i++;
				m++;
			}
		}
		else if (a[i] == '!')
		{
			if (a[i + 1] == '=')
			{
				v[m].type = Notequal;
				v[m].data = "!=";
				i = i + 2;
				m++;
			}
			else
			{
				v[m].type = Not;
				v[m].data = '!';
				i++;
				m++;
			}
		}
		else if (a[i] == '&')
		{
			if (a[i + 1] == '&')
			{
				v[m].type = And;
				v[m].data = "&&";
				i = i + 2;
				m++;
			}
			else if (a[i + 1] == '=')
			{
				v[m].type = Refer;
				v[m].data = "&=";
				i = i + 2;
				m++;
			}
			else
			{
				v[m].type = Unknown;
				v[m].data = '&';
				i++;
				m++;
			}
		}
		else if (a[i] == '=')
		{
			if (a[i + 1] == '>')
			{
				v[m].type = Deduction;
				v[m].data = "=>";
				i = i + 2;
				m++;
			}
			else if (a[i + 1] == '=')
			{
				v[m].type = Equal;
				v[m].data = "==";
				i = i + 2;
				m++;
			}
			else
			{
				v[m].type = Assign;
				v[m].data = '=';
				i++;
				m++;
			}
		}
		else if (a[i] == '-')
		{
			if (m - 1 >= 0 &&( v[m - 1].type == LInt || v[m - 1].type == LFloat || v[m - 1].type == Name))
			{
				v[m].type = Minus;
				v[m].data = '-';
				i++;
				m++;
			}
			else
			{
				if (a[i + 1] < '0' || a[i + 1]>'9')
				{
					v[m].type = Minus;
					v[m].data = '-';
					i++;
					m++;
				}
				else if (a[i + 1] >= '0' && a[i + 1] <= '9')
				{
					int j, k;
					for (j = 0; a[i + 1 + j] >= '0' && a[i + 1 + j] <= '9'; j++);
					if (a[i + 1 + j] != 46)
					{
						v[m].type = LInt;
						v[m].data = a.substr(i, j + 1);
						i += j + 1;
						m++;
					}
					else
					{
						for (k = 1; a[i + 1 + j + k] >= '0' && a[i + 1 + j + k] <= '9'; k++);
						v[m].type = LFloat;
						v[m].data = a.substr(i, j + 1 + k);
						i += j + 1 + k;
						m++;
					}
				}
			}
		}
		else if (a[i] == '_' || (a[i] >= 'A' && a[i] <= 'Z') || (a[i] >= 'a' && a[i] <= 'z'))
		{
			int j=0,p,q;
			while (1)
			{
				if ((a[i + j] >= '0' && a[i + j] <= '9') || (a[i + j] >= 'A' && a[i + j] <= 'Z') || (a[i + j] >= 'a' && a[i + j] <= 'z') || a[i + j] == '_')
					j++;
				else
					break;
			}
			string ms = a.substr(i, j);
			p = strcmp(ms.c_str(), "true");
			q = strcmp(ms.c_str(), "false");
			if (p == 0)
			{
				v[m].type = LBool;
				v[m].data = "true";
				i += j;
				m++;
			}
			else if (q == 0)
			{
				v[m].type = LBool;
				v[m].data = "false";
				i += j;
				m++;
			}
			else
			{
				v[m].type = Name;
				v[m].data = ms;
				i += j;
				m++;
			}

		}
		else if (a[i] == '\'')//单引号打头一定是char
		{
			v[m].type = LChar;
			v[m].data = a[i + 1];
			m++;
			if (a[i + 2] == '\'')
			{
				i += 3;
			}
			else
			{
				v[m].type = Unknown;
				v[m].data = a[i + 2];
				m++;
				i += 3;
			}
		}
		else
		{
			v[m].type = Unknown;
			v[m].data = a[i];
			i++;
			m++;
		}
	}

	for (auto i = v.begin(); i != v.end(); ++i) {
		addToken(*i);
	}
}
/// <summary>
/// 
/// Note:
/// 
/// [1] Each token will be separated by space, tab, or \n
/// [2] the code we are to compile is stored in the codebuf
/// [3] the tokens prefixed with '_' are only for internal use, you can just put them aside
/// 
/// Eg: ">>> point { x: Int; y: Int; } print('a'); c <= -123;"
///		will be divided into:
/// Token:  {type:              data:    }	
/// 		{  Definition,        ">>>"  }
/// 		{  Name,              "point"}
/// 		{  Left_brace,        "{"    }
/// 		{  Name,              "x"    }
/// 		{  Colon,             ":"    }
/// 		{  Name,              "Int"  }
/// 		{  Semicolon,         ";"    }
/// 		{  Name,              "y"    }
/// 		{  Colon,             ":"    }
/// 		{  Name,              "Int"  }
/// 		{  Semicolon,         ";"    }
/// 		{  Right_brace,       "}"    }
/// 		{  Name,              "print"}
/// 		{  Left_parenthesis,  "("    }
/// 		{  LChar,             "a"    }
/// 		{  Right_parenthesis, ")"    }
/// 		{  Semicolon,         ";"    }
/// 		{  Name,              "c"    }
/// 		{  Lessequal,         "<="   }
/// 		{  LInt,              "-123" }
/// 
/// It seems hard?
/// But you just need to implement just 2 function:
/// [1] Token getCurrentToken()
///		This will return the first token of the current codebuf
///		
///		Eg: The current codebuf is              ">>> a { x : Int ; }"
///			getCurrentToken() will return:      Token{type = Definition; data = ">>>"}
///			and the current codebuf is still    ">>> a { x : Int ; }"
/// 
/// [2] Token getNextToken()
///		This will return the fitst token of the current codebuf and remove it from the codebuf
/// 
///		Eg: The current codebuf is:             ">>> a { x : Int ; }"
///			getNextToken() will return:         Token{type = Definition; data = ">>>"}
///			and the current codebuf will be:    "a { x : Int ; }"
/// </summary>

#endif // !__TOKEN_H_
