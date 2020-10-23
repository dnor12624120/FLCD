#include <iostream>
#include <utility>

#include "HashTable.h"

int main()
{
	HashTable ht;
	int pos;
	ht.add(Token("a", TokenType::IDENTIFIER));
	ht.add(Token("b", TokenType::IDENTIFIER));
	ht.add(Token("c", TokenType::IDENTIFIER));
	ht.add(Token("d", TokenType::IDENTIFIER));
	ht.add(Token("e", TokenType::IDENTIFIER));
	ht.add(Token("f", TokenType::IDENTIFIER));
	ht.add(Token("g", TokenType::IDENTIFIER));
	ht.add(Token("h", TokenType::IDENTIFIER));
	ht.add(Token("i", TokenType::IDENTIFIER));
	ht.add(Token("j", TokenType::IDENTIFIER));
	ht.add(Token("k", TokenType::IDENTIFIER));
	ht.add(Token("afzxcvxcvzxg", TokenType::IDENTIFIER));
	ht.add(Token("a", TokenType::IDENTIFIER));
	ht.add(Token("e", TokenType::IDENTIFIER));
	std::cout << ht.get(Token("j", TokenType::IDENTIFIER)) << '\n';
	std::cout << ht.toString() << '\n';
	return 0;
}