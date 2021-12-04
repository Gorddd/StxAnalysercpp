#include <iostream>
#include "SyntaxAnalyser.h"

using namespace std;

SyntaxAnalyser::SyntaxAnalyser(char source[20]) {
	fileToRead = fopen(source, "r");
	fileToWrite = fopen("Results.txt", "w");
}

SyntaxAnalyser::~SyntaxAnalyser() {
	fclose(fileToRead);
	fclose(fileToWrite);
}

string SyntaxAnalyser::GetNameofClass(string str) {
	str.erase(str.find("class"), 5);
	str.erase(str.find('{'), 1);
	str.erase(str.find('\n'), 1);

	while (str.find(' ') != -1)
		str.erase(str.find(' '), 1);
	return str;
}

void SyntaxAnalyser::ReadFile() {
	string str, nameofClass;
	int numofStr = 1;
	char symbol;
	while ((symbol = fgetc(fileToRead)) != EOF) {
		str += symbol;

		if (symbol == '\n') {
			if (!nameofClass.empty() && str.find(nameofClass) != -1) {
				constructAnalyser.TakeRow(str, nameofClass);
				if (constructAnalyser.FindError()) {
					WriteErrortoFile(constructAnalyser.GetErrorDescription(), numofStr);
					WriteErrortoConsole(constructAnalyser.GetErrorDescription(), numofStr);
				}
			}

			if (str.find("class") != -1)
				nameofClass = GetNameofClass(str);

			if (str.find("};") != -1)
				nameofClass.clear();
				
			numofStr++;
			str.clear();
		}
	}
}

void SyntaxAnalyser::WriteErrortoFile(string errDesc, int numberofRow) {
	fputs("Ошибка в строке: ", fileToWrite);
	fprintf(fileToWrite, "%d", numberofRow);
	fputc('\n', fileToWrite);

	for (int i = 0; i < errDesc.length(); i++)
		fputc(errDesc[i], fileToWrite);

	fputc('\n', fileToWrite);
}

void SyntaxAnalyser::WriteErrortoConsole(string errDesc, int numberofRow) {
	cout << "Ошибка в строке: " << numberofRow << endl;
	cout << errDesc << endl;
}








void ConstructAnalyser::TakeRow(string str, string nameofClass) {
	errorDescription.clear();
	strtoProcess = str;
	this->nameofClass = nameofClass;
}

bool ConstructAnalyser::hasInitializationList() {
	return strtoProcess.find(":") != -1;
}

bool ConstructAnalyser::FindError() {
	if (!hasInitializationList())
		return false;

	while (strtoProcess.find(' ') != -1)
		strtoProcess.erase(strtoProcess.find(' '), 1); //Убрать пробелы

	for (int i = 0; i < nameofClass.length(); i++)
		if (strtoProcess[i] != nameofClass[i]) {
			errorDescription = "Неправильное имя конструктора";
			return true;
		}

	if (strtoProcess[nameofClass.length()] != '(') {
		errorDescription = "После имени конструктора ожидалась \"(\"";
		return true;
	}



	return false;
}

string ConstructAnalyser::GetErrorDescription() {
	return errorDescription;
}