// smartPtr_sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"



#include <iostream>

#include <memory>  // for unique_ptr


using namespace::std;

class BigObject
{
	static const int MB1 = 1024 * 1024;
	char buffer[100 * MB1];

public: void showMe() {
	cout << "the buff size is " << sizeof(buffer) << endl;
};

		void deleter()
		{
			cout << "deleter is here! running ! deleting ! " << endl;
		}
};




 

// ====================================
class MyDeleterClass {
public:
	/// this is the standard default deleter function , which is operator ()
	void operator ()(BigObject *p) {

		std::cout << "deleter class working" << endl;
		std::cout << "p->showMe(): " << endl;
		p->showMe();
		cout << "press enter to release memory" << endl;
		cin.get();
		// You must exe delete p, otherwise there would be no one to delete p
		delete p;

	}


};

void standaloneDeleterFunction(BigObject *p) {

	std::cout << "standaloneDeleterFunction working" << endl;
	std::cout << "p->showMe(): " << endl;
	p->showMe();
	cout << "press enter to release memory" << endl;
	cin.get();
	// You must exe delete p, otherwise there would be no one to delete p
	delete p;
}


//normally it should just return void !!!
string standaloneDeleterFunctionRetString(BigObject *p) {

	std::cout << "standaloneDeleterFunctionRetString working" << endl;
	std::cout << "p->showMe(): " << endl;
	p->showMe();
	cout << "press enter to release memory" << endl;
	cin.get();
	// You must exe delete p, otherwise there would be no one to delete p
	delete p;
	return "deleted!";
}


void testDeleter()
{
	//use class and method op () as deleter
	std::unique_ptr<BigObject, MyDeleterClass> unPtr(new BigObject());

	std::unique_ptr<BigObject, void(*)(BigObject*)> unPtr2(new BigObject(), &standaloneDeleterFunction);

	std::unique_ptr<BigObject, string(*)(BigObject*)> unPtr3(new BigObject(), &standaloneDeleterFunctionRetString);

	//std::unique_ptr<void, void(*)(void*)> p1(::CreateEvent(NULL, false, false, NULL), [](void *p) {
	//  ::CloseHandle(p);
	//      std::cout << "deleter in lambda expression\n";
	//    });

	//return 0;
}




int main()
{
	testDeleter();
	cin.get();
    return 0;
}

