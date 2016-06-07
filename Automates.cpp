#include "stdafx.h"
#include "Automates.h"

void CMeale::Read(boost::property_tree::ptree & pt)
{
	BOOST_FOREACH(boost::property_tree::ptree::value_type &state, pt.get_child("states"))
	{
		std::cout << state.second.get_child("id").data() << "++" << std::endl;
		m_states.insert(std::pair<std::string, State>(state.second.get_child("id").data(), State()));
	}
	
	BOOST_FOREACH(boost::property_tree::ptree::value_type &transition, pt.get_child("transitions"))
	{
		m_states.find(transition.second.get_child("from").data())->
			second.m_transitions.insert(std::pair<std::string, std::pair<std::string, std::string>>
				(transition.second.get_child("input").data(), std::pair<std::string,
					std::string>(transition.second.get_child("to").data(), transition.second.get_child("output").data())));
	}

}

void CMoore::Read(boost::property_tree::ptree & pt)
{
	BOOST_FOREACH(boost::property_tree::ptree::value_type &state, pt.get_child("states"))
	{
		m_states.insert(std::pair<std::string, State>(state.second.get_child("id").data(),
			State(state.second.get_child("output").data())));
	}

	BOOST_FOREACH(boost::property_tree::ptree::value_type &transition, pt.get_child("transitions"))
	{
		m_states.find(transition.second.get_child("from").data())->
			second.m_transitions.insert(std::pair<std::string, std::string>
				(transition.second.get_child("input").data(), (transition.second.get_child("to").data())));
	}

}