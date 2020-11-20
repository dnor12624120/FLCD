#pragma once

#include <vector>
#include <map>
#include <string>
#include <fstream>

#include "HashTable.h"
#include "Grammar.h"

enum ParserState
{
	STATE_NORMAL,
	STATE_BACK,
	STATE_FINAL,
	STATE_ERROR
};

class Parser
{
	public:
		Parser(const Grammar& grammar, std::vector<std::pair<int, int>> pif, HashTable st):
			grammar{ grammar },
			programInternalForm{ pif },
			symbolTable{ st },
			currentPosition{ 0 },
			currentState{ ParserState::STATE_NORMAL }
		{

		}

		void expand()
		{

		}

		void advance()
		{

		}

		void momentaryInsuccess()
		{

		}

		void back()
		{

		}

		void anotherTry()
		{

		}

		void success()
		{

		}
	private:
		Grammar grammar;
		std::vector<std::pair<int, int>> programInternalForm;
		HashTable symbolTable;
		int currentPosition;
		ParserState currentState;
};