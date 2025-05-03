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
    
    // 设置窗口圆角
    nsWindow.appearance = [NSAppearance currentAppearance];
    
    // 使用运行时机制检查方法是否可用
    SEL cornerMaskSelector = NSSelectorFromString(@"setCornerMask:");
    SEL cornerRadiusSelector = NSSelectorFromString(@"setCornerRadius:");
    
    if ([nsWindow respondsToSelector:cornerMaskSelector] && 
        [nsWindow respondsToSelector:cornerRadiusSelector]) {
        // macOS 11及以上版本支持的方法
        NSInteger cornerMaskValue = 7; // NSWindowRoundedCornersMask
        NSNumber *maskValue = [NSNumber numberWithInteger:cornerMaskValue];
        NSNumber *radiusValue = [NSNumber numberWithDouble:radius];
        
        [nsWindow performSelector:cornerMaskSelector withObject:maskValue];
        [nsWindow performSelector:cornerRadiusSelector withObject:radiusValue];
    } else {
        // 对于较早版本，可以通过layer设置圆角
        [nsWindow setOpaque:NO];
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
    
    [nsWindow setHasShadow:YES];
    
    // macOS 10.14+支持的方法
    SEL roundedCornersSelector = NSSelectorFromString(@"setShadowsHaveRoundedCorners:");
    if ([nsWindow respondsToSelector:roundedCornersSelector]) {
        NSNumber *yesValue = [NSNumber numberWithBool:YES];
        [nsWindow performSelector:roundedCornersSelector withObject:yesValue];
    }
    
    // 设置内容边框厚度（用于阴影）
    SEL borderThicknessSelector = NSSelectorFromString(@"setContentBorderThickness:forEdge:");
    if ([nsWindow respondsToSelector:borderThicknessSelector]) {
        // 转换参数类型
        NSNumber *thicknessValue = [NSNumber numberWithDouble:shadowRadius];
        NSNumber *edgeBottom = [NSNumber numberWithInteger:0]; // NSRectEdgeBottom
        NSNumber *edgeLeft = [NSNumber numberWithInteger:1];   // NSRectEdgeLeft
        NSNumber *edgeRight = [NSNumber numberWithInteger:2];  // NSRectEdgeRight
        NSNumber *edgeTop = [NSNumber numberWithInteger:3];    // NSRectEdgeTop
        
        // 对每个边缘调用方法
        [nsWindow performSelector:borderThicknessSelector withObject:thicknessValue withObject:edgeBottom];
        [nsWindow performSelector:borderThicknessSelector withObject:thicknessValue withObject:edgeLeft];
        [nsWindow performSelector:borderThicknessSelector withObject:thicknessValue withObject:edgeRight];
        [nsWindow performSelector:borderThicknessSelector withObject:thicknessValue withObject:edgeTop];
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
