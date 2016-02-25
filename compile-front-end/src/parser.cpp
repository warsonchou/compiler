#include "parser.h"

parser::parser(string documentFileName, string aqlFilename) {
	//默认Document view是存在的， 其中有text列, 这里讲他们initial
	//to do list\ex = new Lexer(aliasName.c_str());
	docTokens = new Tokenizer(documentFileName.c_str());
	sourceContent = docTokens->getBuffer();
	lex = new Lexer((char *)aqlFilename.c_str());

	//获取输出的文件名的名字
	outputFileName = "";
	int endOfName;
	for (endOfName = documentFileName.size() - 1; endOfName >= 0; endOfName--) {
		if (documentFileName[endOfName] == '.')
			break;
	}
	for (int i = 0; i < endOfName; i++) {
		outputFileName += documentFileName[i];
	}
	outputFileName += ".output";
	//打开一个输出文件
	outputFile.open(outputFileName.c_str());
	if (outputFile.fail()) {
		cout << "Open File Fail!" << endl;
	}

	ViewColumn textCol("text");
	vector<int> textIndex;
	vector<vector<int> > textIndexContainer;
	textIndex.push_back(0);
	textIndex.push_back(docTokens->getSize() + 1);
	textIndexContainer.push_back(textIndex);
	textCol.setIndexContainer(textIndexContainer);
	string documentContent = docTokens->getBuffer();
	vector<string> documentContentContainer;
	documentContentContainer.push_back(documentContent);
	textCol.setContentContainer(documentContentContainer);
	Document.setName("Document");
	Document.addColumnToView(textCol);
	//将Document放进currentViewContainer里面
	currentViewContainer.push_back(Document);
	while (!docTokens->isEnd()) {
      Token_new tok = docTokens->scan();
      //to do list
      docTokensIndex[tok.getBeginIndex()] = tok.getEndIndex();
      nextStartIndexOfCurrentTokens[tok.getEndIndex()] = tok.getToNextToken();
  }
  //针对pattern子句
  atomPosInAtomSet = 0; //0表示第一个位置
	patternGroupsNumber = 0;
	atomGroupEnv.push_back(0);
	move();	
}

parser::parser(){};

parser::~parser() {
	// 释放动态生成的内存空间
	patternGroups.clear();
	atomGroupEnv.clear();
	groupIdToNumber.clear();
	viewAlias.clear();
	columnInView.clear();
	viewAndColumnToAlias.clear();
	outViewName.clear();
	outViewAlias.clear();

	//关闭输出文件
	outputFile.close();

	//删除掉动态分配的内存
	delete lex;
	delete docTokens;
}

void parser::move() {
	lookahead = lex->scan();
}

void parser::match(int t) {
	try {
		if (lookahead.getTag() == t) {
			move();
		} else {
			throw 0;
		}
	} catch (int e) {
		errorHandle(e);
	}
}
// start aql stament
void parser::aql_stmt() {
	init(); //initial the private variables
	try {
		switch(lookahead.getTag()) {
		case CREATE:
			create_stmt();
			match(SEMICOLON);
			create_view(); //解析正常，那么就创建一个view
			begin_next_stmt(); //开始下一句的解析
			break;
		case OUTPUT:
			output_stmt();
			match(SEMICOLON);
			output_view();
			begin_next_stmt(); //开始下一句的解析
			break;
		default:
			throw 0;
		}
	} catch(int e) {
		errorHandle(e);
	}
}

void parser::create_stmt() {
	match(CREATE);
	string name;
	match(VIEW);
	name = lookahead.str;
	match(IDENTIFIER);
	createdViewName = name;
	match(AS);
	view_stmt();
}

void parser::view_stmt() {
	//0代表
	try {
		switch(lookahead.getTag()) {
			case SELECT:
				select_stmt();
				break;
			case EXTRACT:
				extract_stmt();
				break;
			default:
				throw 0;
		}
	} catch(int e) {
		errorHandle(e);
	}
}

void parser::output_stmt() {
	string name, tempAlias;
	match(OUTPUT);
	match(VIEW);
	name = lookahead.str;
	match(IDENTIFIER);
	outViewName = name;
	if (lookahead.getTag() == AS) {
		tempAlias = alias();
		outViewAlias = tempAlias;
	}
}
string parser::alias() {
	string aliasName = "";
	try {
		if (lookahead.getTag() == AS) {
			match(AS);
			aliasName = lookahead.str;
			match(IDENTIFIER);
		}
		return aliasName;
	} catch (int e) {
		errorHandle(e);
		return aliasName;
	}
}

// select stament
void parser::select_stmt() {
	match(SELECT);
	select_list();
	match(FROM);
	from_list();

	try {
		//检测view是否存在
		map<string, string>::iterator it, itForAlias;
		for (it = viewAlias.begin(); it != viewAlias.end(); it++) {
			if (!isViewExisted(it->first))
				throw 0;
		}

		// ID.ID检测是否第一个ID存在
		for (it = columnInView.begin(); it != columnInView.end(); it++) {
			bool flag = false;
			for (itForAlias = viewAlias.begin(); itForAlias != viewAlias.end(); itForAlias++) {
				if (itForAlias->second == it->first) {
					View viewInstance = getViewByName(itForAlias->first);
					//如果view没有列的话那就直接返回了
					if (viewInstance.isViewHasNoCol()) {
						return;
					}
					if (viewInstance.getNull()) {
						throw 0; //不存在ID ID当中的第一个ID的view
					} else {
						//ID.ID检测是否第二个ID存在
						if (!viewInstance.isColumnExisted(it->second)) 
							throw 0;
						//获取viewInstance当中的列
						ViewColumn vcol = viewInstance.getColumnByName(it->second);
						//如果别名存在的那么久重名命名, ""代表没有别名
						string hasAlias = viewAndColumnToAlias[it->first + "." + it->second];
						if ("" != hasAlias) { 
							vcol.setName(hasAlias);
						}
						viewColumnContainer.push_back(vcol);
					}
					flag = true;
					break;
				}
			}
			//ID.ID的第一个ID不存在 ID ID中
			if (!flag) {
				throw 0;
			}
		}
	} catch (int e) {
		errorHandle(e);
	}
}
// 注意到左递归消除
void parser::select_list() {
	select_item();
	rest_select_list();
}
//使用了尾递归消除
void parser::rest_select_list() {
	if (lookahead.getTag() == COMMA) {
		while(1) {
			if (lookahead.getTag() == COMMA) {
				match(COMMA);
				select_item();
				continue;
			}
			break;
		}
	}
}

void parser::select_item() {
	string view, column;
	view = lookahead.str;
	match(IDENTIFIER);
	match(DOT);
	column = lookahead.str;
	match(IDENTIFIER);
	columnInView[view] = column; //记录是哪个view里面的column
	string tempAlias = alias();
	viewAndColumnToAlias[view + "." + column] = tempAlias; //使用view + column来确定其实有alias name，如果有则alias name不为“”
}
// 采取和select_list函数一样的做法
void parser::from_list() {
	from_item();
	rest_from_list();
}
void parser::rest_from_list() {
	if (lookahead.getTag() == COMMA) {
		while(1) {
			if (lookahead.getTag() == COMMA) {
				match(COMMA);
				from_item();
				continue;
			}
			break;
		}
	}
}

void parser::from_item() {
	// 将view映射到别名空间
	string source, dest;
	source = lookahead.str;
	match(IDENTIFIER);
	dest = lookahead.str;
	match(IDENTIFIER);
	viewAlias[source] = dest;
}

// extract stament with regular expression
void parser::extract_stmt() {
	match(EXTRACT);
	int tag = lookahead.getTag();
	extract_spec();
	match(FROM);
	from_list();
	if (tag == REGEX) {
	//这里和select list相似了，冗余了，to do list, 后面在考虑优化了
		try {
			//检测view是否存在， from_item -> ID ID 其中第一个就是要检测的ID
			map<string, string>::iterator it, itForAlias;
			for (it = viewAlias.begin(); it != viewAlias.end(); it++) {
				if (!isViewExisted(it->first)) {
					throw 0;
				}
			}
			//regex为要提取的正则表达式，计算其中的(得到到底有多少个捕获组
			int catchedGroupNumber = 0;
			for (int i = 0; i < 0; i++) {
				if (regex[i] == '(') {
					catchedGroupNumber++;
				}
			}
			//single_group → group NUM as ID 检测NUM是否合法，边检测边创建列（ViewColumn）创建出来，并将它放入viewColumnContainer中
			map<int, string>::iterator intWithStringIt;
			//将int和ViewColumn一一对应起来
			map<int, ViewColumn> viewColumnMap;
			bool isGroupNumerMapToIDCorrect = true;
			for (intWithStringIt = groupIdToNumber.begin(); intWithStringIt != groupIdToNumber.end(); intWithStringIt++) {
				if (intWithStringIt->first > catchedGroupNumber) { //组号大于捕获的总组号，发生错误
					isGroupNumerMapToIDCorrect = false;
					break;
				}
				//proably error
				ViewColumn tempColInstance(intWithStringIt->second);
				viewColumnMap[intWithStringIt->first] = tempColInstance;
			}
			if (!isGroupNumerMapToIDCorrect) {
				throw 0;
			}
			// ID.ID检测是否第一个ID存在
			for (it = viewAndColumnInRegex.begin(); it != viewAndColumnInRegex.end(); it++) {
				bool flag = false;
				for (itForAlias = viewAlias.begin(); itForAlias != viewAlias.end(); itForAlias++) {
					if (itForAlias->second == it->first) {
						View viewInstance = getViewByName(itForAlias->first);
						if (viewInstance.getNull()) {
							throw 0; //不存在ID ID当中的第一个ID的view
						} else {
							//ID.ID检测是否第二个ID存在
							if (!viewInstance.isColumnExisted(it->second)) {
								throw 0;
							}
							//获取viewInstance当中的列
							ViewColumn vcol = viewInstance.getColumnByName(it->second);
							vector<string> vcolLine = vcol.getContentContainer(); //取出当前列所以行的字符串
							vector<vector<int> > indexOfvcolLine = vcol.getIndexContainer(); //同时取出每一在最初的原文当中的开始索引和结束索引
							int size = vcolLine.size(); //得到当前列当中的行数
							vector<vector<int> > result; //存放正则表达式的匹配的结果
							//将一列中的每行捕获的组都分别添加到相应新的列中
							//regex_spec -> regex REG on column name_spec
							for (int j = 0; j < size; j++) { //遍历每一行，并对其进行正则表达式的匹配，进行捕获
								const char * mystr = vcolLine[j].c_str();
								const char * myregex = regex.c_str();
								//vcolLine[j]在原文的开始索引
								int start = indexOfvcolLine[j][0];
								result = findall(myregex, mystr);
								vector<string> tempContentContainer; //汇总每一行捕获的字符串到每一列中
								vector<vector<int> > indexContainer;//汇总每一行捕获的字符串索引到每一列中
								for (int u = 0; u < result.size(); u++) {
									//pro error
									int whichCatchedGroup = 0;
									for (int y = 0; y < result[0].size(); y+=2) {
										whichCatchedGroup++;
										//如果组号为u的虽然捕获了，但是没有作为列返回，则忽略
										if (viewColumnMap.find(whichCatchedGroup - 1) == viewColumnMap.end())
											continue;
										//获取每个捕获块的捕获的内容
										string catchedContent = "";
										for(int k = result[u][y]; k < result[u][y + 1]; k++) {
											catchedContent += *(mystr + k);
										}
										//findall找到的start和end只是在当前字符串的索引，要加上原来字符串所在的原文开始索引才是找到的字符串在原文的索引
										viewColumnMap[whichCatchedGroup - 1].addALine(result[u][y] + start, result[u][y + 1] + start, catchedContent);
									}
								}
							}
							//填充好每一列之后，然后就将列放在了viewColumnContainer中了
							for(map<int, ViewColumn>::iterator it = viewColumnMap.begin(); it != viewColumnMap.end(); it++) {
								viewColumnContainer.push_back(it->second);
							}

						}
						flag = true;
						break;
					}
				}
				//ID.ID的第一个ID不存在 ID ID中
				if (!flag) {
					throw 0;
				}
			}
		} catch (int e) {
			errorHandle(e);
		}
	} else {
	//这里和select list相似了，冗余了，to do list, 后面在考虑优化了
		try {
			//检测view是否存在， from_item -> ID ID 其中第一个就是要检测的ID
			map<string, string>::iterator it, itForAlias;
			for (it = viewAlias.begin(); it != viewAlias.end(); it++) {
				if (!isViewExisted(it->first))
					throw 0;
			}
			//atom -> < column > | < Token > | REG
			//pattern_group -> ( pattern_expr )
			// //第0个捕获组包括了所有的atom，所以使用它来检测column -》 ID . ID当中的第一个ID是否存在
			for (int i = 0; i < patternGroups[0].size(); i++) {
				if (patternGroups[0][i].getType() == "column") {
					if (isViewAliasExisted(patternGroups[0][i].getFirstId())) {
						string realname = getRealNameByAlias(patternGroups[0][i].getFirstId());
						View tempView = getViewByName(realname);
						//view当中没有列，那么就直接返回了
						if (tempView.isViewHasNoCol())
							return;
						//column -》 ID . ID当中的第二个ID是否存在
						if (!tempView.isColumnExisted(patternGroups[0][i].getSecondId())) {
							throw 0;
						}
					} else {
						throw 0;
					}
				}
			}
			//检测Token是否在前头和尾部
			if (patternGroups[0][0].getType() == "Token" || patternGroups[0][patternGroups[0].size() - 1].getType() == "Token") {
				throw 0;
			}
			//正常则解析
			patternMatcher();
		} catch (int e) {
			errorHandle(e);
		}
	}

}

void parser::extract_spec() {
	try {
		switch(lookahead.getTag()) {
			case REGEX:
				regex_spec();
				break;
			case PATTERN:
				pattern_spec();
				break;
			default:
				throw 0;
		}
	} catch(int e) {
		errorHandle(e);
	}
}

void parser::regex_spec() {
	match(REGEX);
	if (lookahead.getTag() == REG_EXP) {
		regex = lookahead.str;
	}
	match(REG_EXP);
	match(ON);
	vector<string> viewAndCol = column();
	viewAndColumnInRegex[viewAndCol[0]] = viewAndCol[1];
	name_spec();
}

vector<string> parser::column() {
	// collect the id and id in ID.ID
	vector<string> container;

	if (lookahead.getTag() == IDENTIFIER)
		container.push_back(lookahead.str);

	match(IDENTIFIER);
	match(DOT);
	if (lookahead.getTag() == IDENTIFIER)
		container.push_back(lookahead.str);

	match(IDENTIFIER);

	if (!container.empty() && container.size() < 2) {
		container.clear();
	}
	return container;
}

void parser::name_spec() {
	try {
		switch(lookahead.getTag()) {
			//只有一个gourp的时候
			case AS:
			{
				// 记录id
				string tempId;
				match(AS);
				tempId = lookahead.str;
				match(IDENTIFIER);
				//添加group映射
				groupIdToNumber[0] = tempId;
				break;
			}
			case RETURN:
				match(RETURN);
				group_spec();
				break;
			default:
				throw 0;
		}
	} catch(int e) {
		errorHandle(e);
	}
}
// 左递归消除
void parser::group_spec() {
	single_group();
	rest_group_spec();
}

void parser::rest_group_spec() {
		if(lookahead.getTag() == AND) {
			while(1) {
				if(lookahead.getTag() == AND) {
					match(AND);
					single_group();
					continue;
				}
				break;
			}
		}
}
void parser::single_group() {
	match(GROUP);
	string tempNumber = lookahead.str;
	match(NUM);
	match(AS);
	string tempId = lookahead.str;
	match(IDENTIFIER);
	// 将组号和名字形成一一对应
	groupIdToNumber[atoi(tempNumber.c_str())] = tempId;
}
// extract stament with sequence pattern
void parser::pattern_spec() {
	match(PATTERN);
	pattern_expr();
	name_spec();
}
//左递归消除
void parser::pattern_expr() {
	//只有档lookahead是<, (, REG_EXP的时候，其他情况都是这种情况当中的情况
	while (lookahead.tag == L_BRACKET || lookahead.tag == L_ABRACKET || lookahead.tag == REG_EXP) {
		pattern_pkg();
	}
}

void parser::pattern_pkg() {
	try {
		Atom temp;
		switch(lookahead.getTag()) {
			case L_ABRACKET:
			{
				temp = atom();
				if(lookahead.getTag() == L_BRACE) {
					// atom {start, end}
					string startStr, endStr; //记录做为string类型的时候的start和end
					match(L_BRACE);
					startStr = lookahead.str;
					match(NUM);
					match(COMMA);
					endStr = lookahead.str;
					match(NUM);
					match(R_BRACE);
					int s = atoi(startStr.c_str()), e = atoi(endStr.c_str());
					//检测{num1, num1}当中才num是否合理
					//num为非正整数的时候
					if (s <= 0 || e <= 0)
						throw 0;
					//num1 > num2
					if (s > e)
						throw 0;
					//除了{1 , 1}的情况外
					if (!(s == 1 && e == 1)) {
						temp.setRepeatTimes(s, e);
					} 
				}
				// 将当前得到的atom将当前环境的当中的group
				for (int i = 0; i < atomGroupEnv.size(); i++) {
					//atom 为当捕获组的第一个的atom
					if (patternGroups.size() < atomGroupEnv[i] + 1) {
						vector<Atom> tempAtomContainer;
						tempAtomContainer.push_back(temp);
						patternGroups.push_back(tempAtomContainer);
					} else {
						patternGroups[atomGroupEnv[i]].push_back(temp);
					}
				}
				break;
			}
			case REG_EXP:
			{
				temp = atom();
				// 将当前得到的atom将当前环境的当中的group
				for (int i = 0; i < atomGroupEnv.size(); i++) {
					//atom 为当捕获组的第一个的atom
					if (patternGroups.size() < atomGroupEnv[i] + 1) {
						vector<Atom> tempAtomContainer;
						tempAtomContainer.push_back(temp);
						patternGroups.push_back(tempAtomContainer);
					} else {
						patternGroups[atomGroupEnv[i]].push_back(temp);
					}
				}
				break;
			}
			case L_BRACKET:
				match(L_BRACKET);
				// memerize the catched group number
				patternGroupsNumber++;
				atomGroupEnv.push_back(patternGroupsNumber); //因为环境改变了，所以将当前组加入当前环境
				pattern_group();
				match(R_BRACKET);
				atomGroupEnv.pop_back(); //移除当前不属于当前环境的 patternGroups
				break;
			default:
				throw 0;
		}
	} catch(int e) {
		errorHandle(e);
	}
}

Atom parser::atom() {
	Atom at;
	try {
		switch(lookahead.getTag()) {
			case L_ABRACKET:
			{
				match(L_ABRACKET);
				switch(lookahead.getTag()) {
					case IDENTIFIER:
					{
						vector<string> temp = column();
						if (temp.size() == 2) {
							Atom col("column");
							col.setFirstId(temp[0]);
							col.setSecondId(temp[1]);
							at = col;
						}
						match(R_ABRACKET);
						break;

					}
					case TOKEN:
					{
						Atom atomToken(lookahead.str);
						at = atomToken;
						match(TOKEN);
						match(R_ABRACKET);
					}
						break;
					default:
						throw 0;
				}
			}
				break;
			case REG_EXP:
			{
				Atom atomRegEpx("REG");
				at = atomRegEpx;
				string reg_expr = lookahead.str;
				match(REG_EXP);
				at.setRegEpx(reg_expr);
				break;
			}
			default:
				throw 0;
		}
		//设置当前atom在原atom集合当中的位置
		at.setPosAtOfom(atomPosInAtomSet);
		atomPosInAtomSet++;
		return at;
	} catch(int e) {
		errorHandle(e);
		return at;
	}
}

void parser::pattern_group() {
	pattern_expr();
}

//error handle function
void parser::errorHandle(int e) {
	if (e == 0) {
		printf("Tag Error!\n");
	}
}

//auxiliary function
void parser::create_view() {
	View newView(createdViewName);
	for (int i = 0; i < viewColumnContainer.size(); i++) {
		if (!newView.addColumnToView(viewColumnContainer[i])) {
			throw 0; //存在列重名
		}
	}
	currentViewContainer.push_back(newView);
}
void parser::output_view() {
	try {
		int size = currentViewContainer.size(), i;
		for (i = 0; i < size; i++) {
			if (currentViewContainer[i].getName() == outViewName && currentViewContainer[i].getAlias() == outViewAlias) {
				break;
			}
		}
		if (i == size)
			throw 0;
		print_View_Text(outputFile, currentViewContainer[i]);
	} catch(int e) {
		errorHandle(e);
	}
}

bool parser::isViewExisted(string name) {
	for (int i = 0; i < currentViewContainer.size(); i++) {
		if (currentViewContainer[i].getName() == name)
			return true;
	}
	return false;
}

bool parser::isViewAliasExisted(string aliasName) {
	for (map<string, string>::iterator it = viewAlias.begin(); it != viewAlias.end(); it++) {
		if (it->second == aliasName) {
			return true;
		}
	}
	return false;
}


View parser::getViewByName(string name) {
	for (int j = 0; j < currentViewContainer.size(); j++) {
		if (currentViewContainer[j].getName() == name) {
			currentViewContainer[j].setNull(false);
			return currentViewContainer[j];
		}
	}
	View value("null");
	value.setNull(true);
	return value;
}

string parser::getRealNameByAlias(string alias) {
	map<string, string>::iterator it = viewAlias.begin();
	for (; it != viewAlias.end(); it++) {
		if (it->second == alias)
			return it->first;
	}
	string isInvalid = "Invalid";
	return isInvalid;
}

bool parser::isTwoSpanConcat(int start, int end) {
	bool isConcat = false;
	for(int g = start; g <= end; g++) {
		if (*(sourceContent + g) == ' ' || *(sourceContent + g) == '\r' || *(sourceContent + g) == '\t' || *(sourceContent + g) == '\n') {
			continue;
		} else {
			if (g == end)
				isConcat = true;
			else
				break;
		}
	}
	return isConcat;
}

void parser::init() {
	patternGroupsNumber = 0;
	atomPosInAtomSet = 0;
	
	if (!patternGroups.empty())
		patternGroups.clear();
	if (!atomGroupEnv.empty()) {
		atomGroupEnv.clear();
		atomGroupEnv.push_back(0);
	}
	if (!groupIdToNumber.empty())
		groupIdToNumber.clear();
	if (!viewAndColumnInRegex.empty())
		viewAndColumnInRegex.clear();
	regex = "";

	if (!viewAlias.empty())
		viewAlias.clear();
	if (!columnInView.empty())
		columnInView.clear();
	if (!viewAndColumnToAlias.empty())
		viewAndColumnToAlias.clear();
	if (!outViewName.empty())
		outViewName.clear();
	if (!outViewAlias.empty())
		outViewAlias.clear();

	createdViewName = "";
	
	if (!viewColumnContainer.empty())
		viewColumnContainer.clear();
}

void parser::patternMatcher() {
	//pattern模式最复杂的情况：(<P.Per>{1，2}) <Token>{1,2} REG{1，2} (<L.Loc>){1，2}，
	//这里Token有可能为其他列的值，为了使句子更加实用，
	//atom{NUM, NUM}采取不贪婪匹配, 也就是匹配能多小就多小
	//处理< column > REG < Token > 
	//这里将patternGroups[0](atom)当中的atom映射成 vector<vector<int> >
	//所以就变成了vector<vector<vector<int> > >相当于一个动态的二位数组
	//atom相当于一个不正规的列，所以最开始的时候，最里面的vector也即vector<int>是一个长度为2的
	//但是后面拓展成一个大于或者等于2的vector<int>,因为要记录每个atom在一个符合pattern的要求的字符串里面的开始和结束索引
	vector<vector<int> > atomTOIndexContainer;
	//第0个捕获组默认是包含了所有的捕获组，所以先选择第0个不捕获组来处理得到一列，其他的列都依赖于这个组捕获的内容
	//先找前两个vector<atom>和并成一个vector<atom>, 然后合并新的前两个vector<aton>
	//一直迭代下去，最终得到了一个vector<atom>
	int loops = patternGroups[0].size();
	for (int i = 0; i < loops; i++) {
		Atom tempAtom = patternGroups[0][i];
		vector<vector<int> > tempAtomIndex;
		View tempViewInAtom;
		ViewColumn tempColInAtom;
		//这部分解决的是{num, num}这种情况
		//每重复一次就放在一个vector<vector<int> >里面，最后将它们收集起来
		vector<vector<vector<int> > > eachRepeatTimesIndexSet;
		
		if (tempAtom.getType() == "REG") {
			//REG 的内容只一个形式而已，真正的内容还没有提取出来，
			//需要从原文提取出来,这里提取只是其内容在原文当中的start和end
			//to do list
			//这里获取到的regexCotent相当于{1，1}的情况
			vector<vector<int> > regexContent = findall((tempAtom.getRegEpx()).c_str(), sourceContent);
			//这里面regexContent的vector<int>长度大于2，所以将其赋值给tempAtomIndex，其中tempAtomIndex当中的vector<int>长度为2，表示开始索引和结束索引
			for (int j = 0; j < regexContent.size(); j++) {
				vector<int> startAndEnd;
				startAndEnd.push_back(regexContent[j][0]);
				startAndEnd.push_back(regexContent[j][1]);
				tempAtomIndex.push_back(startAndEnd);
			}
		} else if (tempAtom.getType() == "column") {
			string viewNameOfTempAtom = getRealNameByAlias(tempAtom.getFirstId()), colNameOfTempAtom = tempAtom.getSecondId();
			View tempViewOfTempAtom = getViewByName(viewNameOfTempAtom);
			ViewColumn tempColOfTempAtom = tempViewOfTempAtom.getColumnByName(colNameOfTempAtom);
			tempAtomIndex = tempColOfTempAtom.getIndexContainer();
		}

		//先把重复一次的atom放进eachRepeatTimesIndexSet，之后把重复两次，三次。。。的atom也放进eachRepeatTimesIndexSet
		eachRepeatTimesIndexSet.push_back(tempAtomIndex);

		//Token和REG，column的处理过程不同
		//由于Token{1,2}情况太多，如果按照下面的方式 处理的话，会产生的爆炸式增长，
		if (tempAtom.getType() != "Token" && tempAtom.getRepeated()) {
			int repeatStartTimes = tempAtom.getStartIndex(), repeatEndTimes = tempAtom.getEndIndex();
			//这里相当于{1,endTimes}
			for (int j = 2; j <= repeatEndTimes; j++) {
				//就是重复次数大于j - 1次的atom不存在，那么j, j+1, j+2就也不存在了
				if (j - 1 > eachRepeatTimesIndexSet.size())
					continue;
				//计算可重复的times，如果在原文当中符合，那么久放在tempAtomIndex的后面
				vector<vector<int> > currentMostTimesAtom = eachRepeatTimesIndexSet[j - 2], nextMostTimesAtom;
				int oneTimesSize = tempAtomIndex.size(), mostTimesSize = currentMostTimesAtom.size();
				for (int k = 0; k < mostTimesSize; k++) {
					int endOfUnit = currentMostTimesAtom[k][1]; //endofUni相当于{start,end}中的end
					for (int u = 0; u < oneTimesSize; u++) {
						//{1,2}和{2,5}这种情况或者是{1,2}和{4,5}（其中第3,4个为\n或者\t或者空格的情况）
						if (isTwoSpanConcat(endOfUnit, tempAtomIndex[u][0])) {
							vector<int> appendStartAndEnd;
							appendStartAndEnd.push_back(currentMostTimesAtom[k][0]);
							appendStartAndEnd.push_back(tempAtomIndex[u][1]);
							nextMostTimesAtom.push_back(appendStartAndEnd);
						}
					}
				}
				eachRepeatTimesIndexSet.push_back(nextMostTimesAtom);
			}
			//情况1：重复的次数增加了一次之后就把之前的次数删除掉{1，starttimes}
			//情况2：{startTimes, endTimes}和{1,startTimes}，只不过这里不删除掉小于starttimes的元素
			//这里就将eachRepeatTimesIndexSet当中重复次数大于等于startTimes的集成到tempAtomIndex中
			tempAtomIndex.clear();
			for (int i = repeatStartTimes - 1; i < eachRepeatTimesIndexSet.size(); i++) {
				for (int j = 0; j < eachRepeatTimesIndexSet[i].size(); j++) {
					tempAtomIndex.push_back(eachRepeatTimesIndexSet[i][j]);
				}
			}
		}
		//这里也就在重复{startTimes, endTimes}当中的startTimes之后，没有atom符合重复startTimes次，所以相当于匹配失败
		//也即是说atom{startTimes, endTimes匹配失败,针对REG和<P.per>这种情况
		if (tempAtom.getType() != "Token" && tempAtomIndex.empty()) {
			break;
		}
		//经过上面的处理后，一个atom就映射成了一个vector<int>,将它放入到atomTOIndexContainer
		//有两个以上的atom才进行合并成一个atom，就是相当于将两个vector<int>拼接成一个vector<int>
		//也就是重复一次，只不过这里不是真正的重复，因为两个atom不同
		if (i >= 1) {
			int atomTOIndexContainerSize = atomTOIndexContainer.size();
			vector<vector<int> > atomIndexList;
			for(int j = 0; j < atomTOIndexContainerSize; j++) {
				int listSize = atomTOIndexContainer[j].size();
				//REG{1,3}或者<p.l>{3,7}这种情况
				if (tempAtom.getType() == "REG" || tempAtom.getType() == "column") {
					int atomSize = tempAtomIndex.size();
					for (int k = 0; k < atomSize; k++) {
						//拼接的情况，中间隔了空白符夜算是链接在一起的
						if(isTwoSpanConcat(atomTOIndexContainer[j][listSize - 1], tempAtomIndex[k][0])) {
							atomIndexList.push_back(atomTOIndexContainer[j]);
							atomIndexList[atomIndexList.size() - 1].push_back(tempAtomIndex[k][0]);
							atomIndexList[atomIndexList.size() - 1].push_back(tempAtomIndex[k][1]);
						}
					}
					for (int e = 0; e < atomIndexList.size(); e++) {
				}
				} else if (tempAtom.getType() == "Token") { //<Token>{2,6}这种情况
					int formTimes = 1, toTimes = 1, documentSize = docTokens->getSize();
					if (tempAtom.getRepeated()) {
						formTimes = tempAtom.getStartIndex();
						toTimes = tempAtom.getEndIndex();
					}
					//对于每一条atomTOIndexContainer,如果重复次数大于或等于formTimes的话那么就添加到atomIndexList
					int currentTokenEndIndex = atomTOIndexContainer[j][listSize - 1], nextTokenStartIndex = nextStartIndexOfCurrentTokens[currentTokenEndIndex];
					//如果当前的已经到文件末尾，那么就退出，匹配失败
					if (nextTokenStartIndex < documentSize - 1) {
						atomTOIndexContainer[j].push_back(nextTokenStartIndex);
						atomTOIndexContainer[j].push_back(docTokensIndex[nextTokenStartIndex]);
						if (formTimes == 1) {
							atomIndexList.push_back(atomTOIndexContainer[j]);
						}
						currentTokenEndIndex = docTokensIndex[nextTokenStartIndex];

						for (int y = 2; y <= toTimes; y++) {
							nextTokenStartIndex = nextStartIndexOfCurrentTokens[currentTokenEndIndex];
							//到达了文件末尾了，没有东西可以匹配了
							if (nextTokenStartIndex >= documentSize -1)
								continue;
							atomTOIndexContainer[j].pop_back();
							atomTOIndexContainer[j].push_back(docTokensIndex[nextTokenStartIndex]);
							if (y >= formTimes) {
								atomIndexList.push_back(atomTOIndexContainer[j]);
							}
							currentTokenEndIndex = docTokensIndex[nextTokenStartIndex];
						}
					}
				}
			}
			atomTOIndexContainer = atomIndexList;
			//也就是相当于到当前的atom和之前的atom没有相连的地方，那么就退出了，匹配失败
			if (atomTOIndexContainer.empty()) {
				break;
			}
		} else {
			atomTOIndexContainer = tempAtomIndex;
		}
	}
	//如果atomTOIndexContainer长度为0，那么也就没有匹配的内容了，则不进行下面的步骤
	if (atomTOIndexContainer.empty())
		return;
	//将vector<vector<Atom> >patternGroups映射成一个vector<vector<vector<int> > >
	vector<vector<vector<int> > > catchedGroups;
	//对atomTOIndexContainer进行操作，将它其中的每个atom提取出来，并将它放到相应的捕获组里面
	for (int i = 0; i < atomTOIndexContainer.size(); i++) {
		//如果atomTOIndexContainer[i]进行匹配途中失败了，那么atomTOIndexContainer[i]的size就会小于loops + 1
		if (atomTOIndexContainer[i].size() != loops * 2)
			continue;
		for (int j = 0; j < loops; j++) {
			//获取第j个atom的位置
			int posOfAtom = patternGroups[0][j].getAtomPos();
			int patternGroupsSize = patternGroups.size();
			//按顺序将每个组捕获组遍历一次,将每个必要的信息填写到捕获组里面
			
			for (int k = 0; k < patternGroupsSize; k++) {
				vector<Atom> atomInAGroup = patternGroups[k];
				//检测当前的atom是在grous[k]当中
				int isInThisGroups = false;
				for (int u = 0; u < atomInAGroup.size(); u++) {
					if (posOfAtom == atomInAGroup[u].getAtomPos()) {
						isInThisGroups = true;
						break;
					}
				}
				//如果改atom在捕获组里面，那么就修改第k个捕获组当中的第i条信息的结束索引
				if (isInThisGroups) {
					//如果当前的atom是捕获组的第一个atom，那么就添加该捕获组到catchedGroups中
					if (catchedGroups.empty() || catchedGroups.size() - 1 < k) {
						vector<vector<int> > firstlyInAAtom;
						vector<int> firstIndexInAAtom;
						firstIndexInAAtom.push_back(atomTOIndexContainer[i][2 * j]);
						firstIndexInAAtom.push_back(atomTOIndexContainer[i][2 * j + 1]);
						firstlyInAAtom.push_back(firstIndexInAAtom);
						catchedGroups.push_back(firstlyInAAtom);
					} else {
						//如果当前的vector<vector<int > >只有i-1个vector<int>也即是只有i-1条信息
						//那么就得添加一条信息在里面
						if (catchedGroups[k].empty() || catchedGroups[k].size() - 1 < i) {
							vector<int> itemInaCathedgroups;
							itemInaCathedgroups.push_back(atomTOIndexContainer[i][2 * j]);
							itemInaCathedgroups.push_back(atomTOIndexContainer[i][2 * j + 1]);
							catchedGroups[k].push_back(itemInaCathedgroups);
						} else {
							//如果不是，那么久直接修改第k个捕获组当中的第i条信息结束索引
							catchedGroups[k][i][1] = atomTOIndexContainer[i][2 * j + 1];
						}
					}
					
				}
			}
		}
		
	}
	//匹配失败
	if (catchedGroups.empty())
		return;
	//然后就是创建列了
	for (map<int, string>::iterator it = groupIdToNumber.begin(); it != groupIdToNumber.end(); it++) {
		string groupNameFormInt = groupIdToNumber[it->first];
		ViewColumn tempViewCol(groupNameFormInt);
		for (int k = 0; k < catchedGroups[it->first].size(); k++) {
			string content = "";
			for (int u = catchedGroups[it->first][k][0]; u < catchedGroups[it->first][k][1]; u++)
				content += *(sourceContent + u);
			tempViewCol.addALine(catchedGroups[it->first][k][0], catchedGroups[it->first][k][1], content);
		}
		viewColumnContainer.push_back(tempViewCol);
	}
}

void parser::begin_next_stmt() {
 //一句话解析完毕之后开始解析下一句
	if (!lex->isEndOfFile()) {
		aql_stmt();
	}
}


int main() {
	string docFilename, aqlFilename;
	cout << "请输入你处理文本的aql文件名字：" << endl;
	while(cin >> aqlFilename) {
		cout << "请输入你要处理的处理文章的文件名字或者你的文件集所在的目录："  << endl;
		cin >> docFilename;
		ofstream isFileexisted(docFilename.c_str(), ios::app);
		//试图打开文件，如果打开失败，则文件不存在
		if (isFileexisted.fail()) {
			//引用dirent.h来获取目的相应信息
			DIR *dp = opendir(docFilename.c_str());
			if (dp != NULL) {
				string filenames[20]; //储存的目录下要处理的文件集合
				int whichFiles = 0;
				struct dirent * dirInfo;
				//获取目录下所有文件名字
				while ((dirInfo = readdir(dp)) != NULL) {
					if (strcmp(dirInfo->d_name, ".") == 0 || strcmp(dirInfo->d_name, "..") == 0 )  
						continue;
					string tempName = dirInfo->d_name;
					int j, k, sizeOfTempName = tempName.size();
					for (j = sizeOfTempName - 1; j >= 0; j--) {
						if (tempName[j] == '.')
							break;
					}
					string ext(tempName, j + 1, (sizeOfTempName - j));
					//以output结尾的文件忽略掉，因为是输出文件
					if (ext != "output") {
						filenames[whichFiles] = tempName;
						whichFiles++;
					}
				}
				closedir(dp); //关闭打开的目录
				for (int j = 0; j < whichFiles; j++) {
					//如果文件存在目录，那么就处理该该文件，并将盖文件相应的输出结果之余目录
					string realNamOfDoc = filenames[j];
					string lastName = docFilename + "/" + realNamOfDoc;
					parser p(lastName, aqlFilename);
					p.aql_stmt();
				}
			}
		} else {
			isFileexisted.close();
			//文件存在，那么就就处理文件
			parser p(docFilename, aqlFilename);
			p.aql_stmt();
		}
		cout << "请输入你处理文本的aql文件名字：" << endl;
	}
	return 0;
}