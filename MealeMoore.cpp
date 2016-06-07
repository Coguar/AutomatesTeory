// MealeMoore.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Automates.h"
#include "Commands.h"

int main()
{
	std::map<std::string, CMoore> moores;
	std::map<std::string, CMeale> meales;

	try
	{
		std::ifstream ss("input.txt");

		boost::property_tree::ptree pt;
		boost::property_tree::read_json(ss, pt);

		BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("state machine"))
		{
			assert(v.first.empty()); // array elements have no names
									 //boost::property_tree::ptree &id = v.second.get_child("id");

			if (v.second.get_child("type").data() == "moore")
			{
				auto mooreAuto = CMoore();
				mooreAuto.Read(v.second);
				moores.insert(std::make_pair(v.second.get_child("id").data(), mooreAuto));
			}
			else if (v.second.get_child("type").data() == "meale")
			{
				auto mealeAuto = CMeale();
				mealeAuto.Read(v.second);
				meales.insert(std::make_pair(v.second.get_child("id").data(), mealeAuto));
			}

		}
		auto mealeMoore = FromMealeToMoore(meales.begin()->second);
		auto mooreMeale = FromMooreToMeale(moores.begin()->second);

		auto testMoore = Minimize(moores.begin()->second);

		auto DetMoore = Determinancy(moores.begin()->second);
		return EXIT_SUCCESS;
	}
	catch (std::exception const& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return EXIT_FAILURE;
}

