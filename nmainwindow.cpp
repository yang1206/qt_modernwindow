#include "nmainwindow.h"

// 平台特定头文件
#ifdef Q_OS_WIN
#include "windows_effect.h"
#endif

#ifdef Q_OS_MACOS
#include "macos_effect.h"
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

#if !defined(Q_OS_WIN) && !defined(Q_OS_MACOS)
    // 其他平台的基本设置
    setStyleSheet("background-color: white;");
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
