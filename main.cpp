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
    cf.rgbColors = cr;
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

    const int bits_per_char = sizeof(TCHAR) * 8;

    const double cell_width = rect.right / static_cast<double>(bits_per_char + 1);
    const double line_height = rect.bottom / static_cast<double>(txt.size() ? txt.size() : 1);

    for (size_t i = 0; i < txt.size(); ++i) {
        TCHAR ch = txt[i];

        for (int bit = 0; bit < bits_per_char; ++bit) {
            bool is_one = (ch & (1 << (bits_per_char - 1 - bit))) != 0;
            RECT r = {
                LONG(bit * cell_width),
                LONG(i * line_height),
                LONG((bit + 1) * cell_width),
                LONG((i + 1) * line_height)
            };
            FillRect(hdc, &r, is_one ? whiteBrush : blackBrush);
        }
        RECT r = {
            LONG(bits_per_char * cell_width),
            LONG(i * line_height),
            LONG((bits_per_char + 1) * cell_width),
            LONG((i + 1) * line_height)
        };
        TCHAR letter[2] = { ch, 0 };
        DrawText(hdc, letter, 1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

void main_window::on_command(int id) {
    switch (id) {
    case ID_FONT:
        get_font(*this, lf, cr);
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
