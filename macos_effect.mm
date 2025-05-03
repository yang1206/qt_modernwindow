#include "macos_effect.h"
#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>
#include <QWidget>

void MacOSEffect::initWindowEffect(QWidget* window) {
    // 设置窗口背景透明
    window->setAttribute(Qt::WA_TranslucentBackground);
    window->setStyleSheet("background-color: transparent;");

    // 获取macOS原生窗口
    NSView *nsView = reinterpret_cast<NSView *>(window->winId());
    if (nsView) {
        NSWindow *nsWindow = [nsView window];
        if (nsWindow) {
            // 设置窗口属性
            [nsWindow setTitlebarAppearsTransparent:YES];
            [nsWindow setStyleMask:[nsWindow styleMask] | NSWindowStyleMaskFullSizeContentView];
            [nsWindow setMovableByWindowBackground:YES];
        }
    }
}

bool MacOSEffect::setWindowBackdropEffect(QWidget* window, int type) {
    NSView *nsView = reinterpret_cast<NSView *>(window->winId());
    if (!nsView) {
        return false;
    }

    NSWindow *nsWindow = [nsView window];
    if (!nsWindow) {
        return false;
    }

    // 修复：移除所有现有效果视图，放在switch之前
    for (NSView *subview in [nsView subviews]) {
        if ([subview isKindOfClass:[NSVisualEffectView class]]) {
            [subview removeFromSuperview];
        }
    }

    // 根据类型应用不同效果
    switch (type) {
        case Blur: {
            // 应用模糊效果
            nsWindow.appearance = [NSAppearance appearanceNamed:NSAppearanceNameVibrantLight];
            nsView.wantsLayer = YES;

            // 设置模糊效果
            NSVisualEffectView *blurView = [[NSVisualEffectView alloc] initWithFrame:nsView.bounds];
            [blurView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
            [blurView setBlendingMode:NSVisualEffectBlendingModeBehindWindow];
            [blurView setMaterial:NSVisualEffectMaterialHUDWindow]; // 选择适合的材质
            [blurView setState:NSVisualEffectStateActive];

            [nsView addSubview:blurView positioned:NSWindowBelow relativeTo:nil];
            return true;
        }

        case None:
            // 移除效果已经在switch前完成
            return true;

        default:
            // 不支持的效果类型
            return false;
    }
}
