#pragma once
#ifndef  TOKENSTREAM_H
#define TOKENSTREAM_H

#include "Token.h"

namespace calculator {
	class TokenStream {
	public:
		TokenStream();
		Token get();
		void putback(Token t);
		void ignore(char c);
	private:
		bool full;
		Token buffer;
	};
}

#endif