#ifndef INTER_H
#define INTER_H
#include <string>

using namespace std;

class Atom
{
public:
	Atom();
	Atom(string	type);
	Atom(const Atom& C);
	Atom& operator=(const Atom& other);
	~Atom();
	string getType();
	string getFirstId();
	string getSecondId();
	string getRegEpx();
	string getToken();

	void setType(string str);
	void setFirstId(string str);
	void setSecondId(string str);
	void setRegEpx(string str);

	void setRepeated(bool repeat);
	bool getRepeated(); 
	void setRepeatTimes(int st, int ed);
	int getStartIndex();
	int getEndIndex();

	void setPosAtOfom(int pos);//表示当前的atom在原atom集合的位置，按先后顺序计数
	int getAtomPos();//获取atom在原atom集合当中的位置
private:
	string type; //表示Atom的类型, column => < column >, Token => < Token >, REG => REG
	/// for column
	string firstId;
	string secondId;
	// for Token
	string Token;
	//for REG
	string regEpx;
	//Atom{start ,end}这种形式
	bool isRepeated;
	int start, end;

	int pos; //表示atom在原atom集合当中的位置
};
#endif


