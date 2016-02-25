#include "View.h"

// ViewColumn
ViewColumn::ViewColumn() {}
ViewColumn::ViewColumn(string name) {
	this->name = name;
}
ViewColumn::~ViewColumn() {
	indexContainer.clear();
	contentContainer.clear();
}

vector<string> ViewColumn::getContentContainer() {
	return contentContainer;
}
void ViewColumn::setContentContainer(vector<string> content) {
	contentContainer = content;
}
vector<vector<int> > ViewColumn::getIndexContainer() {
	return indexContainer;
}
void ViewColumn::setIndexContainer(vector<vector<int> > cn) {
	indexContainer = cn;
}
string ViewColumn::getName() {
	return name;
}
void ViewColumn::setName(string name) {
	 this->name = name;
}

ViewColumn::ViewColumn(const ViewColumn& c) {
	name = c.name;
	indexContainer = c.indexContainer;
	contentContainer = c.contentContainer;
}
ViewColumn& ViewColumn::operator=(const ViewColumn& c) {
	name = c.name;
	indexContainer = c.indexContainer;
	contentContainer = c.contentContainer;
	return *this;
}

void ViewColumn::addALine(int start, int end, string co) {
	vector<int> tempIndex;
	tempIndex.push_back(start);
	tempIndex.push_back(end);
	indexContainer.push_back(tempIndex);
	contentContainer.push_back(co);
}

// view 
View::View(){}
View::View(string name) {
	isNull = false;
	this->name = name;
}
View::View(const View& c) {
	isNull = c.isNull;
	name = c.name;
	alias = c.alias;
	col = c.col;
	index = c.index;
}
View& View::operator=(const View& c) {
	isNull = c.isNull;
	name = c.name;
	alias = c.alias;
	col = c.col;
	index = c.index;
	return *this;
}
View::~View() {
	col.clear();
}

void View::setName(string name) {
	 this->name = name;
}
void View::setAlias(string alias) {
	 this->alias = alias;
}
string View::getName() {
	return name;
}
string View::getAlias() {
	return alias;
}

ViewColumn View::getColumnByName(string colName) {
	//列不存在的时候返回null
	if ((this->index).empty() || (this->index).find(colName) == (this->index).end()) {
		ViewColumn useless("null");
		return useless;
	}
	int index = (this->index)[colName];

	return (this->col)[index];
}

int View::addColumnToView(ViewColumn tempCol) {
	//列已经重名了， 所以添加失败
	if (!(this->index).empty() && (this->index).find(tempCol.getName()) != (this->index).end()) {
		return 0;
	}
	col.push_back(tempCol);
	string colName = tempCol.getName();
	int size = col.size();
	this->index[colName] = col.size() - 1;
	// index.insert(pair<string, int>(colName, size));

	return 1;
}

vector<ViewColumn> View::getAllColumn() {
	return col;
}

bool View::isColumnExisted(string name) {
	map<string, int>::iterator it;
	for (it = index.begin(); it != index.end(); it++) {
		if (it->first == name) {
			return true;
		}
	}
	return false;
}

void View::setNull(bool bl) {
	isNull = bl;
}
bool View::getNull() {
	return isNull;
}

bool View::isViewHasNoCol() {
	if (index.empty() || index.size() == 0) {
		return true;
	} else {
		return false;
	}
}


