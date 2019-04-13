#pragma once
#ifndef SYMBOLS_H
#define SYMBOLS_H

namespace calculator {

	const char let = 'L';           // declaration token
	const char con = 'C';           // constant token
	const char quit = 'q';          // t.kind==quit means that t is q quit Token
	const char help = 'h';          // help token
	const char print = ';';         // t.kind==print means that t is a print Token
	const char number = '8';        // t.kind==number means that t is a number Token
	const char name = 'a';          // name token
	const char square_root = 's';   // square root token
	const char power = 'p';         // power function token
	const std::string declkey = "let";   // declaration keyword
	const std::string conkey = "const";  // constant keyword
	const std::string sqrtkey = "sqrt";  // keyword for square root
	const std::string powkey = "pow";    // keyword for power function
	const std::string quitkey = "quit";  // keyword to quit
	const std::string helpkey = "help";  // keyword for help
	const std::string prompt = "> ";
	const std::string result = "= "; // used to indicate that what follows is a result

	class Variable {
	public:
		std::string name;
		double value;
		bool is_const;
		Variable(std::string n, double v, bool b) :name(n), value(v), is_const(b) { }
	};
	class SymbolTable {
	public:
		double get(std::string s);                           // return the value of the Variable named s
		void set(std::string s, double d);                   // set the Variable named s to d
		bool is_declared(std::string var);                   // is var already in var_table?
		double declare(std::string var, double val, bool b);   // add (var,val) to var_table
	private:
		std::vector<Variable> var_table; // vector of Variables
	};
}

#endif