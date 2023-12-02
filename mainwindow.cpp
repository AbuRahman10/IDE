#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <iostream>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->RUN, &QPushButton::clicked, this, &MainWindow::inputtest);
    connect(ui->RUN, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
    this->setStyleSheet("background-color: #49006d;");
}

void MainWindow::onButtonClicked()
{
    ui->RUN->setChecked(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::inputtest()
{
    QString Qinput = ui->editor->toPlainText();
    string input = Qinput.toStdString();
    if (ui->RUN->isChecked())
    {
        string exit0 = "Process finished with exit code 0";
        string exit1 = "Process finished with exit code 1";
        ui->console->clear();

        QTextCursor cursor(ui->editor->document());
        cursor.movePosition(QTextCursor::End);
        if (input == "int")
        {
            ui->console->clear();
            ui->console->insertPlainText(QString::fromStdString(exit0));

            ui->editor->clear();
            cursor.insertText("int");

            // Dynamically modify the stylesheet to set the color to green
            QString styleSheet = ui->editor->styleSheet();
            styleSheet += " QTextEdit { color: green; }";
            ui->editor->setStyleSheet(styleSheet);
}
        else
        {
            ui->console->clear();
            ui->console->insertPlainText(QString::fromStdString(exit1));

            ui->editor->clear();
            cursor.insertText(Qinput);

            // Dynamically modify the stylesheet to set the color to green
            QString styleSheet = ui->editor->styleSheet();
            styleSheet += " QTextEdit { color: red; }";
            ui->editor->setStyleSheet(styleSheet);
        }
    }
}
