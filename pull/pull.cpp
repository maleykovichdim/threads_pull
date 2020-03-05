
#include "pch.h"
#include <iostream>
#include "pull_thread.h"

int main()
{
	LOG_DURATION("TIME: ")
	PULL<TEST> pull;
	int n = 20;

	for (int i = 0; i < n; i++)
	{
		int a = rand() % 3;
		switch (a)
		{
		case 0:
			pull.push(std::make_shared<TEST>());
			break;

		case 1:
			pull.push(std::make_shared<TEST2>());
			break;

		case 2:
			pull.push(std::make_shared<TEST3>());
			break;

		default:
			break;
		}

		cout << " -------------------------   creation task event num=" << i  << endl;

	}
	std::this_thread::sleep_for(std::chrono::milliseconds(30));
	return 0;
}

