#pragma once

#include "Utility.h"

#include <vector>
#include <string>
#include <memory>
#include <algorithm>

enum class TokenType
{
	INVALID,
	IDENTIFIER,
	FLOAT_CONST,
	STRING_CONST,
	CHAR_CONST,
	BOOL_CONST
};

class Token
{
	public:
		Token():
			repr{ "" },
			type{ TokenType::INVALID }
		{

		}

		Token(const std::string& repr, TokenType type) :
			repr{ repr },
			type{ type }
		{

		}

		const std::string& toString() const { return repr;  }
		TokenType getType() const { return type; }
	private:
		std::string repr;
		TokenType type;
};

/*
	A KeyValuePair (hash table element) consists of a boolean flag
	that tells us whether its used, and a token/int pair. The code represents 
	the value that will be used in the PIF.
*/

struct KeyValuePair
{
	bool used;
	Token token;
	int code;

	KeyValuePair() = default;

	KeyValuePair(bool used, Token token, int code):
		used{ used },
		token{ token },
		code{ code }
	{ }
};

// implementation of a hash table with linear probing 

class HashTable
{
	using KeyValues = std::vector<KeyValuePair>;
	public:
		HashTable():
			capacity{ DEFAULT_CAPACITY },
			size{ 0 }
		{
			elements.resize(DEFAULT_CAPACITY);
			// by default, elements get initialized to invalid tokens and are thus unused
			for (auto& elem : elements)
			{
				elem = KeyValuePair(false, Token(), -1);
			}
		}

		std::string toString() const
		{
			std::string elems;
			std::for_each(elements.begin(), elements.end(),
				[&elems](const auto& tok) { elems += "Key: " + tok.token.toString() +
					"\nValue: " + std::to_string(tok.code) + "\n\n"; });
			return "Size: " + std::to_string(size) + "\n" +
				"Capacity: " + std::to_string(capacity) + "\n" +
				"Pairs:\n" + elems;
		}

		// the average complexity of the add operation is O(1) (even when considering the amortized case when needing to resize the table)
		void add(const Token& token, int code)
		{
			add_(elements, size, capacity, token, code);
		}

		// the average complxity of the retrieve operation is also O(1)
		int get(const Token& token)
		{
			int pos = hash(token.toString(), capacity);
			int positionsSearched = 0;
			while (elements[pos % capacity].token.toString() != token.toString() && positionsSearched < capacity)
			{
				pos++;
				positionsSearched++;
			}
			if (elements[pos % capacity].token.toString() == token.toString() &&
				elements[pos % capacity].token.getType() == token.getType())
			{
				return elements[pos % capacity].code;
			}
			return -1;
		}
	private:
		void add_(KeyValues& elems, int s, int cap, const Token& token, int code)
		{
			if (s && s == cap) //if the current size is equal to the capacity, we need to resize the array
			{
				int oldSize = resize();
				add(token, code);
				size -= oldSize;
			}
			else
			{
				int pos = hash(token.toString(), cap);
				// if the positions is already in use, find the next empty one (linear probing)
				while (elems[pos % cap].used)
				{
					// if along the way we find a token identical to the one we want to insert, stop
					// we only want one entry per token
					if (elems[pos % cap].token.toString() == token.toString() &&
						elems[pos % cap].token.getType() == token.getType())
					{
						return;
					}
					pos++;
				}
				elems[pos % cap] = KeyValuePair(true, token, code);
				size++;
			}
		}
		
		int resize()
		{
			// we're finding the next prime after the double of the current capacity 
			// so we're not resizing too often
			const unsigned int newCapacity = nextPrime(capacity * 2);
			KeyValues newElements;
			// create new array of elements and reinsert the already existing ones
			int oldSize = size;
			newElements.resize(newCapacity);
			for (auto& elem : newElements)
			{
				elem = KeyValuePair(false, Token(), -1);
			}
			for (const auto& elem : elements)
			{
				if (elem.code != -1)
				{
					add_(newElements, 0, newCapacity, elem.token, elem.code);
				}
			}
			elements = newElements;
			capacity = newCapacity;
			return oldSize;
		}

		// a hashing function that does multiplications and xor operations on the characters of a string 
		unsigned int hash(std::string str, int cap)
		{
			const int A = 54059; 
			const int B = 76963; 
			const int C = 86969; 
			const int FIRSTH = 37;
			const char* s = str.c_str();
			unsigned h = FIRSTH;
			while (*s)
			{
				h = (h * A) ^ (s[0] * B);
				s++;
			}
			return h % cap;
		}
		
		// total number of elements the table can currently hold
		int capacity;
		// current number of elements currently in the table 
		int size;
		KeyValues elements;
		// the capacity a new hash table gets initialized with
		static const int DEFAULT_CAPACITY = 11;
};