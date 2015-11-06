// hashmap_app.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <unordered_map>
#include <assert.h>     /* assert */
using namespace::std;

//if item not found then return false
string removeAKeyValuePair(unordered_map<string, string> *mymap, string removeValue)
{
	std::cout << " ==========  removing =========!!" << endl;
	bool result = false;
	//for (auto &item : *mymap)
	string key;
	for (auto item : *mymap)
	{
		//if (item == mymap->end()) break;


		string testValue = item.second;
		if (testValue == removeValue)
		{

			// found !!
			key = item.first;

			std::cout << item.first.c_str() << " " << item.second.c_str() << " removed!" << endl;

			result = true;
			break;

		}
	}

	if (key.size() > 0) mymap->erase(key);

	return key;
}


void removeAKeyValuePairTest()
{
	string removeValue = "remValue";
	unordered_map<string, string> *mymap = new unordered_map<string, string>;

	mymap->insert({ "aaa", "aaavalue" });
	mymap->insert({ "bbb", "bbbvalue" });
	mymap->insert({ "ccc", "cccvalue" });
	mymap->insert({ "toRemove1", "remValue" });
	mymap->insert({ "toRemove2", "remValue" });
	mymap->insert({ "ddd", "dddvalue" });

	std::cout << "before remove, map is: " << endl;
	for (auto item : *mymap)
	{
		std::cout << item.first.c_str() << " " << item.second.c_str() << "  " << endl;

	}

	string resultKey = removeAKeyValuePair(mymap, removeValue);

	std::cout << " found:  " << resultKey.c_str() << endl;
	for (auto item : *mymap)
	{
		std::cout << item.first.c_str() << " " << item.second.c_str() << "  " << endl;

	}
}



bool findMapItemByValue(unordered_map<string, string> *mymap, string value, string &key)
{

	bool found = false;
	for (auto item : *mymap)
	{
		string testvalue;
		testvalue = item.second;
		if (value == testvalue) {
			key = item.first;
			found = true;
			break;
		}
	}
	return found;
}


void testFindMapItemByValue()
{
	string removeValue = "remValue";
	unordered_map<string, string> *mymap = new unordered_map<string, string>;

	mymap->insert({ "aaa", "aaavalue" });
	mymap->insert({ "bbb", "bbbvalue" });
	mymap->insert({ "ccc", "cccvalue" });
	mymap->insert({ "keytofind1", "findValue" });
	mymap->insert({ "keytoRemove2", "remValue" });
	mymap->insert({ "ddd", "dddvalue" });
	//mymap->insert({ "", "notExistingdddvalue" });

	string key;
	bool found = findMapItemByValue(mymap, "findValue", key);
	if (found) {
		cout << "findValue is found !! key is " << key.c_str() << endl;
	}
	else
	{
		cout << "findValue is not found, wrong!!" << endl;
	}
	assert(found);

	found = findMapItemByValue(mymap, "notExistingvalue", key);

	if (found) {
		cout << "found !! key is " << key.c_str() << endl;
	}
	else
		cout << "notExistingdddvalue not found as expected" << endl;
	assert(!found);

}





void testingMap()
{
	std::unordered_map<char, bool> *m = new std::unordered_map<char, bool>;

	m->insert({ 'a', true });
	m->insert({ 'b', false });
	auto aaValue = m->find('a');
	std::cout << "m->find('a') = " << aaValue->second << endl;

	std::cout << (aaValue != m->end()) << "" << endl;
	/*
	for (auto    p : *m)
	std::cout << " " << p.first << " => " << p.second << '\n';*/

}

int main()
{
	testingMap();
	testFindMapItemByValue();

	removeAKeyValuePairTest();

	
	cin.get();
    return 0;
}

