#include "glslGrammar.h"
#include <cctype>
#include <iostream>


std::vector<Token> tokenizeGLSL(const char *text)
{

	std::vector<Token> ret;
	ret.reserve(100);
	int currentPos = 0;

#pragma region helpers
	auto eof = [&]() -> bool
	{
		return text[currentPos] == 0;
	};

	auto isNewLine = [&]() -> bool
	{
		return
			text[currentPos] == '\n' || // Line Feed
			text[currentPos] == '\r' || // Carriage Return
			text[currentPos] == '\v' || // Vertical Tab
			text[currentPos] == '\f';   // Form Feed (additional newline-like whitespace)
	};
	
	auto isBlankSpace = [&]() -> bool
	{
		return
			text[currentPos] == ' ' ||
			text[currentPos] == '\t';
	};

	auto isNewLineOrSpace = [&]() -> bool
	{
		return isBlankSpace() || isNewLine();
	};

	auto consumeCharacter = [&]()
	{
		currentPos++;
	};

	auto consumeNewLineOrSpace = [&]()
	{
		while (isNewLineOrSpace())
		{
			consumeCharacter();
		}
	};
#pragma endregion

	auto isNumber = [&]()
	{
		return text[currentPos] >= '0' && text[currentPos] <= '9';
	};

	auto isLetterOrUnderscore = [&]()
	{
		return 
			(text[currentPos] >= 'a' && text[currentPos] >= 'z') ||
			(text[currentPos] >= 'A' && text[currentPos] >= 'Z') ||
			text[currentPos] == '_'
			;
	};

	static const char *symbols = ";.+/*%<>[](){}^|&~=!:,?";

	auto isSymbol = [&]()
	{
		return strchr(symbols, text[currentPos]);
	};

	auto isDirectiveSymbol = [&]()
	{
		return text[currentPos] == '#';
	};

	//

	while (!eof())
	{

		consumeNewLineOrSpace();
		if (eof()) { break; }

		// we have a string token!
		if (isLetterOrUnderscore())
		{
			Token t;
			t.start = currentPos;
			t.type = Token_String;

			while (true)
			{
				consumeCharacter();
				t.end = currentPos;

				if (eof() || isSymbol() || isNewLine())
				{
					break;
				}
			}

			ret.push_back(t);
		}
		else if (isNumber()) 
		{
			//we have a number!
			Token t;
			t.start = currentPos;
			t.type = Token_Number;

			int a = 0XaA;

			while (true)
			{
				consumeCharacter();
				t.end = currentPos;

				if (eof()) { break; }

				if (isNumber()
					|| text[currentPos] == '.'
					|| text[currentPos] == 'x'
					|| text[currentPos] == 'X'
					|| text[currentPos] == 'b'
					|| text[currentPos] == 'B'
					|| text[currentPos] == 'a'
					|| text[currentPos] == 'A'
					|| text[currentPos] == 'c'
					|| text[currentPos] == 'C'
					|| text[currentPos] == 'd'
					|| text[currentPos] == 'D'
					|| text[currentPos] == 'e'
					|| text[currentPos] == 'E'
					|| text[currentPos] == 'f'
					|| text[currentPos] == 'F'
					)
				{
					//good
				}
				else
				{
					break;
				}
			}

			ret.push_back(t);
		}
		else if (isSymbol())
		{
			//we have a symbol!

			if (text[currentPos] == '/'
				&& text[currentPos + 1] == '/')
			{
				//we have a comment
				// consume an entire line
				Token t;
				t.type = Token_Comment;
				t.start = currentPos;
				
				consumeCharacter();

				while (true)
				{
					consumeCharacter();
					t.end = currentPos;

					if (eof() || isNewLine())
					{
						break;
					}
				}

				ret.push_back(t);
			}
			else if (text[currentPos] == '/' && text[currentPos + 1] == '*')
			{
				//long comment!
				Token t;
				t.type = Token_Comment;
				t.start = currentPos;

				consumeCharacter();

				while (true)
				{
					consumeCharacter();
					t.end = currentPos;

					if (eof())
					{
						break;
					}

					if (text[currentPos] == '*' && text[currentPos + 1] == '/')
					{
						//end of comment!
						break;
					}
				}

				ret.push_back(t);
			}if (
				(text[currentPos] == '='
				&& text[currentPos + 1] == '=')
				||
				(text[currentPos] == '<'
				&& text[currentPos + 1] == '=') ||
				(text[currentPos] == '>'
				&& text[currentPos + 1] == '=') ||
				(text[currentPos] == '-'
				&& text[currentPos + 1] == '=') ||
				(text[currentPos] == '+'
				&& text[currentPos + 1] == '=') ||
				(text[currentPos] == '*'
				&& text[currentPos + 1] == '=') ||
				(text[currentPos] == '/'
				&& text[currentPos + 1] == '=') ||
				(text[currentPos] == '~'
				&& text[currentPos + 1] == '=') ||
				(text[currentPos] == '|'
				&& text[currentPos + 1] == '=') ||
				(text[currentPos] == '&'
				&& text[currentPos + 1] == '=') ||
				(text[currentPos] == '&'
				&& text[currentPos + 1] == '&') ||
				(text[currentPos] == '|'
				&& text[currentPos + 1] == '|') ||
				(text[currentPos] == '<'
				&& text[currentPos + 1] == '<') ||
				(text[currentPos] == '>'
				&& text[currentPos + 1] == '>') ||
				(text[currentPos] == '!'
				&& text[currentPos + 1] == '=')
				)
			{

				Token t;
				t.type = Token_Symbol;
				t.start = currentPos;
				consumeCharacter();
				consumeCharacter();
				t.end = currentPos;
				ret.push_back(t);
			}
			else
			{
				Token t;
				t.type = Token_Symbol;
				t.start = currentPos;
				t.end = currentPos + 1;
				consumeCharacter();
				ret.push_back(t);
			}


		
		}
		else if (isDirectiveSymbol())
		{
			// consume an entire line
			Token t;
			t.type = Token_Directive;
			t.start = currentPos;

			while (true)
			{
				consumeCharacter();
				t.end = currentPos;

				if (eof() || isNewLine())
				{
					break;
				}
			}

			ret.push_back(t);
		}
		else
		{
			//TODO error out!
			std::cout << "ERROR ! " << text[currentPos] << " !\n";
			break;
		}


	}


	return ret;
}
