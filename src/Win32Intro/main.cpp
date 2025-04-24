import win32;
import classic_window;
import template_window;

// __stdcall is ignored on x64 and ARM
// https://learn.microsoft.com/en-us/cpp/cpp/stdcall?view=msvc-170
auto wWinMain(
    Win32::HINSTANCE hInstance,
    Win32::HINSTANCE hPrevInstance,
    Win32::PWSTR pCmdLine, 
    int nCmdShow
) -> int
{
    //TemplateWindow::Run(nCmdShow);
    //TemplateWindowAdvanced::Run(nCmdShow);
    TemplateWindowAdvanced2::Run(nCmdShow);
    return 0;
}
