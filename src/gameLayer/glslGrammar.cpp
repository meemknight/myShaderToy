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

	static const char *symbols = ";.+-/*%<>[](){}^|&~=!:,?";

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

			//TODO ALSO ACCOUNT FOR COMMENTS!

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


std::vector<Token> tokenizeDirective(const char *text, int start, int end)
{

	std::vector<Token> ret;
	ret.reserve(10);

	int currentPos = start;

	auto eof = [&]() -> bool
	{
		return currentPos >= end;
	};

	auto isBlankSpace = [&]() -> bool
	{
		return
			text[currentPos] == ' ' ||
			text[currentPos] == '\t';
	};

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

	auto consumeCharacter = [&]()
	{
		currentPos++;
	};

	auto consumeBlankSpace = [&]()
	{
		if (isBlankSpace()) { consumeCharacter(); }
	};

	if (eof()) { return {}; }

	consumeBlankSpace();

	//consume the leading # if there
	if (text[currentPos] == '#')
	{
		consumeCharacter();
	}

	while (!eof())
	{
		consumeBlankSpace();

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

				if (eof() || isSymbol() || isBlankSpace())
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
			if (
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
		else
		{
			//TODO error out!
			std::cout << "ERROR ! " << text[currentPos] << " !\n";
			break;
		}

	}


	return ret;
};

bool isStringViewSame(const char *text, int start, int end, const char *other)
{

	int s = strlen(other);

	if (s != end - start) { return 0; }

	for (int i = 0; i < s; i++)
	{
		if (text[start + i] != other[i]) { return false; }
	}

	return true;
}

//takes a tokenized directive!
bool parseVersion(std::vector<Token> &tokens, const char *text)
{
	int pos = 0;
	auto eof = [&]() -> bool
	{
		return pos >= tokens.size();
	};

	auto consume = [&]() { pos++; };

	auto consumeString = [&](const char *s) -> bool
	{
		
		if (tokens[pos].type == Token_String)
		{
			if (isStringViewSame(text, tokens[pos].start, tokens[pos].end, s))
			{
				consume();
				return true;
			}
		}
		return 0;
	};
	
	auto consumeNumber = [&]() -> bool
	{
		if (tokens[pos].type == Token_Number)
		{
			consume();
			return true;
		}
		return 0;
	};

	if (eof()) { return 0; }

	if (!consumeString("version")) { return 0; };
	if (!consumeNumber()) { return 0; }

	if(eof() || consumeString("core")) 
	{
		//good
	}
	else
	{
		return 0;
	}

	//todo stict test. like check for garbage after!

	return true;

}

bool hasVersion(std::vector<Token> &tokens, const char *text)
{

	int pos = 0;

	auto eof = [&]() -> bool
	{
		return pos >= tokens.size();
	};

	if (eof()) { return 0; }

	if (tokens[0].type == Token_Directive)
	{
		auto directiveTokens = tokenizeDirective(text, tokens[0].start, tokens[0].end);

		//for (auto &i : directiveTokens)
		//{
		//	std::string t(text + i.start, text + i.end);
		//	std::cout << " ->  " << t << "\n";
		//}
		
		return parseVersion(directiveTokens, text);
	}

	return false;
}
