#pragma once

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <string>
#include <stack>

#include "HashTable.h"
#include "Grammar.h"

enum class ParserState
{
	STATE_NORMAL,
	STATE_BACK,
	STATE_FINAL,
	STATE_ERROR
};

class Parser
{
	public:
		Parser(const Grammar& grammar, std::vector<std::pair<int, std::tuple<int, int, int>>> pif, HashTable st, std::map<int, std::string> tokenCodes):
			grammar{ grammar },
			programInternalForm{ pif },
			symbolTable{ st },
			currentPosition{ 0 },
			currentState{ ParserState::STATE_NORMAL },
			tokenCodes{ tokenCodes }
		{

		}

		std::stack<std::string> getOutput() { return output; }

		bool parse(const std::string& currentToken)
		{
			if (grammar.isNonterminal(currentToken))
			{
				auto productions = grammar.getNonterminals()[currentToken].getProductions();
				for (const auto& production : productions)
				{
					int correctParses = 0;
					for (const auto& str : production)
					{
						if (parse(str))
						{
							correctParses++;
						}
						else
						{
							currentPosition -= correctParses;
							parse(lastCorrect);
							break;
						}
					}
					if (correctParses == production.size())
					{
						output.push(currentToken);
						return true;
					}
				}
			}
			else
			{
				if (currentToken == "identifier" && programInternalForm[currentPosition].first == 0 ||
					((currentToken == "character-constant" || currentToken == "boolean-constant" || 
					 currentToken == "string-constant" || currentToken == "floating-constant" || currentToken == "integer-constant")
					&& programInternalForm[currentPosition].first == 1) ||
					tokenCodes[std::get<0>(programInternalForm[currentPosition].second)] == currentToken)
				{
					currentPosition++;
					lastCorrect = currentToken;
					output.push(currentToken);
					return true;
				}
				return false;
			}
		}

	private:
		Grammar grammar;
		std::vector<std::pair<int, std::tuple<int, int, int>>> programInternalForm;
		HashTable symbolTable;
		int currentPosition;
		ParserState currentState;
		std::string lastCorrect;
		std::stack<std::string> output;
		std::map<int, std::string> tokenCodes;
};