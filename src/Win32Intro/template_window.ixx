export module template_window;
import std;
import win32;
import util;

//
//
// A basic Window example with CRTP.
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

//
// 
// A more advanced example where the message processing 
// functions are mapped to an array. Uses C++23's 
// deducing this.
export namespace TemplateWindowAdvanced
{
    struct CreateArgs
    {
        Win32::PCWSTR WindowName = nullptr;
        Win32::DWORD Style = 0;
        Win32::DWORD ExtendedStyle = 0;
        int X = Win32::CwUseDefault;
        int Y = Win32::CwUseDefault;
        int Width = Win32::CwUseDefault;
        int Height = Win32::CwUseDefault;
        Win32::HWND hWndParent = 0;
        Win32::HMENU Menu = 0;
    };

    //
    //
    // BaseWindow
    template<typename TDerived, Util::FixedString VClassName>
    struct BaseWindow
    {
        virtual auto Create(CreateArgs args) -> Win32::BOOL
        {
            Win32::WNDCLASS wc{
                .lpfnWndProc = WindowProc,
                .hInstance = Win32::GetModuleHandleW(nullptr),
                .lpszClassName = VClassName.Data()
            };
            Win32::RegisterClassW(&wc);

            m_hwnd = Win32::CreateWindowExW(
                args.ExtendedStyle,
                VClassName.Data(),
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

        virtual auto Window() const -> Win32::HWND final { return m_hwnd; }
        virtual auto ClassName() const -> std::wstring_view { return VClassName.View(); };

    protected:
        using EventFn = int(*)(this TDerived&, Win32::WPARAM wParam, Win32::LPARAM lParam);
        struct EventHandler
        {
            Win32::DWORD Event;
            EventFn Fn = nullptr;
        };

        auto HandleMessage(
            this auto& self,
            Win32::UINT uMsg,
            Win32::WPARAM wParam,
            Win32::LPARAM lParam
        ) -> Win32::LRESULT
        {
            for (auto& handler : self.GetHandlers())
                if (handler.Event == uMsg)
                    return handler.Fn(self, wParam, lParam);

            self[uMsg](self, wParam, lParam);

            return Win32::DefWindowProcW(self.m_hwnd, uMsg, wParam, lParam);
        }

        static auto WindowProc(
            Win32::HWND hwnd,
            Win32::UINT uMsg,
            Win32::WPARAM wParam,
            Win32::LPARAM lParam
        ) -> Win32::LRESULT
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

    //
    //
    // Capability type
    struct Minimisable
    {
        void Minimise(this auto& self)
        {
            Win32::ShowWindow(self.Window(), Win32::ShowWindowValues::Minimise);
        }

        void Maximise(this auto& self)
        {
            Win32::ShowWindow(self.Window(), Win32::ShowWindowValues::Maximise);
        }

        void Restore(this auto& self)
        {
            Win32::ShowWindow(self.Window(), Win32::ShowWindowValues::Restore);
        }
    };

    //
    //
    // MainWindow
    struct MainWindow final : 
        BaseWindow<MainWindow, L"Sample Window Class">, 
        Minimisable
    {
        auto OnPaint(
            this auto& self, 
            Win32::WPARAM wParam, 
            Win32::LPARAM lParam
        ) -> int
        {
            Win32::PAINTSTRUCT ps;
            Win32::HDC hdc = Win32::BeginPaint(self.m_hwnd, &ps);
            Win32::FillRect(hdc, &ps.rcPaint, (Win32::HBRUSH)(Win32::ColorWindow + 1));
            Win32::EndPaint(self.m_hwnd, &ps);
            return 0;
        }

        auto OnDestroy(
            this auto& self, 
            Win32::WPARAM wParam, 
            Win32::LPARAM lParam
        ) -> int
        {
            Win32::PostQuitMessage(0);
            return 0;
        }

        auto GetHandlers()
        {
            return std::array{
                EventHandler{ Win32::Messages::Destroy, OnDestroy },
                EventHandler{ Win32::Messages::Paint, OnPaint }
            }; 
        }

        EventFn operator[](Win32::DWORD event) const
        {
            if (event == Win32::Messages::Destroy)
                return OnDestroy;
            if (event == Win32::Messages::Paint)
                return OnPaint;
            return nullptr;
        }
    };

    //
    //
    // Run()
    auto Run(int nCmdShow) -> int
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
        return 0;
    }
}