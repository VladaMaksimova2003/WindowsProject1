#include "BaseWindows.h"

BaseWindows::BaseWindows(
	HINSTANCE hInst,
	std::wstring className,
	std::wstring Name,
	int width,
	int height,
	int x,
	int y,
	BaseWindows* parent,
	INT id,
	DWORD style,
	DWORD styleEx)
{
	this->hWnd = CreateWindowEx(
		styleEx,
		className.c_str(),
		Name.c_str(),
		style,
		x,
		y,
		width,
		height,
		parent == nullptr ? NULL : parent->hWnd,
		(HMENU)id,
		hInst,
		NULL);

}

BaseWindows::~BaseWindows()
{

}