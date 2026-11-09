#pragma once
#include <QMainWindow>


class QTabWidget;
class QLineEdit;
class QLabel;
class QPushButton;


class MainWindow:public QMainWindow{
     Q_OBJECT
public:

    explicit MainWindow(QWidget*parent=nullptr);
private:
    QLineEdit*m_Input;
    QPushButton*m_Generate;
    QPushButton*m_savebttn;
    QLabel*m_BarcodePreview;
    QLabel*m_Status;
    QTabWidget*m_tabs   ;



    QWidget* createGenerateTab();
    void OnGenerateClicked();
};