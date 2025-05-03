#include "nmainwindow.h"

#include <QOperatingSystemVersion>

#ifdef Q_OS_WIN
#include "windows_effect.h"
#endif

#ifdef Q_OS_MACOS
#include "macos_effect.h"

// 前向声明Objective-C类
#ifdef __OBJC__
@class NSView;
@class NSWindow;
#else
class NSView;
class NSWindow;
#endif
#endif

NMainWindow::NMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_currentEffect(BackdropEffect::None)
{
    // 初始化窗口
    initWindow();
}

NMainWindow::~NMainWindow()
{
}

void NMainWindow::initWindow()
{
    // 初始化平台特定效果
    initPlatformEffect();

    // 设置默认效果
    setBackdropEffect(getDefaultEffect());
}

void NMainWindow::initPlatformEffect()
{
#ifdef Q_OS_WIN
    WindowsEffect::initWindowEffect(this);
#endif

#ifdef Q_OS_MACOS
    MacOSEffect::initWindowEffect(this);
#endif
}

bool NMainWindow::setBackdropEffect(int effectType)
{
    bool success = setPlatformEffect(effectType);

    if (success) {
        m_currentEffect = effectType;
    }

    return success;
}

bool NMainWindow::setPlatformEffect(int type)
{
#ifdef Q_OS_WIN
    return WindowsEffect::setWindowBackdropEffect(this, type);
#endif

#ifdef Q_OS_MACOS
    // 修复macOS的反向类型映射问题
    int macOSType;
    switch (type) {
        case BackdropEffect::None:
            macOSType = MacOSEffect::None;
            break;
        case BackdropEffect::Blur:
            macOSType = MacOSEffect::Blur;
            break;
        default:
            macOSType = MacOSEffect::None;
    }
    return MacOSEffect::setWindowBackdropEffect(this, macOSType);
#endif

    // 其他平台不支持特殊效果
    return false;
}

int NMainWindow::currentEffect() const
{
    return m_currentEffect;
}

int NMainWindow::getDefaultEffect()
{
#ifdef Q_OS_WIN
    bool isWin11 = QOperatingSystemVersion::current() >=
                   QOperatingSystemVersion(QOperatingSystemVersion::Windows, 10, 0, 22000);
    return isWin11 ? BackdropEffect::Mica : BackdropEffect::Acrylic;
#endif

#ifdef Q_OS_MACOS
    return BackdropEffect::None;
#endif

    return BackdropEffect::None;
}

void NMainWindow::enableWindowAnimation(bool enable) {
#ifdef Q_OS_WIN
    HWND hwnd = reinterpret_cast<HWND>(this->winId());
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);

    if (!enable) {
        style &= ~WS_CAPTION;
        style &= ~WS_THICKFRAME;
    } else {
        style |= WS_CAPTION;
        style |= WS_THICKFRAME;
    }

    SetWindowLong(hwnd, GWL_STYLE, style);
#endif

#ifdef Q_OS_MACOS
    // 调用ObjC实现的辅助函数，而不是直接在C++代码中使用ObjC语法
    MacOSEffect::setWindowAnimationBehavior(this, enable);
#endif
}
