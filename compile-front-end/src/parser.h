#ifndef PARSER_H
#define PARSER_H
#include "Lexer.h"
#include "Tokenizer.h"
#include "View.h"
#include "inter.h"
#include "regex.cpp"
#include "Output.cpp"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

// compelte the aql with top-to-down parsing
class parser {
private:
	Lexer* lex;
	Token lookahead;
	//Document view 默认是存在的
	View Document;

	// pattern 子句
	int patternGroupsNumber; //对组数进行记数
	int atomPosInAtomSet; //atomPosInAtomSet表示pattern关键字后面的atom集合
	vector<vector<Atom> > patternGroups; //记录组中各个Atom
	vector<int> atomGroupEnv; //代表现在atom在那个group里面, 刚开始的时候就有group0

	// regex 子句
	// 将不同的组号映射到不同的ID
	map<int, string> groupIdToNumber;
	string regex;
	//column -> ID.ID
	map<string, string> viewAndColumnInRegex;

	//from_list
	//view alias map example: ID ID
	map<string, string> viewAlias;

	//select list
	//select_item
	map<string, string> columnInView; //ID.ID
	map<string, string> viewAndColumnToAlias;

	//output statement
	string outViewName; //放输出view的真正的名字
	string outViewAlias; //放输出view的真正的别名， ""代表了没有alias

	//create statement
	string createdViewName; //放要创建的view的真正的名字

	//currentViewContainer 当前已经存在的view存放的地方
	vector<View> currentViewContainer;


	//get the handling docment content and tokenizer it
	Tokenizer *docTokens; //分词器
	map<int, int> docTokensIndex; //分词之后每个token在原文当中的索引：第一个表示每个单词的索引的开始，第二个表示每个索引的结束
	map<int, int> nextStartIndexOfCurrentTokens; //第一个表示当前token的结束索引，第二个表示下一个token的开始索引
	// 获取原文的内容，为char *类型
	char *sourceContent;
	//对于create语句，如果正确的话，最终会创建出一个view当中的column，然后在将column组装到相应的view中
	vector<ViewColumn> viewColumnContainer;

	//要输出的文件打印到目录和和文件名
	string outputFileName;
	ofstream outputFile;
public:
	parser();
	parser(string documentFileName, string aqlFilename);
	~parser();
	void move();
	void match(int t);
	// start aql stament
	void aql_stmt();
	void create_stmt();
	void view_stmt();
	void output_stmt();
	string alias();
	// select stament
	void select_stmt();
	void select_list();
	void rest_select_list();
	void select_item();
	void from_list();
	void rest_from_list();
	void from_item();
	// extract stament with regular expression
	void extract_stmt();
	void extract_spec();
	void regex_spec();
	vector<string> column();
	void name_spec();
	void group_spec();
	void rest_group_spec();
	void single_group();
	// extract stament with sequence pattern
	void pattern_spec();
	void pattern_expr();
	void rest_pattern_expr();
	void pattern_pkg();
	Atom atom();
	void pattern_group();
	//error handle function
	void errorHandle(int e);

	//auxiliary function
	void create_view();
	void output_view();
	bool isViewExisted(string name);
	bool isViewAliasExisted(string name);
	//在pattern的情况下判断两个两个span是否可以拼接，如果两个span之间只有空白符，空白符是被忽略的，被认为是可以连接起来的
	bool isTwoSpanConcat(int start, int end);
	View getViewByName(string name); 
	string getRealNameByAlias(string alias);
	void init(); //initial the private variables
	//和正则表达式是同等地位，不过它处理的的是pattern子句
	void patternMatcher();

	void begin_next_stmt(); //一句话解析完毕之后开始解析下一句
};

#endif
