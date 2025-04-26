#include "main.h"
#include "rc.h"
#include <windows.h>
#include <tchar.h>

void get_font(HWND parent, LOGFONT& lf, COLORREF& cr) {
    CHOOSEFONT cf;
    LOGFONT chosenFont = lf;
    ZeroMemory(&cf, sizeof cf);
    cf.lStructSize = sizeof cf;
    cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS;
    cf.lpLogFont = &chosenFont;
    cf.hwndOwner = parent;
    if (ChooseFont(&cf)) {
        cr = cf.rgbColors;
        lf = chosenFont;
    }
}

COLORREF get_color(HWND parent, COLORREF cur) {
    static COLORREF custom_colors[16]{ 0 };
    CHOOSECOLOR cc{ sizeof CHOOSECOLOR };
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;
    cc.hwndOwner = parent;
    cc.lpCustColors = custom_colors;
    cc.rgbResult = cur;
    if (ChooseColor(&cc))
        cur = cc.rgbResult;
    return cur;
}

main_window::main_window() {
    ZeroMemory(&lf, sizeof(lf));
    _tcscpy_s(lf.lfFaceName, _T("Arial"));
    HDC hdc = ::GetDC(0);
    lf.lfHeight = -18 * ::GetDeviceCaps(hdc, LOGPIXELSY) / 72;
    ::ReleaseDC(0, hdc);
}

void main_window::on_paint(HDC hdc) {
    RECT rect;
    GetClientRect(*this, &rect);

    brush backgroundColor(back);
    sel_obj background(hdc, backgroundColor);
    FillRect(hdc, &rect, backgroundColor);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, cr);

    font f(lf);
    sel_obj selectedFont(hdc, f);

    HBRUSH blackBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
    HBRUSH whiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);

    const double x = rect.right / 9.0;
    const double y = rect.bottom / static_cast<double>(txt.size());

    for (size_t i = 0; i < txt.size(); ++i) {
        unsigned char ch = txt[i];
        for (int j = 0; j < 8; ++j) {
            RECT r = {
                LONG(j * x),
                LONG(i * y),
                LONG((j + 1) * x),
                LONG((i + 1) * y)
            };
            FillRect(hdc, &r, (ch & (1 << (7 - j))) ? whiteBrush : blackBrush);
        }
        RECT r = {
            LONG(8 * x),
            LONG(i * y),
            LONG(9 * x),
            LONG((i + 1) * y)
        };
        TCHAR letter[2] = { txt[i], 0 };
        DrawText(hdc, letter, 1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

void main_window::on_command(int id) {
    switch (id) {
    case ID_FONT:
        get_font(*this, lf, cr);
        break;
    case ID_COLOR:
        back = get_color(*this, back);
        break;
    case ID_TEXT: {
        main_dialog t;
        t.tstr = txt;
        if (t.do_modal(0, *this) == IDOK)
            txt = t.tstr;
        break;
    }
    case ID_EXIT:
        DestroyWindow(*this);
        break;
    }
    InvalidateRect(*this, NULL, TRUE);
}

void main_window::on_destroy() {
    PostQuitMessage(0);
}

int main_dialog::idd() const {
    return IDD_DIALOG;
}

bool main_dialog::on_init_dialog() {
    set_text(IDC_EDIT1, tstr);
    return true;
}

bool main_dialog::on_ok() {
    tstr = get_text(IDC_EDIT1);
    return true;
}

int WINAPI WinMain(HINSTANCE hi, HINSTANCE, LPSTR, int) {
    vsite::nwp::application app;
    main_window wnd;
    wnd.create(0, WS_OVERLAPPEDWINDOW | WS_VISIBLE, _T("NWP"), (UINT_PTR)LoadMenu(hi, MAKEINTRESOURCE(IDM_MAIN)));
    return app.run();
}
