#include <iostream>
#include "SyntaxAnalyser.h"

int main() {
	setlocale(LC_ALL, "Russian");

	char source[20];
	std::cout << "Введите расположение файла: ";
	std::cin >> source;
	
	SyntaxAnalyser syntaxAnalyser(source);
	syntaxAnalyser.ReadFile();
	
}