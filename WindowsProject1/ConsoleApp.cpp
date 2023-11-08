#include <fstream>
#include <iostream>
#include "Config.h"

void init(
	int* WIDTH,
	int* HEIGHT,
	int* N,
	COLORREF* gridColor,
	COLORREF* backgroundColor)

{
	const std::string FileName = "config.txt";
	// ��������� ������� ���� ��� ������

	std::ifstream inputFile(FileName);
	if (inputFile.is_open()) {
		// ���� ����������, ������ �������� ������ � �����

		inputFile >> *WIDTH >> *HEIGHT >> *N >> *gridColor >> *backgroundColor;
		inputFile.close();

	}
	else {
		*WIDTH = 320;
		*HEIGHT = 240;
		*N = 3;
		*gridColor = RGB(255, 0, 0);
		*backgroundColor = RGB(0, 0, 255);
	}

}

void save(
	int* WIDTH,
	int* HEIGHT,
	int* N,
	COLORREF* gridColor,
	COLORREF* backgroundColor)
{
	const std::string FileName = "config.txt";
	std::ofstream outputFile(FileName);
	outputFile << *WIDTH << " " << *HEIGHT << " " << *N << " " << *gridColor << " " << *backgroundColor << std::endl;
	outputFile.close();
}
