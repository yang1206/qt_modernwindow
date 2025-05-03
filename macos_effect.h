#pragma once

#include <QWidget>

// macOS背景效果类
class MacOSEffect {
public:
    // 背景效果类型
    enum BackdropType {
        None = 0,  // 无效果
        Blur = 1   // 模糊效果
    };

    static void initWindowEffect(QWidget* window);
    static bool setWindowBackdropEffect(QWidget* window, int type);
    static void setWindowCornerRadius(QWidget* window, qreal radius = 10.0);
    static void addWindowShadowEffect(QWidget* window, qreal shadowRadius = 20.0);
    static void setWindowAnimationBehavior(QWidget* window, bool enable);
    static bool isDarkMode();
};
