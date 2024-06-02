module;

#include <Windows.h>

export module main_window;
import base_window;

export struct MainWindow final : public BaseWindow<MainWindow>
{
    PCWSTR ClassName() const noexcept override 
    { 
        return L"Sample Window Class"; 
    }

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override
    {
        switch (uMsg)
        {
            case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            }

            case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(m_hwnd, &ps);
                FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
                EndPaint(m_hwnd, &ps);
                return 0;
            }

            default:
                return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
        }
    }
};
