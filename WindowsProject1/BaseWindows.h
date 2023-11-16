#pragma once
#include "framework.h"

class BaseWindows
{
protected:
	HWND hWnd;
	std::string className;
public:
	BaseWindows(
		HINSTANCE hInst,
		std::wstring className,
		std::wstring Name,
		int width,
		int height, 
		int x, 
		int y,
		BaseWindows* parent=nullptr,
		INT id=0,
		DWORD style=WS_OVERLAPPEDWINDOW,
		DWORD styleEx=NULL);
	RECT getRect();
	void setText(std::wstring text);
	void setBgColor(COLORREF color);
	void setSize(int width, int height);
	void setPos(int x, int y);
	virtual ~BaseWindows();

};

