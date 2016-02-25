#include <string>
#include <fstream>
#include <iostream>

// using std::string;
// using std::ifstream;
using namespace std;


// 256-269 are keyword, 270 is ID, 271 is ',', 272 is '.', 273 is ';'
// 274 is '(', 275 is ')', 276 is '<', 277 is '>', 278 is '{', 279 is '}'
// 280 is a number, 281 is a regex expression, 282 is other symbol
static const int  CREATE     = 256, VIEW       = 257, AS         = 258, OUTPUT    = 259,
	        SELECT     = 260, FROM       = 261, EXTRACT    = 262, REGEX     = 263,
	        ON         = 264, RETURN     = 265, GROUP      = 266, AND       = 267,
	        TOKEN      = 268, PATTERN    = 269, IDENTIFIER = 270, COMMA     = 271,
	        DOT        = 272, SEMICOLON  = 273, L_BRACKET  = 274, R_BRACKET = 275,
	        L_ABRACKET = 276, R_ABRACKET = 277, L_BRACE    = 278, R_BRACE   = 279,
	        NUM        = 280, REG_EXP    = 281, OTHER      = 282;

/*
static string words[20] = {
	"create"    , "view", "as"    , "output", "select", "from" , "extract",
	"regex"     , "on"  , "return", "group" , "and"   , "Token", "pattern";
};
*/

class Token
{
	public:
		Token();
		Token(int l, int t, string s);
		Token(const Token &other);
		Token& operator=(const Token &other);
		int getTag();
		int getLine();
		string getStr();
		int line;
		int tag;
		string str;
};


//只实例化一次，不涉及赋值和作为参数传递的情况，以所没有重载赋值操作和拷贝构造函数
class Lexer
{
  public:
	Lexer();
	Lexer(char* fileName);
	~Lexer();
	// read a char
	void readch();

	Token scan();
	void printToken(Token tok);
	bool isEndOfFile();

  private:
  	ifstream file;
  	char peek;
  	int curLine;
};
