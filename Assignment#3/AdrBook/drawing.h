#pragma once

#ifdef WIN32
#pragma warning (disable: 4996)
#include <Windows.h>
#endif

#include <utility>
#include "object.h"
#include "table.h"
#include "listElements.h"

using namespace std;
pair<HPEN, HBRUSH> draw_init(HDC& dc, HPEN& hPen, HBRUSH& hBrush, int back, int border, int back_color, int border_color)
{
	if (!border)
		hPen = (HPEN)GetStockObject(NULL_PEN);
	else
		hPen = CreatePen(0, border, border_color);

	if (!back)
		hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	else
		hBrush = CreateSolidBrush(back_color);

	return{ (HPEN)SelectObject(dc, hPen), (HBRUSH)SelectObject(dc, hBrush) };
}
void draw_delete(HDC& dc, HPEN& hPen, HBRUSH& hBrush, pair<HPEN, HBRUSH> old)
{
	SelectObject(dc, old.first);
	DeleteObject(hPen);
	SelectObject(dc, old.second);
	DeleteObject(hBrush);
}

void drawBlt(HDC * dc, int x, int y, int w, int h, int srcx, int srcy, HDC * bDC)
{
	BitBlt(*dc, x, y, w, h, *bDC, srcx, srcy, SRCCOPY);
	BitBlt(*dc, x, y, w, h, *bDC, srcx + w, srcy, SRCPAINT);
}
void drawProc(HDC * dc, int x, int y, int w, int h, int back, int border, int back_color, int border_color,
	function<bool(HDC, int, int, int, int)> draw)
{
	HPEN hPen;
	HBRUSH hBrush;
	pair<HPEN, HBRUSH> old = draw_init(*dc, hPen, hBrush, back, border, back_color, border_color);

	draw(*dc, x, y, x + w, y + h);

	draw_delete(*dc, hPen, hBrush, old);
}
void drawText(HDC * dc, int x, int y, int w, int h, int size, string text, UINT flags)
{
	RECT rcDraw;
	HFONT	hFont = CreateFont(size, 0, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, 0, 0, 0, 0 | FF_SWISS, TEXT("��������")),
		hOldFont = (HFONT)SelectObject(*dc, hFont);

	SetRect(&rcDraw, x, y, x + w, y + h);
	DrawText(*dc, text.c_str(), -1, &rcDraw, DT_HIDEPREFIX | flags);

	SelectObject(*dc, hOldFont);
	DeleteObject(hFont);
}
void drawText(HDC * dc, int x, int y, int w, int h, int size, string text)
{
	drawText(dc, x, y, w, h, size, text, 0);
}

void draw_loop_text(HDC * dc, table<object_text> table)
{
	object_text pivot = table.getPivot();
	FOREACH_TABLE(table, it, object_text)
		if (it->visible)
			drawText(dc, pivot.x + it->x, pivot.y + it->y, it->w, it->h, it->size, it->text);
}
template <typename T>
void draw_loop_blt(HDC * dc, table<T> table)
{
	T pivot = table.getPivot();
	HDC bDC = CreateCompatibleDC(*dc);
	SelectObject(bDC, pivot.bitmap);

	FOREACH_TABLE(table, it, T)
		if (it->visible)
			drawBlt(dc, it->x + pivot.x, it->y + pivot.y, it->w, it->h, it->srcx, it->srcy, &bDC);

	DeleteDC(bDC);
}
template <typename T>
void draw_loop_proc(HDC * dc, table<T> table)
{
	function <bool(HDC, int, int, int, int)> proc;
	FOREACH_TABLE(table, it, T)
	{
		if (it->visible)
		{
			switch (it->type)
			{
			case ELLIPSE:
				proc = Ellipse;
				break;
			case RECTANGLE:
				proc = Rectangle;
				break;
			default:
				break;
			}
			drawProc(dc, it->x, it->y, it->w, it->h, it->back, it->border, it->back_color, it->border_color, proc);
		}
	}
}

void draw_loop_list(HDC * dc, listElements& list, const string& find)
{
	if (find == "")
	{
		static int space = 50;
		FOREACH_LIST(list, i, info)
		{
			int yy = list.y + i * space;
		
			MoveToEx(*dc, list.x, yy, NULL);
			LineTo(*dc, list.x + list.w, yy);

			drawText(dc, list.x + 10, yy + 8, list.w, yy + 32, 24, info.name);
			drawText(dc, list.x + 15, yy + 31, list.w, yy + 40, 14, info.artist);
			drawText(dc, list.x - 10, yy + 18, list.w, yy + 25, 16, info.album, DT_RIGHT);
		}
	}
	else
	{

	}
}