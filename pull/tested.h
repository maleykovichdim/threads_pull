#pragma once

#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <string.h>

#include <condition_variable>
#include <mutex>
#include <shared_mutex >
#include <thread>
#include <vector>
//#include <variant>
#include <future>
#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;

#include <assert.h>


static std::atomic<int> NUM = 0;
//////////////////////////////////////////////////////////////////////////////////
class TEST {
	public:
		TEST() { id = 0; }
		virtual ~TEST(){}

		int getID() { return id; }

		virtual int run()
		{
			int j = ++NUM;
			cout << "NUM=" << j << "  run from TEST id="<< id << endl;
			return 0;
		}


	protected:
		int id;
};
//////////////////////////////////////////////////////////////////////////////////

class TEST2: public TEST
{
public:
	TEST2() { id = 2; }
	~TEST2() {}

	int run()
	{
		for (int i = 0; i < rand() % 1000; i++)
		{
			m_vect.push_back(rand() % 1000);
		}

		int sum = 0;
		for(int i : m_vect)
		{
			sum += i;
		}
		int j = ++NUM;
		cout << "NUM=" << j << "   sum=" << sum << endl;
		return 2;
	}
private:
	vector <int> m_vect;

};

//////////////////////////////////////////////////////////////////////////////////

class TEST3 : public TEST
{
public:
	TEST3() { id = 3; }
	~TEST3() {}

	int run()
	{
		for (int i = 0; i < rand() % 1000; i++)
		{
			m_vect.push_back(rand() % 1000);
		}

		int sum = 0;

		for (int i : m_vect)
		{
			sum += i*3;
		}

		int j = ++NUM;
		cout << "NUM=" << j << "   sum(*3)=" << sum << endl;
		return 3;

	}
private:
	vector <int> m_vect;
};