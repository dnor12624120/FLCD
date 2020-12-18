#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <set>

class FiniteAutomaton
{
	using State = std::string;
	using Input = std::string;
	using Transition = std::pair<State, Input>;

	public:
		FiniteAutomaton(const std::string& filepath)
		{
			loadFromFile(filepath);
		}

		bool isDeterministic() const
		{
			for (auto& pair : transitions)
			{
				if (pair.second.size() > 1)
				{
					return false;
				}
			}
			return true;
		}

		bool accepted(const std::string& input)
		{
			if (!isDeterministic())
			{
				return false;
			}
			State currentState = initialState;
			for (char c : input)
			{
				if (transitions.find(Transition(currentState, std::string(1, c))) != transitions.end())
				{
					currentState = transitions[Transition(currentState, std::string(1, c))][0];
				}
				else
				{
					return false;
				}
			}
			return std::find(finalStates.begin(), finalStates.end(), currentState) != finalStates.end();
		}

		std::set<State> getStates() const
		{
			std::set<State> setOfStates;
			for (const auto& state : states)
			{
				setOfStates.emplace(state);
			}
			return setOfStates;
		}

		std::set<Input> getAlphabet() const
		{
			std::set<Input> setOfInputs;
			for (const auto& input : alphabet)
			{
				setOfInputs.emplace(input);
			}
			return setOfInputs;
		}

		std::set<State> getFinalStates() const
		{
			std::set<State> setOfFinalStates;
			for (const auto& state : finalStates)
			{
				setOfFinalStates.emplace(state);
			}
			return setOfFinalStates;
		}

		std::map<Transition, std::vector<State>> getTransitions() const
		{
			return transitions;
		}
	private:
		void loadFromFile(const std::string& filepath)
		{
			std::ifstream inputFile(filepath);
			int numOfStates, numOfInputs, numOfFinalStates, numOfTransitions;
			inputFile >> numOfStates >> numOfInputs >> numOfFinalStates >> numOfTransitions;
			for (int i = 0; i < numOfStates; i++)
			{
				State state;
				inputFile >> state;
				states.emplace_back(state);
			}
			for (int i = 0; i < numOfInputs; i++)
			{
				Input input;
				inputFile >> input;
				alphabet.emplace_back(input);
			}
			inputFile >> initialState;
			for (int i = 0; i < numOfFinalStates; i++)
			{
				State finalState;
				inputFile >> finalState;
				finalStates.emplace_back(finalState);
			}
			for (int i = 0; i < numOfTransitions; i++)
			{
				int num;
				State startingState;
				Input input;
				State finalState;
				inputFile >> num;
				inputFile >> startingState >> finalState;
				for (int i = 0; i < num; i++)
				{
					inputFile >> input;
					if (transitions.find(Transition(startingState, input)) == transitions.end())
					{
						std::vector<State> outputStates;
						outputStates.emplace_back(finalState);
						transitions.insert(std::pair<Transition, std::vector<State>>(Transition(startingState, input), outputStates));
					}
					else // we have a nondeterministic FA
					{
						transitions[Transition(startingState, input)].emplace_back(finalState);
					}
				}
			}
		}

		std::vector<State> states;
		std::vector<Input> alphabet;
		State initialState;
		std::vector<State> finalStates;
		std::map<Transition, std::vector<State>> transitions;
};