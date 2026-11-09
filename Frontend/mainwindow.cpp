#include "mainwindow.h"
#include <QTabWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QBoxLayout>
#include <QRegularExpressionValidator>
#include <iostream>

using namespace std;
MainWindow::MainWindow(QWidget*parent):QMainWindow(parent){
    setWindowTitle("Barcode Reader");
    resize(700,500);
    m_tabs=new QTabWidget(this);
    m_tabs->addTab(createGenerateTab(),"Generator");
    m_tabs->addTab(createGenerateTab(),"Reader");
    setCentralWidget(m_tabs);

    setStyleSheet("QPushButton:hover { background-color: #D97757; color: white; }");
}
QWidget*MainWindow::createGenerateTab(){
    QWidget*page=new QWidget;
    QVBoxLayout*layout=new QVBoxLayout(page);

    QHBoxLayout *inputrows=new QHBoxLayout;
    QLabel*qq=new QLabel("6-digit number:");
    m_Input=new QLineEdit;
    m_Input->setPlaceholderText("Six digit Num");
    m_Input->setMaxLength(6);
    m_Input->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{0,6}"),m_Input));

    inputrows->addWidget(qq);
    inputrows->addWidget(m_Input);
    m_Generate=new QPushButton("Generate");
    inputrows->addWidget(m_Generate);

    layout->addLayout(inputrows);

    m_Status=new QLabel;
    layout->addWidget(m_Status);

    connect(m_Generate,&QPushButton::clicked,this,&MainWindow::OnGenerateClicked);
    connect(m_Input,&QLineEdit::returnPressed,this,&MainWindow::OnGenerateClicked);
    return page;
}

void MainWindow::OnGenerateClicked(){
    if(m_Input->text().size()!=6){
        m_Status->setText("Error:enter exactly 6 digits");
        return;
    }
    else{
        m_Status->setText("Input OK:)");
    }
}