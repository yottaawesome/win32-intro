module;

#include <Windows.h>

export module main_window;
import base_window;
export class MainWindow : public BaseWindow<MainWindow>
{
    public:
        virtual PCWSTR  ClassName() const override { return L"Sample Window Class"; }
        virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};
