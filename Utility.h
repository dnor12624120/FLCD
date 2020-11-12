#pragma once

#include "Token.h"
#include <fstream>
#include <sstream>

std::string loadProgram(const std::string& filepath)
{
	std::ifstream t(filepath);
	return std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
}

bool isPrime(int n)
{
    if (n <= 1)
    {
        return false;
    }
    if (n <= 3)
    {
        return true;
    }
    if (n % 2 == 0 || n % 3 == 0)
    {
        return false;
    }

    for (int i = 5; i * i <= n; i = i + 6)
    {
        if (n % i == 0 || n % (i + 2) == 0)
        {
            return false;
        }
    }
    return true;
}

int nextPrime(int n)
{
    if (n <= 1)
    {
        return 2;
    }

    int prime = n;
    bool found = false;
    while (!found)
    {
        prime++;
        if (isPrime(prime))
            return prime;
    }
}

void specialRepresentations(int code, std::string& repr)
{
	switch (code)
	{
	case -1:
	{
		repr = "";
		break;
	}
	case 0:
	{
		repr = "";
		break;
	}
	case 1:
	{
		repr = "";
		break;
	}
	case 56:
	{
		repr = "\n";
		break;
	}
	case 57:
	{
		repr = " ";
		break;
	}
	case 58:
	{
		repr = "\t";
		break;
	}
	default:
	{
		break;
	}
	}
}
