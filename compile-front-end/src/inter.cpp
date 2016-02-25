#include "inter.h"

Atom::Atom() {
	firstId = "";
	secondId = "";
	Token = "";
	regEpx = "";
}

Atom::Atom(string tp) {
	type = tp;
	firstId = "";
	secondId = "";
	Token = "";
	regEpx = "";
	start = end = 0;
	isRepeated = false;
}
Atom::Atom(const Atom& c) {
	type = c.type;
	firstId = c.firstId;
	secondId = c.secondId;
	Token = c.Token;
	regEpx = c.regEpx;
	start = c.start;
	end = c.end;
	isRepeated = c.isRepeated;
	pos = c.pos;
}

Atom& Atom::operator=(const Atom& c) {
	type = c.type;
	firstId = c.firstId;
	secondId = c.secondId;
	Token = c.Token;
	regEpx = c.regEpx;
	start = c.start;
	end = c.end;
	isRepeated = c.isRepeated;
	pos = c.pos;
	return *this;
}


string Atom::getType() {
	return type;
}

string Atom::getFirstId() {
	return firstId;
}
string Atom::getSecondId() {
	return secondId;
}
string Atom::getRegEpx() {
	return regEpx;
}
string Atom::getToken() {
	return Token;
}

void Atom::setType(string str) {
	type = str;
}
void Atom::setFirstId(string str) {
	firstId = str;
}
void Atom::setSecondId(string str) {
	secondId = str;
}
void Atom::setRegEpx(string str) {
	regEpx = str;
}

void Atom::setRepeated(bool repeat) {
	isRepeated = repeat;
}
bool Atom::getRepeated() {
	return isRepeated;
}
void Atom::setRepeatTimes(int st, int ed) {
	start = st;
	end = ed;
	isRepeated = true;
}

int Atom::getStartIndex() {
	return start;
}

int Atom::getEndIndex() {
	return end;
}

void Atom::setPosAtOfom(int pos) {
	this->pos = pos;
}
int Atom::getAtomPos() {
	return pos;
}

Atom::~Atom() {}