#pragma once

#include <QMainWindow>

// 统一背景效果类型
namespace BackdropEffect {
enum Type {
    None = 0,       // 无特殊效果
    Blur = 1,       // 模糊效果 (macOS)
    Mica = 2,       // Mica效果 (Windows 11)
    Acrylic = 3,    // 亚克力效果 (Windows 10/11)
    Tabbed = 4      // 标签式效果 (Windows 11)
};
}

class NMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit NMainWindow(QWidget *parent = nullptr);
    virtual ~NMainWindow();
    
    // 设置窗口背景效果
    bool setBackdropEffect(int effectType);
    
    // 获取当前效果
    int currentEffect() const;
    
protected:
    // 初始化窗口，子类可以重写此方法添加额外初始化
    virtual void initWindow();
    
private:
    // 初始化平台特定窗口效果
    void initPlatformEffect();
    
    // 平台特定效果设置
    bool setPlatformEffect(int type);

    int getDefaultEffect();
    
    int m_currentEffect;
};