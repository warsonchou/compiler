
#include <iostream>
#include <fstream>
#include <string>
#include "View.h"
using namespace std;

int getNumberLen(int x) {
	int len = 0;
	if (x == 0) len = 1;
	while (x) {
		x = x / 10;
		len++;
	}
	return len;
}

int getColumn_Maxlen(ViewColumn v) {
	int max = 0;
	vector<string> content = v.getContentContainer();
	vector<vector<int> > t = v.getIndexContainer();
	for (int i = 0; i < content.size(); i++) {
		string temp = content[i];
		int len = temp.length() + getNumberLen(t[i][0])
			+ getNumberLen(t[i][1]);
		if (max < len) max = len;
	}
	return max + 6;
}

void print_View(View v) {    // 将结果输出到屏幕
	cout << "View: " << v.getName() << endl;
	vector<ViewColumn> allcolumn = v.getAllColumn();
	vector<int> each_Len;
	for (int i = 0; i < allcolumn.size(); i++) {
		each_Len.push_back(getColumn_Maxlen(allcolumn[i]));
	}

	cout << "+";
	for (int i = 0; i < allcolumn.size(); i++) {
		for (int j = 0; j < each_Len[i]; j++) {
			cout << '-';
		}
		cout << '+';
	}
	cout << endl;

	cout << '|';
	for (int i = 0; i < allcolumn.size(); i++) {
		cout << ' ' << allcolumn[i].getName();
		for (int j = 1; j < each_Len[i] - allcolumn[i].getName().length(); j++) {
			cout << ' ';
		}
		cout << '|';
	}
	cout << endl;

	cout << "+";
	for (int i = 0; i < allcolumn.size(); i++) {
		for (int j = 0; j < each_Len[i]; j++) {
			cout << '-';
		}
		cout << '+';
	}
	cout << endl;

	int flag = 0;
	while (flag != allcolumn[0].getIndexContainer().size()) {
		cout << '|';
		for (int i = 0; i < allcolumn.size(); i++) {
			int begin = allcolumn[i].getIndexContainer()[flag][0];
			int end = allcolumn[i].getIndexContainer()[flag][1];
			cout << ' ' << allcolumn[i].getContentContainer()[flag] << ":(";
			cout << begin << ',' << end << ") ";
			int len = allcolumn[i].getContentContainer()[flag].length()
				+ getNumberLen(begin) + getNumberLen(end) + 6;
			for (int j = 0; j < each_Len[i] - len; j++) {
				cout << ' ';
			}
			cout << '|';
		}
		cout << endl;
		flag++;
	}

	cout << "+";
	for (int i = 0; i < allcolumn.size(); i++) {
		for (int j = 0; j < each_Len[i]; j++) {
			cout << '-';
		}
		cout << '+';
	}
	cout << endl;
	
	cout << allcolumn[0].getIndexContainer().size() << " rows in set\n";
}

void print_View_Text(ofstream &file, View v) {  // 将结果写入到文件流中
	file << endl;
	file << "View: " << v.getName() << endl;
	vector<ViewColumn> allcolumn = v.getAllColumn();
	vector<int> each_Len;
	for (int i = 0; i < allcolumn.size(); i++) {
		each_Len.push_back(getColumn_Maxlen(allcolumn[i]));
	}
	file << "+";
	for (int i = 0; i < allcolumn.size(); i++) {
		for (int j = 0; j < each_Len[i]; j++) {
			file << '-';
		}
		file << '+';
	}
	file << endl;
	file << '|';
	for (int i = 0; i < allcolumn.size(); i++) {
		file << ' ' << allcolumn[i].getName();
		for (int j = 1; j < each_Len[i] - allcolumn[i].getName().length(); j++) {
			file << ' ';
		}
		file << '|';
	}
	file << endl;

	file << "+";
	for (int i = 0; i < allcolumn.size(); i++) {
		for (int j = 0; j < each_Len[i]; j++) {
			file << '-';
		}
		file << '+';
	}
	file << endl;
	if (v.isViewHasNoCol()) {
		file << "There is nothing in the view " << v.getName() << "!" << endl;
		return;
	}
	int flag = 0;
	while (flag != allcolumn[0].getIndexContainer().size()) {
		file << '|';
		for (int i = 0; i < allcolumn.size(); i++) {
			int begin = allcolumn[i].getIndexContainer()[flag][0];
			int end = allcolumn[i].getIndexContainer()[flag][1];
			file << ' ' << allcolumn[i].getContentContainer()[flag] << ":(";
			file << begin << ',' << end << ") ";
			int len = allcolumn[i].getContentContainer()[flag].length()
				+ getNumberLen(begin) + getNumberLen(end) + 6;
			for (int j = 0; j < each_Len[i] - len; j++) {
				file << ' ';
			}
			file << '|';
		}
		file << endl;
		flag++;
	}
	file << "+";
	for (int i = 0; i < allcolumn.size(); i++) {
		for (int j = 0; j < each_Len[i]; j++) {
			file << '-';
		}
		file << '+';
	}
	file << endl;
	
	file << allcolumn[0].getIndexContainer().size() << " rows in set\n";
}