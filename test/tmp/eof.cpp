#include <iostream>
#include <ostream>
#include <string>

int main()
{
	std::string s;
	char ch = 0;
	
	std::getline(std::cin, s);
	
	std::cout << "EOF test" << std::endl;

	while (std::cin >> ch) {
		std::cout << ch;
	}

	std::cout << "\nEOF" << std::endl;
	return 0;
}
