#include "nmainwindow.h"

#include <QOperatingSystemVersion>

#ifdef Q_OS_WIN
#include "windows_effect.h"
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
    // 其他平台不需要特殊初始化
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
    // Windows平台使用特殊效果
    return WindowsEffect::setWindowBackdropEffect(this, type);
#else
    // 其他平台直接忽略效果设置，返回成功
    return true;
#endif
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
#else
    // 其他平台默认无效果
    return BackdropEffect::None;
#endif
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
    // 其他平台不进行特殊处理
}
