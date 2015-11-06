// stringTestApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

using namespace::std;



void testPrint()
{
	const int Max = 16;
	for (int i = 0; i < Max; i++)
	{
		if (i % 2 == 0)
			std::cout << i << ":" << " 2 ";

		if (i % 7 == 0)std::cout << i << ":" << " 7";
		std::cout << std::endl;
	}
}


void showAllCharInString(string text)
{
	for (int i = 0; i < text.size(); i++)
	{
		char tempchar = text[i];
		std::cout << i << " : " << tempchar << " ";

	}
	std::cout << endl;

	return;

}


void removeCharsInString(string text, string chars)
{
	const int Size = 255;
	char charMap[Size];
	memset(charMap, 0, Size);

	cout << "old string is " << text.c_str() << endl;
	cout << "to remove " << chars.c_str() << endl;

	for (int i = 0; i < chars.size(); i++)
	{
		char removeChar = chars[i];

		charMap[removeChar] = true;

	}

	//int fromIndex = 0;
	int toIndex = 0;
	for (int fromIndex = 0; fromIndex < text.size(); fromIndex++)
	{
		char testChar = text[fromIndex];
		bool remove = charMap[testChar];

		if (remove) {}
		else {
			//keep char
			if (toIndex != fromIndex)
			{   //only when orig and dest are different, we copy
				text[toIndex] = text[fromIndex];
			}
			toIndex++;
		}

	}
	// text[toIndex] = 0;
	text.resize(toIndex);

	cout << "new string is " << text.c_str() << endl;


}





int main()
{

	string string1;
	string1 = "This is string";
	showAllCharInString(string1);

	std::cout << "hello " << string1.c_str() << std::endl;
	std::cout << " pointer addr A " << &string1 << endl;

	string1 = "abc new string";

	std::cout << "hello " << string1.c_str() << std::endl;

	std::cout << " pointer addr B " << &string1 << endl;

	std::cout << " string length is  " << string1.length() << endl;
	string1[2] = 0;
	std::cout << " after set [2]=0, string length is  " << string1.size() << endl;


	std::cout << " after shrink to fit(), string length is  " << string1.size() << endl;

	removeCharsInString("abcdefghij", "bd");
	removeCharsInString("abcdefghij", "aj");
	removeCharsInString("abcdefghij", "ih");
 
	showAllCharInString(string1);

	testPrint();

	cin.get();


    return 0;
}

