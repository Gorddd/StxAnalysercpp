#include <iostream>
#include <vector>
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

bool ConstructAnalyser::ParamsHandler(vector<string>& nameofParams)
{
	string params = strtoProcess;

	params.erase(0, params.find("(") + 1);
	params.erase(params.find(")"));

	int numberofTypes = DeleteandCountTypes(params);
	int numberofVariables = 0;
	if (!params.empty())
		numberofVariables++;
	for (int i = 0; i < params.length(); i++)
	{
		if (params[i] == ',')
			numberofVariables++;
	}
	if (numberofTypes != numberofVariables) {
		errorDescription = "Ошибка в описании типа в параметрах";
		return true;
	}
	
	string varName;
	for (int i = 0; i < params.length() + 1; i++)
	{
		if (params[i] == ',' || i == params.length()) {
			nameofParams.push_back(varName);
			varName.clear();
		}
		else
			varName += params[i];
	}
	
	return false;
}

int ConstructAnalyser::DeleteandCountTypes(string& params)
{
	int count = 0;
	while (params.find("int") != -1) { //Сотрёт int
		params.erase(params.find("int"), 3);
		count++;
	}

	while (params.find("char") != -1) { //Сотрёт char
		params.erase(params.find("char"), 4);
		count++;
	}

	while (params.find("float") != -1) { //Сотрёт float
		params.erase(params.find("float"), 5);
		count++;
	}
	return count;
}

bool ConstructAnalyser::InitializationListHandler(vector<string> nameofParams)
{
	string initList = strtoProcess;

	initList.erase(0, initList.find(":") + 1);

	if (initList.find("(") == -1 && initList.find(")") == -1) {
		errorDescription = "В списке инициализации отсутствуют \"()\"";
		return true;
	}

	if (initList.find("(") == -1) {
		errorDescription = "В списке инициализации отсутствует \"(\"";
		return true;
	}

	if (initList.find(")") == -1) {
		errorDescription = "В списке инициализации отсутствует \")\"";
		return true;
	}

	int numberofCommas = GetNumberofCommas(initList);
	int numberofBlocks;
	if (GetNumberofBlocks(initList, numberofBlocks)) {
		errorDescription = "Проблема в количестве ( или )";
		return true;
	}

	if (numberofCommas + 1 != numberofBlocks) {
		errorDescription = "Проблема в несоответствии \",\" и \"()\"";
		return true;
	}

	string nameofVar;
	while (!initList.empty()) {
		if (initList.find("(") != -1)
			initList.erase(0, initList.find("(") + 1);
		else
			break;
		
		for (int i = 0; initList[i] != ')'; i++)
		{
			nameofVar += initList[i];
		}

		if (nameofVar.empty())
			continue;

		for (int i = 0; i < nameofParams.size(); i++)
		{
			if (nameofVar == nameofParams[i])
				break;
			else
				if (i == nameofParams.size() - 1) {
					errorDescription = "Неизвестное имя переменной в списке инициализации в ()";
					return true;
				}
		}
		nameofVar.clear();
	}

	return false;
}

int ConstructAnalyser::GetNumberofCommas(string str)
{
	int count = 0;
	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == ',')
			count++;
	}
	return count;
}

bool ConstructAnalyser::GetNumberofBlocks(string str, int& num)
{
	num = 0;
	bool flag = false;

	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == '(') {
			if (flag)
				return true;
			flag = true;
		}
		if (str[i] == ')') {
			if (!flag)
				return true;
			flag = false;
			num++;
		}
	}

	return flag;
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

	vector<string> nameofParams;
	if (ParamsHandler(nameofParams))
		return true;
		
	if (InitializationListHandler(nameofParams))
		return true;

	return false;
}

string ConstructAnalyser::GetErrorDescription() {
	return errorDescription;
}