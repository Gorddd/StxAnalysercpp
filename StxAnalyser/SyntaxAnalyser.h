#pragma once
#include <stdio.h>
#include <vector>

using namespace std;

class ConstructAnalyser
{
private:
	string strtoProcess;
	string nameofClass;

	string errorDescription;

	bool hasInitializationList();
	bool ParamsHandler(vector<string>& nameofParams);
	int DeleteandCountTypes(string& params);
	bool InitializationListHandler(vector<string> nameofParams);
	int GetNumberofCommas(string str);
	bool GetNumberofBlocks(string str, int& num);
public:
	bool FindError();
	void TakeRow(string str, string nameofClass);
	string GetErrorDescription();
};




class SyntaxAnalyser {
private:
	FILE* fileToRead;
	FILE* fileToWrite;

	ConstructAnalyser constructAnalyser;
	
	string GetNameofClass(string str);
	void WriteErrortoFile(string errDesc, int numberofRow);
	void WriteErrortoConsole(string errDesc, int numberofRow);

public:
	

	SyntaxAnalyser(char source[20]);
	~SyntaxAnalyser();

	void ReadFile();
};
