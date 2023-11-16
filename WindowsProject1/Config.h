#pragma once

#include <windows.h>
#include <fstream>
#include <iostream>
#include <sstream>

class Config {
public:
	Config(std::string FileName = "config.txt", int width = 320, int height = 240, int n = 3,
		COLORREF gridColor = RGB(255, 0, 0), COLORREF backgroundColor = RGB(0, 0, 255))
		:FileName(FileName), WIDTH(width), HEIGHT(height), N(n), gridColor(gridColor), backgroundColor(backgroundColor) {
	}


	//Потоки ввода - вывода:
	void init();
	void save();

	//Чтение и запись файлов в библиотеке Win32 API:
	void initWinAPI();
	void saveWinAPI();

	//Файловые переменные 
	void initFileVariable();
	void saveFileVariable();

	//отображения файлов на память
	void initMap();
	void saveMap();

	void setDefaultValues();

public:
	std::string FileName;
	int WIDTH;
	int HEIGHT;
	int N;
	COLORREF gridColor;
	COLORREF backgroundColor;
};

