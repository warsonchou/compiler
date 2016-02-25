#include "Tokenizer.h"

Token_new::Token_new() {}
Token_new::Token_new(long l, int t, string s, long b, long e, long tlt, long tnt) {
    line = l;
    tag = t;
    str = s;
    begin = b;
    end = e;
    to_last_token = tlt;
    to_next_token = tnt;
}
long Token_new::getBeginIndex() {
    return begin;
}
long Token_new::getEndIndex() {
    return end;
}
long Token_new::getToLastToken() {
    return to_last_token;
}
long Token_new::getToNextToken() {
    return to_next_token;
}
Tokenizer::Tokenizer(){}
Tokenizer::Tokenizer(const char *filename) {
    buffer = new char;
    pos = 0;
    peek = ' ';
    curLine = 0;
    size = 0;
    ifstream file;
    //为了读入整个文件，只能以二进制形式打开
    file.open(filename, ios::binary);
    //判断文件是否存在
    if (!file.is_open()) {
        cout << "the file is not existed!" << endl;
    }
    else {
        //获取file对应buffer对象的指针
        filebuf *pbuf = file.rdbuf();
        //调用buffer对象方法获取文件大小
        size = pbuf->pubseekoff(0, ios::end, ios::in);
        pbuf->pubseekpos(0, ios::in);
        //分配内存空间
        buffer = new char[size];
        //获取文件内容
        pbuf->sgetn(buffer, size);
    }
    file.close();
}

//拷贝构造函数
Tokenizer::Tokenizer(Tokenizer& other) {
    pos = other.pos;  //表示当前读取，在文件中的位置
    size = other.size;  //文件大小
    peek = other.peek;  //用来存储当前读取到的字符
    // char buff;  //用来判断浮点数
    curLine = other.curLine;  //表示当前字符在文件中所在行数

    char *tmp = new char[size];  //申请另一块内存空间
    memset(tmp, 0, size);  //初始化
    strncpy(tmp, other.buffer, size);  //拷贝前size个字符到temp
    tmp += '\0';
    buffer = tmp;
}
//赋值操作符重载
Tokenizer& Tokenizer::operator=(const Tokenizer& other) {
    pos = other.pos;  //表示当前读取，在文件中的位置
    size = other.size;  //文件大小
    peek = other.peek;  //用来存储当前读取到的字符
    // char buff;  //用来判断浮点数
    curLine = other.curLine;  //表示当前字符在文件中所在行数

    delete []buffer;  //销毁原有空间
    char *tmp = new char[size];
    memset(tmp, 0, size);
    strncpy(tmp, other.buffer, size);
    tmp += '\0';
    buffer = tmp;
    return *this;
}
 //由于构造函数中调用了new来创建对象的内存分配空间，则需要专门调用delete来释放内存
Tokenizer::~Tokenizer() {delete []buffer;}

//判断是否已经读取到文章末尾
bool Tokenizer::isEnd() {return pos == size;}

//文章未读完时，读取下一个字符
void Tokenizer::readch() {
    if (!isEnd()) {
        peek = buffer[pos];
        pos++;
    }
}

//扫描文档
Token_new Tokenizer::scan() {
    if (!isEnd()) {
        //忽略所有的空白字符
        for (;;readch()) {
            if (isEnd()) return Token_new(-1, -1, "EOF", -1, -1, -1, -1);
            if (peek == ' ' || peek == '\t') continue;
            else if (peek == '\n') curLine += 1;
            else break;
        }

        string str = "";  //缓存读取的字符
        int tag;  //记录token的属性
        long begin = pos-1;
        long to_last_token, to_next_token;
        if (begin > 0) {
            int p = begin-1;
            while (buffer[p] == ' ' || buffer[p] == '\t' || buffer[p] == '\n') {
                p--;
                if (p < 0) break;
            }
            to_last_token = p + 1;
        } else {
            to_last_token = 0;
        }

        //读取的字符是字母
        if ((peek >= 'a' && peek <= 'z') || (peek >= 'A' && peek <= 'Z')) {
            str += peek;
            while (1) {
                readch();
                if (isEnd()) break;
                if ((peek >= 'a' && peek <= 'z') || (peek >= 'A' && peek <= 'Z'))
                    str += peek;
                else
                    break;
            }
            tag = WORD;
        }
        //读取的字符是数字
        else if (peek >= '0' && peek <= '9') {
            str += peek;
            // int flag = 0;  //用来判断之前字符串中有没有出现'.'，从而判断是否是浮点数
            while (1) {
                readch();
                if (isEnd()) break;
                if (peek >= '0' && peek <= '9') {
                    str += peek;
                }
                else {
                    break;
                }
            }
            tag = NUMINTOKENIZER;
        }
        //读取的字符是特殊字符
        else {
            str += peek;
            tag = SP;
            readch();
        }
        long end = pos - 1;
        if (end < size-1) {
            int p = end;
            while (buffer[p] == ' ' || buffer[p] == '\t' || buffer[p] == '\n') {
                p++;
                if (p == size-1) break;
            }
            to_next_token = p;
        } else {
            to_next_token = size-1;
        }
        return Token_new(curLine, tag, str, begin, end, to_last_token, to_next_token);
    }
    return Token_new(-1, -1, "EOF", -1, -1, -1, -1);
}

void Tokenizer::printToken(Token_new tok) {
    if (tok.str != "EOF")
              cout << tok.str <<"[" << tok.begin << "," << tok.end << ")" << " <" << tok.line << "> TAG:" << tok.tag << "{" << tok.to_last_token << ", " << tok.to_next_token << "}" << endl;
    }

long Tokenizer::getPos() {return pos;}
long Tokenizer::getSize() {return size;}
long Tokenizer::getCurline() {return curLine;}
char Tokenizer::getChar() {return peek;}
char* Tokenizer::getBuffer() {
    if (size == 0) {
        char *notContent = "there is no content in the file!";
         return notContent;

    }
    char *tmp = new char[size];  //申请另一块内存空间
    memset(tmp, 0, size);  //初始化
    strncpy(tmp, buffer, size);  //拷贝前size个字符到temp
    tmp += '\0';
    return tmp;
}
