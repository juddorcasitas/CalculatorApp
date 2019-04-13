// CalculatorApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Thanks  bewuethr (https://github.com/bewuethr/) for providing exercises from book 

#include "pch.h"
#include <iostream>
#include <vector>
#include "Token.h"
#include "TokenStream.h"
#include "Symbols.h"
using namespace calculator;

/* PROJECT Structure:
	class Token
	class TokenStream
	class Variable
	class SymbolTable

	void Token_stream::putback(Token t)
	Token Token_stream::get()

	Token_stream ts; - Provides get and putback
	double expression() - Declaration so that primary() can call expression()
	double primary() - deal with number and parenthesis
	double term() - deal with * and /
	double expression() - deal with + and -
*/

void error(std::string s) {
	throw std::runtime_error(s);
}

TokenStream::TokenStream() :full(false), buffer(0) {} //constructor for token stream

void TokenStream::putback(Token t) {
	if (full) error("putback() into full buffer");
	buffer = t;
	full = true;
}

Token TokenStream::get() {
	if (full) {
		full = false;
		return buffer;
	}

	char ch;
	std::cin.get(ch);
	
	while (isspace(ch)) {
		if (ch == '\n') return Token(print); // if newline detected, return print Token
		std::cin.get(ch);
	};

	switch (ch) {
		case print:// for "print"
		case '(': 
		case ')': 
		case '+': 
		case '-': 
		case '*': 
		case '/':
		case '%':
		case '=':
		case ',':
			return Token(ch);        // let each character represent itself
		case '.':
		case '0': 
		case '1': 
		case '2': 
		case '3': 
		case '4':
		case '5': 
		case '6': 
		case '7': 
		case '9':
		{
			std::cin.putback(ch);         // put digit back into input stream
			double val;
			std::cin >> val;              // read a floating-point number
			return Token(number, val);   // let '8' represent "a number"
		}
		default:
			if (isalpha(ch)) {
				std::string s;
				s += ch;
				while (std::cin.get(ch) && (isalpha(ch) 
						|| isdigit(ch) || ch == '_')) s += ch;
				std::cin.putback(ch);
				if (s == declkey) return Token(let);            // declaration keyword
				if (s == conkey) return Token(con);             // constant keyword
				if (s == sqrtkey) return Token(square_root);    // square root keyword
				if (s == powkey) return Token(power);           // power function keyword
				if (s == helpkey) return Token(help);           // help keyword
				if (s == quitkey) return Token(quit);           // quit keyword
				return Token(name, s);
			}
			error("Bad token");
	}
}

void TokenStream::ignore(char c) {
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;
	char ch = 0;
	while (std::cin >> ch)
		if (ch == c) return;

}

// return the value of the Variable named s
double SymbolTable::get(std::string s){
	for (int i = 0; i < var_table.size(); ++i)
		if (var_table[i].name == s) return var_table[i].value;
	error("get: undefined variable ");
}

void SymbolTable::set(std::string s, double d) {
	for (int i = 0; i < var_table.size(); ++i)
		if (var_table[i].name == s) {
			if (var_table[i].is_const) error("SymbolTable - set():constant already exists");
			var_table[i].value = d;
			return;
		}
	error("SymbolTable set(): undefined variable ");
}

bool SymbolTable::is_declared(std::string var){
	for (int i = 0; i < var_table.size(); ++i)
		if (var_table[i].name == var) return true;
	return false;
}

double SymbolTable::declare(std::string var, double val, bool b){
	if (is_declared(var)) error(" SymbolTable - declare(): Symbol was declared twice");
	var_table.push_back(Variable(var, val, b));
	return val;
}


TokenStream ts;
SymbolTable st;
double expression();

// simple power function
// handles only integers >= 0 as exponents
double my_pow(double base, int expo)
{
	if (expo == 0) {
		if (base == 0) return 0;    // special case: pow(0,0)
		return 1;                   // something to power of 0
	}
	double res = base;              // corresponds to power of 1
	for (int i = 2; i <= expo; ++i)   // powers of 2 and more
		res *= base;
	return res;
}

// deal with numbers, unary +/-, parentheses, sqrt, pow, names and assignments
// calls expression()
double primary()
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':   // handle '(' expression ')'
	{	double d = expression();
	t = ts.get();
	if (t.kind != ')') error("')' expected");
	return d;
	}
	case '-':
		return -primary();
	case '+':
		return primary();
	case number:
		return t.value;
	case name:
	{   Token t2 = ts.get();    // check next token
	if (t2.kind == '=') {   // handle name '=' expression
		double d = expression();
		st.set(t.name, d);
		return d;
	}
	else {  // not an assignment
		ts.putback(t2);
		return st.get(t.name);
	}
	}
	case square_root:   // handle 'sqrt(' expression ')'
	{   
		t = ts.get();
		if (t.kind != '(') error("'(' expected");
		double d = expression();
		if (d < 0) error("Square roots of negative numbers... nope!");
		t = ts.get();
		if (t.kind != ')') error("')' expected");
		return sqrt(d);
	}
	case power: // handle 'pow(' expression ',' integer ')'
	{   
		t = ts.get();
		if (t.kind != '(') error("'(' expected");
		double d = expression();
		t = ts.get();
		if (t.kind != ',') error("',' expected");
		t = ts.get();
		if (t.kind != number) error("second argument of 'pow' is not a number");
		int i = int(t.value);
		if (i != t.value) error("second argument of 'pow' is not an integer");
		t = ts.get();
		if (t.kind != ')') error("')' expected");
		return my_pow(d, i);
	}
	default:
		error("primary expected");
	}
}

// -----------------------------------------------------------------------

// deal with *, / and %
// calls primary()
double term()
{
	double left = primary();
	Token t = ts.get(); // get the next token from Token_stream

	while (true) {
		switch (t.kind) {
		case '*':
			left *= primary();
			t = ts.get();
			break;
		case '/':
		{	double d = primary();
		if (d == 0) error("divide by zero");
		left /= d;
		t = ts.get();
		break;
		}
		case '%':
		{   double d = primary();
		int i1 = int(left);
		if (i1 != left) error("left-hand operand of % not int");
		int i2 = int(d);
		if (i2 != d) error("right-hand operand of % not int");
		if (i2 == 0) error("%: divide by zero");
		left = i1 % i2;
		t = ts.get();
		break;
		}
		default:
			ts.putback(t);  // put t back into the Token_stream
			return left;
		}
	}
}

// -----------------------------------------------------------------------

// deal with + and -
// calls term()
double expression()
{
	double left = term();   // read and evaluate a Term
	Token t = ts.get();     // get the next Token from the Token stream

	while (true) {
		switch (t.kind) {
		case '+':
			left += term(); // evaluate Term and add
			t = ts.get();
			break;
		case '-':
			left -= term(); // evaluate Term and subtract
			t = ts.get();
			break;
		case '=':
			error("use of '=' outside of a declaration");
		default:
			ts.putback(t);  // put t back into the token stream
			return left;    // finally: no more + or -; return the answer
		}
	}
}

// -----------------------------------------------------------------------

// assume we have seen "let" or "const"
// handle: name = expression
// declare a variable called "name" with the initial value "expression"
double declaration(bool b)
{
	Token t = ts.get();
	if (t.kind != name) error("name expected in declaration");
	std::string var_name = t.name;

	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of ");

	double d = expression();
	st.declare(var_name, d, b);
	return d;
}

// -----------------------------------------------------------------------

// handles declarations and expressions
double statement()
{
	Token t = ts.get();
	switch (t.kind) {
	case let:
		return declaration(false);
	case con:
		return declaration(true);
	default:
		ts.putback(t);
		return expression();
	}
}

// -----------------------------------------------------------------------

// clean input after error
void clean_up_mess()
{
	ts.ignore(print);
}

// -----------------------------------------------------------------------

// print help instructions
void print_help()
{
	std::cout << "Instructions: enter expressions, there are\n";
	std::cout << "a few functions, you can declare variables using\n";
	std::cout << "the 'let' keyword and constants with the 'const'\n";
	std::cout << "keyword.\n";
}

// expression evaluation loop
void calculate()
{
	while (std::cin)
		try {
		std::cout << prompt.c_str();
		Token t = ts.get();
		while (t.kind == print) t = ts.get(); // first discard all "prints"
		if (t.kind == help) print_help();   // print help instructions
		else {
			if (t.kind == quit) return;
			ts.putback(t);
			std::cout << result.c_str() << statement() << std::endl;
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;   // write error message
		clean_up_mess();
	}
}

// -----------------------------------------------------------------------


int main()
{
	try{
		// predefine names :
		st.declare("pi", 3.1415926535, true);
		st.declare("e", 2.7182818284, true);
		st.declare("k", 1000, true);

		calculate();

		std::cin.get();
		return 0;
	}
	catch (std::exception& e) {
		std::cerr << "error: " << e.what() << '\n';
		std::cin.get();
		return 1;
	}
	catch (...) {
		std::cerr << "Oops: unknown exception!\n";
		std::cin.get();
		return 2;
	}
}
