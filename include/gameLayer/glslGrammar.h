#pragma once
#include <vector>


constexpr int Token_None = 0;
constexpr int Token_String = 1;
constexpr int Token_Number = 2;
constexpr int Token_Symbol = 3;
constexpr int Token_Comment = 4;
constexpr int Token_Directive = 5; //for # stuff

struct Token
{

	unsigned int start = 0;
	unsigned int end = 0;
	unsigned int type = 0;

};


std::vector<Token> tokenizeGLSL(const char *text);