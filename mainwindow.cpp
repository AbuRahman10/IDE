#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QIcon>
#include <QPainter>
#include <QFontMetrics>
#include <QRect>
#include <QLabel>
#include <iostream>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTextCursor>
#include "Lexer.h"
#include "CFG.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->RUN, &QPushButton::clicked, this, &MainWindow::inputtest);
    connect(ui->RUN, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
    this->setWindowTitle("IDE");
    setWindowIcon(QIcon("C:/Users/aboba/Downloads/jetbrains-space-icon-logo-69559658A8-seeklogo.com.png"));
    this->setStyleSheet("background-color: #49006d;");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onButtonClicked()
{
    ui->RUN->setChecked(false);
}

void MainWindow::inputtest()
{
    ///////// EDITORS /////////
    vector<QTextEdit*> editors;
    editors.push_back(ui->editor);
    editors.push_back(ui->editor_2);
    editors.push_back(ui->editor_3);
    editors.push_back(ui->editor_4);
    editors.push_back(ui->editor_5);
    editors.push_back(ui->editor_6);
    editors.push_back(ui->editor_7);
    editors.push_back(ui->editor_8);
    editors.push_back(ui->editor_9);
    editors.push_back(ui->editor_10);
    // EXIT CODES
    QString exit0 = "Process finished with exit code 0";
    QString exit1 = "Process finished with exit code 1";
    // RUN
    if (ui->RUN->isChecked())
    {
        // INPUT
        vector<QString> Qinputs;
        vector<string> inputs;
        for (QTextEdit *editor : editors)
        {
            QString Qinput = editor->toPlainText();
            string input = Qinput.toStdString();
            Qinputs.push_back(Qinput);
            inputs.push_back(input);
        }
        ui->console->clear();
        for (int i = 0; i < 10; i++)
        {
            string inp = inputs[i];
            ///////// LEXER /////////
            Lexer lexer(inputs[i]);
            auto get_tokens = lexer.tokenize();
            ///////// CFG AND PARSER /////////
            CFG cfg;
            bool accept = cfg.parse(get_tokens);
            ///////// ACCEPTER /////////
            if (accept)  // juiste syntax
            {
                ///////// CONSOLE /////////
                QString exit0Line = exit0 + " for line: " + QString::fromStdString(to_string(i + 1)) + "\n";
                ui->console->insertPlainText(exit0Line);
                ///////// EDITOR /////////
                editors[i]->clear();
                QTextCursor cursor(editors[i]->document());
                cursor.movePosition(QTextCursor::End);
                cursor.insertText(Qinputs[i]);
                // Dynamically modify the stylesheet to set the color to green
                QString styleSheet = editors[i]->styleSheet();
                styleSheet += " QTextEdit { color: green; }";
                editors[i]->setStyleSheet(styleSheet);
            }
            else // foute syntax
            {
                ///////// CONSOLE /////////
                QString exit1Line = exit1 + " for line: " + QString::fromStdString(to_string(i + 1)) + "\n";
                ui->console->insertPlainText(exit1Line);
                ///////// EDITOR /////////
                QTextDocument* document = new QTextDocument(this);
                QTextCursor cursor(document);
                cursor.insertText(Qinputs[i]);
                // wavy underline
                QTextCharFormat format;
                format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
                format.setUnderlineColor(Qt::red);
                cursor.setPosition(0); // Move cursor to the beginning
                cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor); // Select the entire text
                cursor.mergeCharFormat(format); // Apply formatting to the selected text
                editors[i]->setDocument(document);
                // changing font
                QString styleSheet = "QTextEdit {font-family: 'Fira Code', monospace;font-size: 17px; line-height: 1.5; background-color: #282a36; color: #ba8602; }";
                editors[i]->setStyleSheet(styleSheet);
            }
        }
        ///////// LIVE EDITOR /////////
        for (QTextEdit *editor : editors)
        {
            connect(editor, &QTextEdit::textChanged, [=]() {textchanged(editor);});
        }
    }
}

void MainWindow::textchanged(QTextEdit *editor)
{
    /////// INPUT ////////
    QString Qinput = editor->toPlainText();
    string input = Qinput.toStdString();
    /////// DEFAULT FONT ////////
    QString styleSheet = "QTextEdit {font-family: 'Fira Code', monospace;font-size: 17px; line-height: 1.5; background-color: #282a36; color: #ba8602; }";
    editor->setStyleSheet(styleSheet);
    return;
}
