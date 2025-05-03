#include "windows_effect.h"
#include <QOperatingSystemVersion>

// Windows 10/11 相关常量定义
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#ifndef DWMWA_MICA_EFFECT
#define DWMWA_MICA_EFFECT 1029
#endif

#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif

bool WindowsEffect::isWindows11OrGreater() {
    return QOperatingSystemVersion::current() >=
           QOperatingSystemVersion(QOperatingSystemVersion::Windows, 10, 0, 22000);
}

void WindowsEffect::initWindowEffect(QWidget* window) {
    // 设置窗口背景透明
    window->setAttribute(Qt::WA_TranslucentBackground);
    window->setStyleSheet("background-color: transparent;");

    // 扩展窗口框架到客户区
    const HWND hwnd = reinterpret_cast<HWND>(window->winId());
    const MARGINS margins = {0, 0, 0, -1}; // -1表示整个客户区
    DwmExtendFrameIntoClientArea(hwnd, &margins);
}

bool WindowsEffect::setWindowBackdropEffect(QWidget* window, int type) {
    const HWND hwnd = reinterpret_cast<HWND>(window->winId());

    if (isWindows11OrGreater()) {
        // Windows 11 - 直接尝试设置SystemBackdropType
        HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &type, sizeof(type));

        if (SUCCEEDED(hr)) {
            return true;
        }
    }

    // Windows 10回退方案
    if (type == BackdropType::Acrylic) {
        return applyAcrylicEffect(window);
    } else if (type == BackdropType::Mica) {
        return applyMicaEffect(window);
    } else if (type == BackdropType::None) {
        return disableWindowEffects(window);
    }

    return false;
}

bool WindowsEffect::applyAcrylicEffect(QWidget* window) {
    const HWND hwnd = reinterpret_cast<HWND>(window->winId());

    // 尝试使用Windows 10的亚克力效果
    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (!hUser) {
        return false;
    }

    pfnSetWindowCompositionAttribute setWindowCompositionAttribute =
        (pfnSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");

    if (!setWindowCompositionAttribute) {
        return false;
    }

    // 设置亚克力效果 - 修复颜色值问题
    ACCENTPOLICY accent = {
        ACCENT_ENABLE_ACRYLICBLURBEHIND,  // 亚克力模糊
        0,
        0x80F0F0F0,  // 使用小于INT_MAX的值
        0
    };

    WINCOMPATTRDATA data = {
        19,  // WCA_ACCENT_POLICY
        &accent,
        sizeof(accent)
    };

    return setWindowCompositionAttribute(hwnd, &data);
}

bool WindowsEffect::applyMicaEffect(QWidget* window) {
    const HWND hwnd = reinterpret_cast<HWND>(window->winId());

    // 尝试在Windows 10中启用Mica
    BOOL micaEnabled = TRUE;
    HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_MICA_EFFECT, &micaEnabled, sizeof(micaEnabled));

    if (SUCCEEDED(hr)) {
        return true;
    }

    // 如果失败，尝试使用暗模式
    BOOL darkMode = TRUE;
    hr = DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode));

    if (SUCCEEDED(hr)) {
        return true;
    }

    // 最后回退到亚克力效果
    return applyAcrylicEffect(window);
}

bool WindowsEffect::disableWindowEffects(QWidget* window) {
    const HWND hwnd = reinterpret_cast<HWND>(window->winId());

    if (isWindows11OrGreater()) {
        int type = BackdropType::None;
        DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &type, sizeof(type));
    } else {
        // Windows 10 - 禁用特效
        HMODULE hUser = GetModuleHandle(L"user32.dll");
        if (!hUser) {
            return false;
        }

        pfnSetWindowCompositionAttribute setWindowCompositionAttribute =
            (pfnSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");

        if (!setWindowCompositionAttribute) {
            return false;
        }

        ACCENTPOLICY accent = { ACCENT_DISABLED, 0, 0, 0 };
        WINCOMPATTRDATA data = { 19, &accent, sizeof(accent) };
        setWindowCompositionAttribute(hwnd, &data);
    }

    return true;
}
