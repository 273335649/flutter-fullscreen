#include <flutter/dart_project.h>
#include <flutter/flutter_view_controller.h>
#include <windows.h>

#include "flutter_window.h"
#include "utils.h"

int APIENTRY wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prev,
                      _In_ wchar_t *command_line, _In_ int show_command) {
  // Attach to console when present (e.g., 'flutter run') or create a
  // new console when running with a debugger.
  if (!::AttachConsole(ATTACH_PARENT_PROCESS) && ::IsDebuggerPresent()) {
    CreateAndAttachConsole();
  }

  // Initialize COM, so that it is available for use in the library and/or
  // plugins.
  ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

  flutter::DartProject project(L"data");

  std::vector<std::string> command_line_arguments =
      GetCommandLineArguments();

  project.set_dart_entrypoint_arguments(std::move(command_line_arguments));

  FlutterWindow window(project);
  Win32Window::Point origin(0, 0);
  Win32Window::Size size(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
  if (!window.Create(L"flutter_app1", origin, size)) {
    return EXIT_FAILURE;
  }
  window.SetQuitOnClose(true);

  // 添加全屏设置代码
  HWND hwnd = ::FindWindowW(nullptr, L"flutter_app1");
  if (hwnd != nullptr) {
    // 移除标题栏和边框
    LONG_PTR style = ::GetWindowLongPtrW(hwnd, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
    ::SetWindowLongPtrW(hwnd, GWL_STYLE, style);
    // 设置窗口大小为全屏
    ::SetWindowPos(hwnd, nullptr, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOZORDER | SWP_FRAMECHANGED);
  }

  ::MSG msg;
  while (::GetMessage(&msg, nullptr, 0, 0)) {
    // 处理F11全屏切换
    if (msg.message == WM_KEYDOWN && msg.wParam == VK_F11) {
      HWND hwndWindow = msg.hwnd;
      if (hwndWindow != nullptr) {
        LONG_PTR style = ::GetWindowLongPtrW(hwndWindow, GWL_STYLE);
        bool isFullscreen = (style & WS_CAPTION) == 0;
        if (isFullscreen) {
          // 恢复窗口样式
          style |= WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
          ::SetWindowLongPtrW(hwndWindow, GWL_STYLE, style);
          ::SetWindowPos(hwndWindow, nullptr, 100, 100, 800, 600, SWP_NOZORDER | SWP_FRAMECHANGED);
        } else {
          // 设置全屏样式
          style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
          ::SetWindowLongPtrW(hwndWindow, GWL_STYLE, style);
          ::SetWindowPos(hwndWindow, nullptr, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOZORDER | SWP_FRAMECHANGED);
        }
        continue;
      }
    }
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }

  ::CoUninitialize();
  return EXIT_SUCCESS;
}
