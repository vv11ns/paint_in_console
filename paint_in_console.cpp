#include <iostream>
#include <Windows.h>

bool cmp_bytes(char* b1, char* b2, size_t size) {
    for (; size; ++b1, ++b2, --size)
        if (*b1 != *b2) return 0;
    return 1;
}
bool cmp_bytes(void* b1, void* b2, size_t size) { return cmp_bytes((char*)b1, (char*)b2, size); }
void paint_line(HDC hdc, COLORREF color, int from_x, int from_y, int to_x, int to_y) {
    bool fromxbig = false;
    bool fromybig = false;
    int count_x = from_x - to_x;
    int count_y = from_y - to_y;
    if (count_x < 0)
        count_x = -count_x;
    else fromxbig = true;
    if (count_y < 0)
        count_y = -count_y;
    else fromybig = true;

    double dfrom_x = (double)from_x;
    double dfrom_y = (double)from_y;
    double x_step = count_x == 0 ? 0 : 1;
    double y_step = count_x == 0 ? 1 : (double)count_y / (double)count_x;
    if (fromxbig) x_step = -x_step;
    if (fromybig) y_step = -y_step;
    int pixels = count_x + count_y;
    while((fromxbig ? dfrom_x > to_x : dfrom_x < to_x) || (fromybig ? dfrom_y > to_y : dfrom_y < to_y)) {
        dfrom_x += x_step;
        if (y_step > 1) {
            for (double i = 0; i < y_step; ++i, ++dfrom_y) {
                SetPixel(hdc, (int)dfrom_x, (int)dfrom_y, color);
            }
        }
        else {
            dfrom_y += y_step;
            SetPixel(hdc, (int)dfrom_x, (int)dfrom_y, color);
        }
    }
}
int main()
{
    const unsigned char color_x = 255, color_y = 255, color_z = 255; // RGB
    const COLORREF color = RGB(color_x, color_y, color_z);
    HWND hConsole_Window = GetConsoleWindow();
    HDC hConsole_Window_HDC = GetDC(hConsole_Window);
    //paint_line(hConsole_Window_HDC, color, 10, 100, 99, 10);
    //getchar();
    //return 0;
    bool session = false;
    int old_console_x = 0;
    int old_console_y = 0;
    POINT old_cursor_pos = {};
    POINT cursor_pos = {};
    RECT window_rect = {};
    while (true) {
        //Sleep(1);
        HWND hActive_Window = GetForegroundWindow();
        if (hConsole_Window == hActive_Window) {
            if (GetAsyncKeyState(' ')) {
                system("cls");
                if (session) session = false;
            }
            if (GetAsyncKeyState(MOUSEEVENTF_LEFTDOWN)) {
                GetCursorPos(&cursor_pos);
                if (!cmp_bytes(&old_cursor_pos, &cursor_pos, sizeof(POINT))) {
                    GetWindowRect(hConsole_Window, &window_rect);
                    if (cursor_pos.x >= window_rect.left && cursor_pos.x <= window_rect.right && cursor_pos.y >= window_rect.top && cursor_pos.y <= window_rect.bottom) {
                        int console_x = cursor_pos.x - window_rect.left - 8;
                        int console_y = cursor_pos.y - window_rect.top - 30;
                        if (session)
                            paint_line(hConsole_Window_HDC, color, old_console_x, old_console_y, console_x, console_y);
                        else SetPixel(hConsole_Window_HDC, console_x, console_y, color);
                        old_console_x = console_x;
                        old_console_y = console_y;
                        old_cursor_pos = cursor_pos;
                        session = true;
                    }
                    else if (session) session = false;
                }
            }
            else if (session) session = false;
        }
        else if (session) session = false;
    }
    return 0;
}