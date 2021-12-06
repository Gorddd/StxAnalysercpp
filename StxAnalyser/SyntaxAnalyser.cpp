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
	fputs("������ � ������: ", fileToWrite);
	fprintf(fileToWrite, "%d", numberofRow);
	fputc('\n', fileToWrite);

	for (int i = 0; i < errDesc.length(); i++)
		fputc(errDesc[i], fileToWrite);

	fputc('\n', fileToWrite);
}

void SyntaxAnalyser::WriteErrortoConsole(string errDesc, int numberofRow) {
	cout << "������ � ������: " << numberofRow << endl;
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

	int numberofVariables = 1;
	for (int i = 0; i < params.length(); i++)
	{
		if (params[i] == ',')
			numberofVariables++;
	}
	int numberofTypes = DeleteandCountTypes(params);
	if (numberofTypes != numberofVariables) {
		errorDescription = "������ � �������� ���� � ����������";
		return true;
	}
	
	return false;
}

int ConstructAnalyser::DeleteandCountTypes(string& params)
{
	int count = 0;
	while (params.find("int") != -1) { //����� int
		params.erase(params.find("int"), 3);
		count++;
	}

	while (params.find("char") != -1) { //����� char
		params.erase(params.find("char"), 4);
		count++;
	}

	while (params.find("float") != -1) { //����� float
		params.erase(params.find("float"), 5);
		count++;
	}
	return count;
}

bool ConstructAnalyser::FindError() {
	if (!hasInitializationList())
		return false;

	while (strtoProcess.find(' ') != -1)
		strtoProcess.erase(strtoProcess.find(' '), 1); //������ �������

	for (int i = 0; i < nameofClass.length(); i++)
		if (strtoProcess[i] != nameofClass[i]) {
			errorDescription = "������������ ��� ������������";
			return true;
		}

	if (strtoProcess[nameofClass.length()] != '(') {
		errorDescription = "����� ����� ������������ ��������� \"(\"";
		return true;
	}

	vector<string> nameofParams;
	if (ParamsHandler(nameofParams))
		return true;
		
	

	return false;
}

string ConstructAnalyser::GetErrorDescription() {
	return errorDescription;
}