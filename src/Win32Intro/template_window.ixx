export module template_window;
import win32;
import std;

export namespace TemplateWindow
{
    struct CreateArgs
    {
        Win32::PCWSTR WindowName;
        Win32::DWORD Style;
        Win32::DWORD ExtendedStyle = 0;
        int X = Win32::CwUseDefault;
        int Y = Win32::CwUseDefault;
        int Width = Win32::CwUseDefault;
        int Height = Win32::CwUseDefault;
        Win32::HWND hWndParent = 0;
        Win32::HMENU Menu = 0;
    };

    template<typename TDerived>
    struct BaseWindow
    {
        virtual auto Create(CreateArgs args) -> Win32::BOOL
        {
            Win32::WNDCLASS wc{
                .lpfnWndProc = WindowProc,
                .hInstance = Win32::GetModuleHandleW(nullptr),
                .lpszClassName = ClassName()
            };
            Win32::RegisterClassW(&wc);

            m_hwnd = Win32::CreateWindowExW(
                args.ExtendedStyle,
                ClassName(),
                args.WindowName,
                args.Style,
                args.X,
                args.Y,
                args.Width,
                args.Height,
                args.hWndParent,
                args.Menu,
                Win32::GetModuleHandleW(nullptr),
                this
            );
            return m_hwnd ? true : false;
        }

        virtual auto Window() const -> Win32::HWND { return m_hwnd; }
        virtual auto ClassName() const -> Win32::PCWSTR = 0;

    protected:
        virtual Win32::LRESULT HandleMessage(
            Win32::UINT uMsg,
            Win32::WPARAM wParam,
            Win32::LPARAM lParam
        ) = 0;

        static Win32::LRESULT WindowProc(
            Win32::HWND hwnd,
            Win32::UINT uMsg,
            Win32::WPARAM wParam,
            Win32::LPARAM lParam
        )
        {
            if (uMsg == Win32::Messages::NcCreate)
            {
                auto pCreate = reinterpret_cast<Win32::CREATESTRUCT*>(lParam);
                auto pThis = reinterpret_cast<TDerived*>(pCreate->lpCreateParams);
                Win32::SetWindowLongPtrW(
                    hwnd,
                    Win32::GwlpUserData,
                    reinterpret_cast<Win32::LONG_PTR>(pThis)
                );
                pThis->m_hwnd = hwnd;
                return pThis->HandleMessage(uMsg, wParam, lParam);
            }

            auto pThis = reinterpret_cast<TDerived*>(
                Win32::GetWindowLongPtrW(hwnd, Win32::GwlpUserData)
            );
            return pThis 
                ? pThis->HandleMessage(uMsg, wParam, lParam)
                : Win32::DefWindowProcW(hwnd, uMsg, wParam, lParam);
        }

        Win32::HWND m_hwnd = nullptr;
    };

    struct MainWindow final : BaseWindow<MainWindow>
    {
        auto ClassName() const noexcept -> Win32::PCWSTR override
        {
            return L"Sample Window Class";
        }

        auto HandleMessage(
            Win32::UINT uMsg,
            Win32::WPARAM wParam,
            Win32::LPARAM lParam
        ) -> Win32::LRESULT override
        {
            switch (uMsg)
            {
                case Win32::Messages::Destroy:
                {
                    Win32::PostQuitMessage(0);
                    return 0;
                }

                case Win32::Messages::Paint:
                {
                    Win32::PAINTSTRUCT ps;
                    Win32::HDC hdc = Win32::BeginPaint(m_hwnd, &ps);
                    Win32::FillRect(hdc, &ps.rcPaint, (Win32::HBRUSH)(Win32::ColorWindow + 1));
                    Win32::EndPaint(m_hwnd, &ps);
                    return 0;
                }

                default:
                    return Win32::DefWindowProcW(m_hwnd, uMsg, wParam, lParam);
            }
        }

        auto OnDestroy(this auto& self)
        {
            Win32::PostQuitMessage(0);
            return 0;
        }

        using X = int(*)(this MainWindow&);
        struct EventHandler
        {
            Win32::DWORD Event;
            X Fn = nullptr;
        };

        std::vector<EventHandler> EventHandlers{{Win32::Messages::Destroy, OnDestroy}};
    };
    
    void Run(int nCmdShow)
    {
        MainWindow main;
        main.Create({ L"TestWindow", Win32::WsOverlappedWindow });
        Win32::ShowWindow(main.Window(), nCmdShow);

        Win32::MSG msg{};
        while (Win32::GetMessageW(&msg, nullptr, 0, 0))
        {
            Win32::TranslateMessage(&msg);
            Win32::DispatchMessageW(&msg);
        }
    }
}
