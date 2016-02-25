#include <iostream>
#include <string>
#include <fstream>
#include <string.h>
using namespace std;

// WORD是单词，NUMINTOKENIZER是数字
// SP是特殊字符，包括''','-','"',':','(',')'等等
static int WORD = 256, NUMINTOKENIZER = 257, SP = 258;
class Token_new {
    public:
        Token_new();
        Token_new(long l, int t, string s, long b, long e, long tlt, long tnt);
        long getBeginIndex();
        long getEndIndex();
        long getToLastToken();
        long getToNextToken();
        long line;
        int tag;
        long begin;
        long end;
        long to_last_token;  //表示前一个token的末尾
        long to_next_token;  //表示后一个token的开端
        string str;
};

class Tokenizer {
    public:
        Tokenizer();
        Tokenizer(const char *filename);
        Tokenizer& operator=(const Tokenizer& other);
        //拷贝构造函数
        Tokenizer(Tokenizer& other);
         //由于构造函数中调用了new来创建对象的内存分配空间，则需要专门调用delete来释放内存
        ~Tokenizer();

        //判断是否已经读取到文章末尾
        bool isEnd();

        //文章未读完时，读取下一个字符
        void readch();

        //扫描文档
        Token_new scan();

        void printToken(Token_new tok);
        
        long getPos();
        long getSize();
        long getCurline();
        char getChar();
        char* getBuffer();
    private:
        long pos;  //表示当前读取，在文件中的位置
        long size;  //文件大小
        char *buffer;  //缓存整个文件
        char peek;  //用来存储当前读取到的字符
        // char buff;  //用来判断浮点数
        long curLine;  //表示当前字符在文件中所在行数
};
