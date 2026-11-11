#pragma once

#include <QMainWindow>
#include <QImage>

class QTabWidget;
class QLineEdit;
class QLabel;
class QPushButton;
class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    QLineEdit* m_Input;
    QPushButton* m_Generate;
    QPushButton* m_saveBtn;
    QLabel* m_Preview;
    QLabel* m_Status;
    QImage m_currentImage;
    QTabWidget* m_tabs;

    QPushButton* m_uploadBtn;
    QLabel* m_readPreview;
    QLabel* m_decodedLabel;
    QLabel* m_readStatus;

    QWidget* createGenerateTab();
    QWidget* createReadTab();

private slots:
    void onGenerateClicked();
    void onSaveClicked();
    void onUploadClicked();
};