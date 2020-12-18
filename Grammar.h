#pragma once

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

class Terminal
{
	public:
		Terminal() = default;
		Terminal(const std::string& repr) :
			representation{ repr }
		{

		}
	private:
		std::string representation;
};

class NonTerminal
{
	public:
		NonTerminal() = default;
		NonTerminal(const std::string& repr, const std::vector<std::vector<std::string>>& prods) :
			representation{ repr },
			productions{ prods }
		{

		}

		const std::vector<std::vector<std::string>>& getProductions() { return productions; }
	private:
		std::string representation;
		std::vector<std::vector<std::string>> productions;
};

class Grammar
{
	public:
		Grammar(const std::string& filepath)
		{
			loadGrammar(filepath);
		}

		std::map<std::string, Terminal> getTerminals() { return terminals; }
		std::map<std::string, NonTerminal> getNonterminals() { return nonterminals; }
		std::string getStarting() { return starting; }

		bool isTerminal(const std::string& input)
		{
			return terminals.find(input) != terminals.end() || input == "epsilon";
		}

		bool isNonterminal(const std::string& input)
		{
			return nonterminals.find(input) != nonterminals.end();
		}

		void print()
		{
			std::cout << "Terminals:\n";
			for (auto& terminal : terminals)
			{
				std::cout << terminal.first << '\n';
			}
			std::cout << "Nonterminals:\n";
			for (auto& nonterminal : nonterminals)
			{
				std::cout << "Nonterminal name: " << nonterminal.first << '\n';
				std::cout << "Productions:\n";
				for (auto& prods : nonterminal.second.getProductions())
				{
					for (auto& prod : prods)
					{
						std::cout << prod << ' ';
					}
					std::cout << '\n';
				}
				std::cout << '\n';
			}
		}
	private:
		void loadGrammar(const std::string& filepath)
		{
			std::ifstream inputFile(filepath);
			int numTerminals;
			int numNonterminals;

			inputFile >> starting >> numTerminals >> numNonterminals;

			for (int i = 0; i < numTerminals; i++)
			{
				std::string repr;
				inputFile >> repr;
				terminals[repr] = Terminal(repr);
			}

			for (int i = 0; i < numNonterminals; i++)
			{
				std::string repr;
				int numProductions;
				std::vector<std::vector<std::string>> prods;
				inputFile >> repr >> numProductions;
				for (int j = 0; j < numProductions; j++)
				{
					int prodLength;
					std::vector<std::string> prod;
					inputFile >> prodLength;
					for (int k = 0; k < prodLength; k++)
					{
						std::string prodComponent;
						inputFile >> prodComponent;
						prod.emplace_back(prodComponent);
					}
					prods.emplace_back(prod);
				}
				nonterminals[repr] = NonTerminal(repr, prods);
			}

		}

		std::string starting;
		std::map<std::string, Terminal> terminals;
		std::map<std::string, NonTerminal> nonterminals;
};