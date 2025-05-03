#pragma once

#include "nmainwindow.h"
#include <QComboBox>

class MainWindow : public NMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void setupUI();
    void setupEffectOptions();

private slots:
    void onEffectChanged(int index);

private:
    QComboBox *m_effectComboBox;
};