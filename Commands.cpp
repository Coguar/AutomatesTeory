#include "stdafx.h"
#include "Commands.h"

CMeale FromMooreToMeale(CMoore & mooreAuto)
{
	CMeale mealeAuto;
	for (auto &it : mooreAuto.m_states)
	{
		mealeAuto.m_states.insert(std::pair<std::string, CMeale::State>(it.first, CMeale::State()));
		
		auto mealeState = mealeAuto.m_states.find(it.first);
		for (auto &stateIt : it.second.m_transitions)
		{
			mealeState->second.m_transitions.insert(
				std::pair<std::string, std::pair<std::string, std::string>>(stateIt.first,
					std::pair<std::string, std::string>(stateIt.second, 
						mooreAuto.m_states.find(stateIt.second)->second.m_outSymb)));
		}
	}
	return mealeAuto;
}

std::string FindKeyByValue(std::pair<std::string, std::string> const& key, std::map<std::string, std::pair<std::string, std::string>> dict)
{
	for (auto &it : dict)
	{
		if (it.second == key)
		{
			return it.first;
		}
	}
	return "err";
}

CMoore FromMealeToMoore(CMeale & mealeAuto)
{
	std::set<std::pair<std::string, std::string>> uniqueStates;

	for (auto &it : mealeAuto.m_states)
	{
		for (auto &state : it.second.m_transitions)
		{
			uniqueStates.insert(state.second);
		}
	}

	std::map<std::string, std::pair<std::string, std::string>> newStates;
	unsigned number = 0;
	for (auto &state : uniqueStates)
	{
		newStates.insert(std::pair<std::string, std::pair<std::string, std::string>>
			("q" + std::to_string(number), state));
		++number;
	}

	CMoore mooreAuto;

	for (auto &it : newStates)
	{
		mooreAuto.m_states.insert(std::pair<std::string, CMoore::State>(it.first, CMoore::State(it.second.second)));
	}

	for (auto &it : mooreAuto.m_states)
	{
		for (auto &state : mealeAuto.m_states.find(newStates.find(it.first)->second.first)->
			second.m_transitions)
		{
			it.second.m_transitions.insert(std::pair<std::string, std::string>(
				state.first, FindKeyByValue(state.second, newStates)));
		}

	}
	return mooreAuto;
}

struct Vertex
{
	Vertex(std::string const& n, std::multimap<std::string, std::string> const& trans)
		: name(n)
	{
		transitions = trans;
	}
	std::string name;
	std::multimap<std::string, std::string> transitions;

	bool operator ==(Vertex const& ver)const
	{
		for (auto &it : transitions)
		{
			if (ver.transitions.find(it.first) == ver.transitions.end())
			{
				return false;
			}
			if (ver.transitions.find(it.first)->second != it.second)
			{
				return false;
			}
		}
		return true;
	}
};

struct Group
{
	std::string name;
	std::string output;
	std::set<std::string> includedVertex;
	std::vector<Vertex> vertexs;
};

void Refresh(std::vector<Group> & groups)
{
	for (auto &it : groups)
	{
		for (auto &vert : it.vertexs)
		{
			for (auto &trans : vert.transitions)
			{
				for (auto &it2 : groups)
				{
					if (it2.includedVertex.find(trans.second) != it2.includedVertex.end())
					{
						trans.second = it2.name;
					}
				}
			}
		}
	}
};

void Division(std::vector<Group> & groups, int & groupNum)
{
	for (auto &it : groups)
	{
		for (unsigned i = 1; i < it.vertexs.size(); ++i)
		{
			if (!(it.vertexs[i] == it.vertexs[0]))
			{
				bool isInserted = false;
				for (auto &it1 : groups)
				{
					if (it.vertexs[i] == it1.vertexs[0])
					{
						isInserted = true;
						it1.includedVertex.insert(it.vertexs[i].name);
						it1.vertexs.push_back(Vertex(it.vertexs[i].name, it.vertexs[i].transitions));
						it.vertexs.erase(it.vertexs.begin() + i);
						it.includedVertex.erase(it.vertexs[i].name);
					}
				}
				if (!isInserted)
				{
					groups.push_back(Group());
					groups.back().name = std::to_string(groupNum);
					groups.back().output = it.output;
					++groupNum;
					groups.back().includedVertex.insert(it.vertexs[i].name);
					groups.back().vertexs.push_back(Vertex(it.vertexs[i].name, it.vertexs[i].transitions));
					it.vertexs.erase(it.vertexs.begin() + i);
					it.includedVertex.erase(it.vertexs[i].name);
				}
			}
		}
	}
}

CMoore Minimize(CMoore & mainMoore)
{
	auto moore = mainMoore;
	int groupNum = 0;
	std::vector<Group> groups;
	std::map<std::string, std::string> groopsNames;
	//первоначальное распределение
	for (auto &it : moore.m_states)
	{
		bool isInserted = false;
		for (auto &group : groups)
		{
			if (it.second.m_outSymb == group.output)
			{
				group.includedVertex.insert(it.first);
				group.vertexs.push_back(Vertex(it.first, it.second.m_transitions));
				isInserted = true;
				break;
			}
		}
		if (!isInserted)
		{
			groups.push_back(Group());
			groups.back().name = std::to_string(groupNum);
			groups.back().output = it.second.m_outSymb;
			++groupNum;
			groups.back().includedVertex.insert(it.first);
			groups.back().vertexs.push_back(Vertex(it.first, it.second.m_transitions));
		}
	}

	Refresh(groups);
	
	auto oldGroups = groups;
	Division(groups, groupNum);
	Refresh(groups);

	while (oldGroups.size() != groups.size())
	{
		oldGroups = groups;
		Division(groups, groupNum);
		Refresh(groups);
	}

	CMoore newMoore;
	for (auto &it : groups)
	{
		auto state = CMoore::State(it.output);
		for (auto &it1 : it.vertexs[0].transitions)
		{
			state.m_transitions.insert({ it1.first, it1.second });
		}
		newMoore.m_states.insert({ it.name, state });
	}
	return newMoore;
}

CMoore Determinancy(CMoore & mainMoore)
{
	std::map<int, std::set<std::string>> already_using_vertex;
	int vertex_num = 1;
	auto moore = mainMoore;
	CMoore newMoore;
	newMoore.m_states.insert({ "0", moore.m_states.begin()->second });
	already_using_vertex.insert({ 0, { moore.m_states.begin()->first} });
	for (auto &it : newMoore.m_states)
	{
		//for (auto &trans : it.second.m_transitions)
		auto trans = it.second.m_transitions.begin();
		while (trans != it.second.m_transitions.end())
		{
			if (it.second.m_transitions.count(trans->first) > 1)
			{
				std::set<std::string> using_vertex;
				auto range = it.second.m_transitions.equal_range(trans->first);
				CMoore::State state(it.second.m_outSymb);
				for (auto iter = range.first; iter != range.second; ++iter)
				{
					auto vertex = moore.m_states.find(iter->second);
					for (auto &t : vertex->second.m_transitions)
					{
						state.m_transitions.insert(t);
						using_vertex.insert(t.second);

					}

				}

				bool thisVertexWasAlready = false;
				int vertNumIfItWas = 0;
				for (auto &it_using_vertex : already_using_vertex)
				{
					if (it_using_vertex.second == using_vertex)
					{
						thisVertexWasAlready = true;
						break;
					}
					++vertNumIfItWas;
				}

				auto input = it.second.m_transitions.find(trans->first)->first;
				if (!thisVertexWasAlready)
				{
					already_using_vertex.insert({ vertex_num, using_vertex });
					auto transfer_iterator = state.m_transitions.begin();
					while (transfer_iterator != --state.m_transitions.end())
					{
						auto inc_transfer_iterator = transfer_iterator;
						++inc_transfer_iterator;
						if (transfer_iterator->first == inc_transfer_iterator->first
							&& transfer_iterator->second == inc_transfer_iterator->second)
						{
							state.m_transitions.erase(transfer_iterator);
							transfer_iterator = state.m_transitions.begin();
						}
						else
						{
							++transfer_iterator;
						}
					}
					newMoore.m_states.insert({ std::to_string(vertex_num), state });
					moore.m_states.insert({ std::to_string(vertex_num), state });
					
					auto oldTrans = it.second.m_transitions.find(input);
					while (oldTrans != it.second.m_transitions.end())
					{
						it.second.m_transitions.erase(oldTrans);
						oldTrans = it.second.m_transitions.find(input);
						trans = it.second.m_transitions.begin();
					}
					it.second.m_transitions.insert({ input, std::to_string(vertex_num) });
					++vertex_num;
				}
				else
				{
					auto oldTrans = it.second.m_transitions.find(input);
					while (oldTrans != it.second.m_transitions.end())
					{
						it.second.m_transitions.erase(oldTrans);
						oldTrans = it.second.m_transitions.find(input);
						trans = it.second.m_transitions.begin();
					}
					if (it.second.m_transitions.find(input) == it.second.m_transitions.end()
						|| it.second.m_transitions.find(input)->second != std::to_string(already_using_vertex.find(vertNumIfItWas)->first))
					{
						it.second.m_transitions.insert({ input, std::to_string(already_using_vertex.find(vertNumIfItWas)->first) });
					}
				}
			}
			else
			{
				newMoore.m_states.insert({ moore.m_states.find(trans->second)->first, moore.m_states.find(trans->second)->second });
				moore.m_states.insert({ moore.m_states.find(trans->second)->first, moore.m_states.find(trans->second)->second });
				++trans;
			}
		}
	}
	return newMoore;
}