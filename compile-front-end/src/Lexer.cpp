#include "Lexer.h"

Token::Token() {}
Token::Token(int l, int t, string s)
{
	line = l;
	tag = t;
	str = s;
}
Token::Token(const Token &other) {
	line = other.line;
	tag = other.tag;
	str = other.str;
}
Token& Token::operator=(const Token &other) {
	line = other.line;
	tag = other.tag;
	str = other.str;
	return *this;
}
int Token::getTag() {
	return tag;
}
int Token::getLine() {
	return line;
}
string Token::getStr() {
	return str;
}
//Lexer
Lexer::Lexer()
{
	file.open("Revenue.aql");
	peek = ' ';
	curLine = 0;
}
Lexer::Lexer(char* fileName)
{
	file.open(fileName);
	peek = ' ';
	curLine = 0;
}
Lexer::~Lexer()
{
	file.close();
}
// read a char
void Lexer::readch()
{
	peek = file.get();
}

Token Lexer::scan()
{
	for ( ; ; readch())
	{
		if (peek == ' ' || peek == '\t') continue;
		else if (peek == '\n') curLine += 1;
		else break;
	}
	string str = "";
	int tag = 282;
	// maybe an id or a keyword
	if ((peek >= 'a' && peek <= 'z') || (peek >= 'A' && peek <= 'Z') || peek == '_')
	{
		str += peek;
		while (1)
		{
			readch();

			if ((peek >= 'a' && peek <= 'z') || (peek >= 'A' && peek <= 'Z') || peek == '_'
				|| (peek >= '0' && peek <= '9'))
			{
				str += peek;
			}
			else break;
		}
		if (str == "create") tag = CREATE;
		else if (str == "view") tag = VIEW;
		else if (str == "as") tag = AS;
		else if (str == "output") tag = OUTPUT;
		else if (str == "select") tag = SELECT;
		else if (str == "from") tag = FROM;
		else if (str == "extract") tag = EXTRACT;
		else if (str == "regex") tag = REGEX;
		else if (str == "on") tag = ON;
		else if (str == "return") tag = RETURN;
		else if (str == "group") tag = GROUP;
		else if (str == "and") tag = AND;
		else if (str == "Token") tag = TOKEN;
		else if (str == "pattern") tag = PATTERN;
		else tag = IDENTIFIER;
	}
	// a number
	else if (peek >= '0' && peek <= '9')
	{
		str += peek;
		while (1)
		{
			readch();
			if (peek >= '0' && peek <= '9') str += peek;
			else break;
		}
		tag = NUM;
	}
	// a regex expression
	else if (peek == '/')
	{
		readch();
		while (1)
		{
			if(peek == '/')
			{
				tag = REG_EXP;
				readch();
				break;
			}
			else
			{
				str += peek;
				readch();
			}
		}
	}
	else
	{
		if (peek == ',') tag = COMMA;
		else if (peek == '.') tag = DOT;
		else if (peek == ';') tag = SEMICOLON;
		else if (peek == '(') tag = L_BRACKET;
		else if (peek == ')') tag = R_BRACKET;
		else if (peek == '<') tag = L_ABRACKET;
		else if (peek == '>') tag = R_ABRACKET;
		else if (peek == '{') tag = L_BRACE;
		else if (peek == '}') tag = R_BRACE;
		str += peek;
		readch();
	}
	Token tok(curLine, tag, str);
	return tok;
}
void Lexer::printToken(Token tok)
{
	std::cout << tok.getStr() << " <" << tok.getLine() << "> TAG:" << tok.getTag() << std::endl;
}

bool Lexer::isEndOfFile() {
	return (peek == EOF);
}