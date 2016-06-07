#pragma once
#include <map>
#include <boost/property_tree/ptree.hpp>

struct IBaseAutomate
{
	virtual ~IBaseAutomate() = default;

	virtual void Read(boost::property_tree::ptree & pt) = 0;
};


class CMeale : public IBaseAutomate
{
public:

	struct State
	{
		std::multimap <std::string, std::pair<std::string, std::string>> m_transitions;
	};
	virtual void Read(boost::property_tree::ptree & pt) override;

	std::map <std::string, State> m_states;
};


class CMoore : public IBaseAutomate
{
public:

	struct State
	{
		State(std::string const& output) :
			m_outSymb(output)
		{};
		std::multimap <std::string, std::string> m_transitions;
		std::string m_outSymb;
	};
	virtual void Read(boost::property_tree::ptree & pt) override;

	std::map <std::string, State> m_states;
};