#include "mainwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QOperatingSystemVersion>

MainWindow::MainWindow(QWidget *parent)
    : NMainWindow(parent)
    , m_effectComboBox(nullptr)
{
    // 设置窗口属性
    setWindowTitle(tr("现代窗口示例"));
    resize(800, 600);
    enableWindowAnimation(true);
    // 设置UI
    setupUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // 创建中央控件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // 添加标题
    QLabel *titleLabel = new QLabel(tr("请选择窗口效果："), this);
    mainLayout->addWidget(titleLabel);
    
    // 创建效果选择下拉框
    m_effectComboBox = new QComboBox(this);
    m_effectComboBox->setStyleSheet("QComboBox { background-color: rgba(255, 255, 255, 0.1); }");
    setupEffectOptions();
    mainLayout->addWidget(m_effectComboBox);
    
    // 添加一些空间
    mainLayout->addStretch();
    
    // 连接信号
    connect(m_effectComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onEffectChanged);

    // 为所有控件设置样式，确保在模糊背景上可见
    QString styleSheet = R"(
        QLabel {
            color: rgba(0, 0, 0, 200);
            background-color: transparent;
        }
        QComboBox {
            background-color: rgba(255, 255, 255, 0.2);
            color: rgba(0, 0, 0, 200);
            border: 1px solid rgba(0, 0, 0, 60);
            border-radius: 4px;
            padding: 2px;
        }
    )";


    centralWidget->setStyleSheet(styleSheet);
}

void MainWindow::setupEffectOptions()
{
    m_effectComboBox->clear();
    
    // 添加所有平台都支持的效果
    m_effectComboBox->addItem(tr("无效果"), BackdropEffect::None);

#ifdef Q_OS_WIN
    bool isWin11 = QOperatingSystemVersion::current() >=
                   QOperatingSystemVersion(QOperatingSystemVersion::Windows, 10, 0, 22000);

    if (isWin11) {
        // Windows 11 特有效果
        m_effectComboBox->addItem(tr("Mica"), BackdropEffect::Mica);
        m_effectComboBox->addItem(tr("标签式"), BackdropEffect::Tabbed);
    }

    // Windows 10/11 都支持
    m_effectComboBox->addItem(tr("亚克力"), BackdropEffect::Acrylic);
#endif

#ifdef Q_OS_MACOS
    m_effectComboBox->addItem(tr("模糊"), BackdropEffect::Blur);
#endif

    // 设置当前选项为当前效果
    int currentIndex = m_effectComboBox->findData(currentEffect());
    if (currentIndex >= 0) {
        m_effectComboBox->setCurrentIndex(currentIndex);
    }
}

void MainWindow::onEffectChanged(int index)
{
    int effectType = m_effectComboBox->itemData(index).toInt();
    setBackdropEffect(effectType);
}
