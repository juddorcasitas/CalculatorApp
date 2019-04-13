#pragma once
#ifndef  TOKEN_H
#define TOKEN_H

namespace calculator {
	class Token {
	public:
		char kind;
		double value;
		std::string name;
		Token(char ch, double v = 0) :kind(ch), value(v) {} //basic init functions
		Token(char ch, std::string n) :kind(ch), name(n) {}
	};
}

#endif