export module classic_window;
import win32;

// The classic way of creating windows
export namespace ClassicWindow
{
    auto WindowProc(
        Win32::HWND hwnd,
        Win32::UINT uMsg,
        Win32::WPARAM wParam,
        Win32::LPARAM lParam
    ) -> Win32::LRESULT
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
                Win32::HDC hdc = Win32::BeginPaint(hwnd, &ps);

                Win32::FillRect(
                    hdc,
                    &ps.rcPaint,
                    (Win32::HBRUSH)(Win32::ColorWindow + 1)
                );

                Win32::EndPaint(hwnd, &ps);
                return 0;
            }
        }

        return Win32::DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }

	auto Run(Win32::HINSTANCE hInstance, int nCmdShow) -> int
	{
        // Register the window class.
        constexpr wchar_t ClassName[] = L"Sample Window Class";

        Win32::WNDCLASS wc{
            .lpfnWndProc = WindowProc,
            .hInstance = hInstance,
            .lpszClassName = ClassName
        };

        Win32::RegisterClassW(&wc);

        // Create the window.
        Win32::HWND hwnd = Win32::CreateWindowExW(
            0,                              // Optional window styles.
            ClassName,                     // Window class
            L"Learn to Program Windows",    // Window text
            Win32::WsOverlappedWindow,            // Window style
            // Size and position
            Win32::CwUseDefault,
            Win32::CwUseDefault,
            Win32::CwUseDefault,
            Win32::CwUseDefault,
            nullptr,       // Parent window    
            nullptr,       // Menu
            hInstance,  // Instance handle
            nullptr        // Additional application data
        );
        if (not hwnd)
            return 1;

        Win32::ShowWindow(hwnd, nCmdShow);

        // Run the message loop.
        Win32::MSG msg = { };
        while (Win32::GetMessageW(&msg, nullptr, 0, 0))
        {
            Win32::TranslateMessage(&msg);
            Win32::DispatchMessageW(&msg);
        }

        return 0;
	}
}
