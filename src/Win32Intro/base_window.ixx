module;

#include <Windows.h>

export module base_window;

export template <class DERIVED_TYPE>
class BaseWindow
{
    public:
        virtual BOOL Create(
            PCWSTR lpWindowName,
            DWORD dwStyle,
            DWORD dwExStyle = 0,
            int x = CW_USEDEFAULT,
            int y = CW_USEDEFAULT,
            int nWidth = CW_USEDEFAULT,
            int nHeight = CW_USEDEFAULT,
            HWND hWndParent = 0,
            HMENU hMenu = 0
        )
        {
            WNDCLASS wc{ 
                .lpfnWndProc = WindowProc,
                .hInstance = GetModuleHandle(nullptr),
                .lpszClassName = ClassName()
            };
            RegisterClassW(&wc);

            m_hwnd = CreateWindowExW(
                dwExStyle, 
                ClassName(), 
                lpWindowName, 
                dwStyle, 
                x, 
                y,
                nWidth, 
                nHeight, 
                hWndParent, 
                hMenu, 
                GetModuleHandleW(nullptr), 
                this
            );
            return m_hwnd ? true : false;
        }

        virtual HWND Window() const { return m_hwnd; }
        virtual PCWSTR ClassName() const = 0;

    protected:
        virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {
            DERIVED_TYPE* pThis = nullptr;

            if (uMsg == WM_NCCREATE)
            {
                CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
                pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
                SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

                pThis->m_hwnd = hwnd;
            }
            else
            {
                pThis = (DERIVED_TYPE*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            }

            if (pThis)
            {
                return pThis->HandleMessage(uMsg, wParam, lParam);
            }
            else
            {
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
            }
        }

    protected:
        HWND m_hwnd = nullptr;
};
