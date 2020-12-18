#include <iostream>
#include <utility>
#include <map>
#include <fstream>
#include <regex>

#include "HashTable.h"
#include "Token.h"
#include "Utility.h"
#include "Scanner.h"
#include "FiniteAutomaton.h"
#include "Grammar.h"
#include "Parser.h"
#include "Output.h"

void testProgram(const std::string& filepath);
void testAutomaton(const std::string& filepath);
void testGrammar(const std::string& grammarFilepath);
void printMenu();

int main()
{
	testGrammar("g1.in");
	return 0;
}

void testGrammar(const std::string& grammarFilepath)
{
	/*
	S -> a A | b B
	A -> a A | c B
	B -> c
	*/
	Grammar grammar(grammarFilepath);
	std::vector<std::tuple<std::string, int, int>> w1{
		std::tuple<std::string, int, int>("a", 0, 0),
		std::tuple<std::string, int, int>("a", 0, 1),
		std::tuple<std::string, int, int>("a", 0, 2),
		std::tuple<std::string, int, int>("c", 0, 3),
		std::tuple<std::string, int, int>("c", 0, 4)
	};
	std::vector<std::tuple<std::string, int, int>> w2{
		std::tuple<std::string, int, int>("a", 0, 0),
		std::tuple<std::string, int, int>("x", 0, 1)
	};
	Parser parser(grammar, w1);
	try
	{
		auto output = parser.parse();
		for (int i = output.size() - 1; i >= 0; i--)
		{
			std::cout << output[i] << '\n';
		}
		Output o(grammar, output);
		o.buildTree(0, o.getRoot());
		o.print(o.getRoot());
	}
	catch (std::exception& e)
	{
		std::cout << "Error parsing\n";
	}
}

void testProgram(const std::string& filepath)
{
	std::ifstream inputFile("token.in");
	std::map<std::string, int> tokens;
	for (int i = 0; i < 61; i++)
	{
		std::string repr;
		int code;
		inputFile >> repr >> code;
		specialRepresentations(code, repr);
		tokens.insert(std::pair<std::string, int>(repr, code));
	}
	std::string p1 = loadProgram(filepath);
	Scanner sc(tokens);
	try
	{
		sc.scan(p1);
		std::cout << sc.pifToString() << '\n';
		std::cout << sc.stToString() << '\n';
	}
	catch (std::runtime_error& e)
	{
		std::cout << e.what() << '\n';
	}
}

void testAutomaton(const std::string& filepath)
{
	FiniteAutomaton automaton(filepath);
	bool shouldExit = false;
	printMenu();
	while (!shouldExit)
	{
		int option;
		std::cout << ">";
		std::cin >> option;
		if (option == 0)
		{
			shouldExit = true;
		}
		else if (option == 1)
		{
			auto states = automaton.getStates();
			std::cout << "States:\n";
			for (auto& state : states)
			{
				std::cout << state << '\n';
			}
		}
		else if (option == 2)
		{
			auto inputs = automaton.getAlphabet();
			std::cout << "Alphabet:\n";
			for (auto& input : inputs)
			{
				std::cout << input << '\n';
			}
		}
		else if (option == 3)
		{
			auto states = automaton.getFinalStates();
			std::cout << "Final states:\n";
			for (auto& state : states)
			{
				std::cout << state << '\n';
			}
		}
		else if (option == 4)
		{
			auto transitions = automaton.getTransitions();
			std::cout << "Final states:\n";
			for (auto& state : transitions)
			{
				std::cout << state.first.first << " " << state.first.second << " -> ";
				for (auto& s : state.second)
				{
					std::cout << s << " ";
				}
				std::cout << '\n';
			}
		}
		else if (option == 5)
		{
			std::cout << "Checking if FA is deterministic\n";
			if (automaton.isDeterministic())
			{
				std::cout << "FA is deterministic\n";
			}
			else
			{
				std::cout << "FA is nondeterministic\n";
			}
		}
		else
		{
			std::string input;
			std::cout << "Enter input\n>";
			std::cin >> input;
			std::cout << "Checking if " + input + " is accepted by FA\n";
			if (automaton.accepted(input))
			{
				std::cout << input + " is accepted\n";
			}
			else
			{
				std::cout << input + " is not accepted\n";
			}
		}
	}
}

void printMenu()
{
	std::cout << "0. Exit\n";
	std::cout << "1. Print all states\n";
	std::cout << "2. Print alphabet\n";
	std::cout << "3. Print final states\n";
	std::cout << "4. Print transitions\n";
	std::cout << "5. Check determinism\n";
	std::cout << "6. Check if input is accepted\n";
}