#ifndef VIEW_H
#define VIEW_H
#include <string>
#include <vector>
#include <map>
#include <iostream>
using namespace std;

class ViewColumn
{
public:
	ViewColumn();
	ViewColumn(string name);
	//拷贝构造  
	ViewColumn(const ViewColumn& C);
	ViewColumn& operator=(const ViewColumn& C);
	~ViewColumn();
	vector<string> getContentContainer();
	void setContentContainer(vector<string> content);
	vector<vector<int> > getIndexContainer();
	void setIndexContainer(vector<vector<int> > cn);

	void setName(string name);
	string getName();

	//start
	void addALine(int start, int end, string co);
private:
	string name;
	vector<vector<int> > indexContainer; //the first elelment is the start in the document and the second is the end in the docment
	vector<string> contentContainer; //内容存放的地方
};


class View
{
private:
	bool isNull; //是否为空对象
	string name;
	string alias;
	vector<ViewColumn> col;
	map<string, int> index; //string代表column的name， int代表该column在col当中的index，形成映射关系
public:
	View();
	View(string name);
	View(const View& C);
	View& operator=(const View& other);
	~View();

	ViewColumn getColumnByName(string colName);
	bool isColumnExisted(string name);

	int addColumnToView(ViewColumn col);
	vector<ViewColumn> getAllColumn();

	void setName(string name);
	void setAlias(string alias);
	string getName();
	string getAlias();

	void setNull(bool bl); //判断一个view是否为空
	bool getNull();

	bool isViewHasNoCol();
};
#endif