#pragma once
#include "nwpwin.h"
#include "nwpdlg.h"

class main_dialog : public vsite::nwp::dialog {
public:
	tstring tstr;
protected:
	int idd() const override;
	bool on_init_dialog() override;
	bool on_ok() override;
};

class main_window : public vsite::nwp::window {
public:
	tstring txt;
	LOGFONT lf;
	COLORREF cr{ RGB(0,0,0) };      // Boja teksta - crna
	COLORREF back{ RGB(255,255,255) }; // Pozadina - bijela
	main_window();
protected:
	void on_paint(HDC hdc) override;
	void on_command(int id) override;
	void on_destroy() override;
};

class sel_obj {
	HDC hdc;
	HGDIOBJ hOld;
public:
	sel_obj(HDC hdc, HGDIOBJ hObj) : hdc(hdc), hOld(::SelectObject(hdc, hObj)) { }
	~sel_obj() { ::SelectObject(hdc, hOld); }
};

class brush {
	HBRUSH h;
public:
	brush(COLORREF color, int hatch = -1) :
		h(hatch >= 0 ? CreateHatchBrush(hatch, color) : ::CreateSolidBrush(color)) {}
	~brush() { ::DeleteObject(h); }
	operator HBRUSH() { return h; }
};

class font {
	HFONT h;
public:
	font(const LOGFONT& lf) { h = ::CreateFontIndirect(&lf); }
	font(const TCHAR* name, int height, bool bold = false, bool italic = false, int angle = 0, bool underline = false) : h(0) {
		h = ::CreateFont(height, 0, angle * 10, 0, bold ? FW_BOLD : FW_NORMAL, italic, underline, 0, 0, 0, 0, 0, 0, name);
	}
	~font() { if (h) ::DeleteObject(h); }
	operator HFONT() { return h; }
};
