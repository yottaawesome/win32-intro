module;

#include <windows.h>

export module win32;

export namespace Win32
{
	using
		::LRESULT,
		::UINT,
		::LPARAM,
		::WPARAM,
		::HINSTANCE,
		::WNDCLASS,
		::HWND,
		::MSG,
		::PAINTSTRUCT,
		::HDC,
		::PWSTR,
		::PCWSTR,
		::HBRUSH,
		::BOOL,
		::CREATESTRUCT,
		::DWORD,
		::HMENU,
		::LONG_PTR,
		::GetWindowLongPtrW,
		::SetWindowLongPtrW,
		::GetModuleHandleW,
		::RegisterClassW,
		::CreateWindowExW,
		::ShowWindow,
		::TranslateMessage,
		::DispatchMessage,
		::GetMessageW,
		::BeginPaint,
		::EndPaint,
		::DefWindowProcW,
		::FillRect,
		::PostQuitMessage,
		::DefWindowProcW
		;

	constexpr ::DWORD WsOverlappedWindow = WS_OVERLAPPEDWINDOW;
	constexpr auto CwUseDefault = CW_USEDEFAULT;
	constexpr auto ColorWindow = COLOR_WINDOW;

	namespace Messages
	{
		constexpr auto Paint = WM_PAINT;
		constexpr auto Destroy = WM_DESTROY;
		constexpr auto NcCreate = WM_NCCREATE;
	}

	constexpr auto GwlpUserData = GWLP_USERDATA;
}
