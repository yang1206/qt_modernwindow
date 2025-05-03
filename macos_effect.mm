#include "macos_effect.h"
#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>
#include <QWidget>
#include <QOperatingSystemVersion>

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

    // 移除所有现有效果视图
    for (NSView *subview in [nsView subviews]) {
        if ([subview isKindOfClass:[NSVisualEffectView class]]) {
            [subview removeFromSuperview];
        }
    }

    // 根据类型应用不同效果
    switch (type) {
        case Blur: {
            // 获取是否为暗黑模式
            bool isDarkMode = MacOSEffect::isDarkMode();
            
            // 应用模糊效果
            nsView.wantsLayer = YES;
            
            // 创建模糊效果视图
            NSVisualEffectView *blurView = [[NSVisualEffectView alloc] initWithFrame:nsView.bounds];
            [blurView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
            [blurView setBlendingMode:NSVisualEffectBlendingModeBehindWindow];
            
            // 根据当前系统主题选择材质
            if (isDarkMode) {
                // 在暗黑模式下使用较暗的材质，但仍确保文本可见
                [blurView setMaterial:NSVisualEffectMaterialPopover];
                [blurView setAppearance:[NSAppearance appearanceNamed:NSAppearanceNameDarkAqua]];
            } else {
                [blurView setMaterial:NSVisualEffectMaterialPopover];
                [blurView setAppearance:[NSAppearance appearanceNamed:NSAppearanceNameAqua]];
            }
            
            [blurView setState:NSVisualEffectStateActive];
            
            // 将模糊视图放在底层
            [nsView addSubview:blurView positioned:NSWindowBelow relativeTo:nil];

            // 在全屏监听中完全隐藏模糊效果
            [[NSNotificationCenter defaultCenter] addObserverForName:NSWindowWillEnterFullScreenNotification
                                                            object:nsWindow
                                                             queue:[NSOperationQueue mainQueue]
                                                        usingBlock:^(NSNotification *notification) {
                // 全屏前隐藏模糊效果
                [blurView setHidden:YES];
            }];

            // 退出全屏时恢复
            [[NSNotificationCenter defaultCenter] addObserverForName:NSWindowDidExitFullScreenNotification
                                                            object:nsWindow
                                                             queue:[NSOperationQueue mainQueue]
                                                        usingBlock:^(NSNotification *notification) {
                // 退出全屏后显示模糊效果
                [blurView setHidden:NO];
            }];

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

void MacOSEffect::setWindowCornerRadius(QWidget* window, qreal radius) {
    NSView *nsView = reinterpret_cast<NSView *>(window->winId());
    if (!nsView) {
        return;
    }

    NSWindow *nsWindow = [nsView window];
    if (!nsWindow) {
        return;
    }
    
    // 确保窗口不透明属性设置正确
    [nsWindow setOpaque:NO];
    
    // 使用Qt方式检查macOS版本
    bool isMacOS11OrGreater = QOperatingSystemVersion::current() >=
                             QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 11, 0);
    
    if (isMacOS11OrGreater) {
        // 使用运行时机制设置圆角
        SEL cornerRadiusSelector = NSSelectorFromString(@"setCornerRadius:");
        if ([nsWindow respondsToSelector:cornerRadiusSelector]) {
            // 转换double为NSNumber
            NSNumber *radiusValue = [NSNumber numberWithDouble:radius];
            [nsWindow performSelector:cornerRadiusSelector withObject:radiusValue];
        }
    } else {
        // 对于旧版macOS，通过layer设置圆角
        nsView.wantsLayer = YES;
        nsView.layer.cornerRadius = radius;
        nsView.layer.masksToBounds = YES;
    }
}

void MacOSEffect::addWindowShadowEffect(QWidget* window, qreal shadowRadius) {
    NSView *nsView = reinterpret_cast<NSView *>(window->winId());
    if (!nsView) {
        return;
    }

    NSWindow *nsWindow = [nsView window];
    if (!nsWindow) {
        return;
    }
    
    // 启用窗口阴影
    [nsWindow setHasShadow:YES];
    
    // 设置窗口可由背景移动
    [nsWindow setMovableByWindowBackground:YES];
    
    // 通过CALayer设置阴影参数
    if (shadowRadius > 0) {
        nsView.wantsLayer = YES;
        
        // 使用运行时方式访问CALayer属性
        id layer = nsWindow.contentView.layer;
        
        // 设置阴影不透明度
        SEL shadowOpacitySelector = NSSelectorFromString(@"setShadowOpacity:");
        if ([layer respondsToSelector:shadowOpacitySelector]) {
            NSNumber *opacityValue = [NSNumber numberWithFloat:0.4];
            [layer performSelector:shadowOpacitySelector withObject:opacityValue];
        }
        
        // 设置阴影颜色
        SEL shadowColorSelector = NSSelectorFromString(@"setShadowColor:");
        if ([layer respondsToSelector:shadowColorSelector]) {
            CGColorRef blackColor = CGColorCreateGenericRGB(0, 0, 0, 1.0);
            [layer performSelector:shadowColorSelector withObject:(__bridge id)blackColor];
            CGColorRelease(blackColor);
        }
        
        // 设置阴影半径
        SEL shadowRadiusSelector = NSSelectorFromString(@"setShadowRadius:");
        if ([layer respondsToSelector:shadowRadiusSelector]) {
            NSNumber *radiusValue = [NSNumber numberWithDouble:shadowRadius];
            [layer performSelector:shadowRadiusSelector withObject:radiusValue];
        }
        
        // 设置阴影偏移
        SEL shadowOffsetSelector = NSSelectorFromString(@"setShadowOffset:");
        if ([layer respondsToSelector:shadowOffsetSelector]) {
            NSValue *offsetValue = [NSValue valueWithSize:NSMakeSize(0, -5)];
            [layer performSelector:shadowOffsetSelector withObject:offsetValue];
        }
    }
}

void MacOSEffect::setWindowAnimationBehavior(QWidget* window, bool enable) {
    NSView *nsView = reinterpret_cast<NSView *>(window->winId());
    if (nsView) {
        NSWindow *nsWindow = [nsView window];
        if (nsWindow) {
            if (enable) {
                [nsWindow setAnimationBehavior:NSWindowAnimationBehaviorDefault];
            } else {
                [nsWindow setAnimationBehavior:NSWindowAnimationBehaviorNone];
            }
        }
    }
}

bool MacOSEffect::isDarkMode() {
    NSDictionary *dict = [[NSUserDefaults standardUserDefaults] persistentDomainForName:NSGlobalDomain];
    id style = [dict objectForKey:@"AppleInterfaceStyle"];
    return (style && [style isKindOfClass:[NSString class]] && 
           NSOrderedSame == [style caseInsensitiveCompare:@"dark"]);
}
