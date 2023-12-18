#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QIcon>
#include <QPainter>
#include <QRect>
#include <QLabel>
#include <iostream>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTextCursor>
#include "../src/Lexer.h"
#include "../src/CFG.h"
#include "message.h"
#include <QTimer>
#include <QThread>
#include <QMessageBox>
#include "QDebug"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ofstream file("debug.txt");
    file.clear();
    file << "MainWindow called" << endl;
    ///////// FRAMES /////////
    frames.push_back(ui->frame);
    frames.push_back(ui->frame_2);
    frames.push_back(ui->frame_3);
    frames.push_back(ui->frame_4);
    frames.push_back(ui->frame_5);
    frames.push_back(ui->frame_6);
    frames.push_back(ui->frame_7);
    frames.push_back(ui->frame_8);
    frames.push_back(ui->frame_9);
    frames.push_back(ui->frame_10);
    ///////// EDITORS /////////
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
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::settings);
    connect(ui->actionSave_Project, &QAction::triggered, this, &MainWindow::save);
    connect(ui->actionOpen_Project, &QAction::triggered, this, &MainWindow::open);
    connect(ui->RUN, &QPushButton::clicked, this, &MainWindow::consoletest);
    connect(ui->clearConsole, &QPushButton::clicked, [=]() {clearALL(ui->clearConsole);});
    connect(ui->clearInput, &QPushButton::clicked, [=]() {clearALL(ui->clearInput);});
    connect(ui->RUN, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
    cfg = CFG("grammar.json");
    parser = cfg.createTable();
    ///////// LIVE EDITOR /////////
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(simulateRunButtonClick()));
    timer->start(400);
    file << "Timer called" << endl;
    file.close();
    this->setWindowTitle("IDE");
    setWindowIcon(QIcon("logo.png"));
    this->setStyleSheet("background-color: #49006d;");
}

MainWindow::~MainWindow()
{
    ofstream file("debug.txt", std::ios::app);
    file << "~MainWindow called" << endl;
    file.close();
    delete ui;
}

void MainWindow::simulateRunButtonClick()
{
    ofstream file("debug.txt", std::ios::app);
    file << "simulateRunButtonClick called" << endl;
    file.close();
    // Save the current cursor position for all editors
    QVector<QTextCursor> originalCursors;
    for (QTextEdit* editor : editors)
    {
        originalCursors.append(editor->textCursor());
    }
    //////// INPUT CHECK ////////
    inputtest();
    //////// ONLY ONE LINE ////////
    for (int i = 0; i < editors.size(); ++i)
    {
        oneLine(editors[i]);
        // Set the cursor position to the end for each editor
        QTextCursor cursor = editors[i]->textCursor();
        cursor.movePosition(QTextCursor::End);
        editors[i]->setTextCursor(cursor);
    }
    // Restore the cursor position to the end for all editors
    for (int i = 0; i < editors.size(); ++i)
    {
        QTextCursor cursor = editors[i]->textCursor();
        cursor.setPosition(originalCursors[i].position());
        editors[i]->setTextCursor(cursor);
    }
}

void MainWindow::onButtonClicked()
{
    ofstream file("debug.txt", std::ios::app);
    file << "onButtonClicked called" << endl;
    file.close();
    ui->RUN->setChecked(false);
}

void MainWindow::inputtest()
{
    ofstream file("debug.txt", std::ios::app);
    file << "inputtest called" << to_string(counter++) << endl;
    file.close();
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
    for (int i = 0; i < 10; i++)
    {
        ///////// LEXER /////////
        Lexer lexer(inputs[i]);
        auto get_tokens = lexer.tokenize();
        ///////// CFG AND PARSER /////////
        bool accept = cfg.parse(get_tokens,parser);
        ///////// ACCEPTER /////////
        string inp = inputs[i];
        if (accept)  // juiste syntax
        {
            ///////// FRAME /////////
            if (inp == "")
            {
                frames[i]->setStyleSheet("background-color: #49006d;");
            }
            else
            {
                frames[i]->setStyleSheet("background-color: green;");
            }
            ///////// COLOR /////////
            QString text = QString::fromStdString(inp);
            QTextCursor cursor = editors[i]->textCursor();
            QTextCharFormat format;
            editors[i]->clear();
            string datatype;
            for (int charIndex = 0; charIndex < text.length(); ++charIndex)
            {
                datatype += inp[charIndex];
                QString Qdatatype = QString::fromStdString(datatype);
                const QChar character = text.at(charIndex);
                // Apply formatting based on conditions
                if (inp[0] == '/' and inp[1] == '/')
                {
                    format.setForeground(Qt::gray);
                    cursor.insertText(character, format);
                    datatype.clear();
                }
                else if (datatypes.contains(QString::fromStdString(datatype)))
                {
                    size_t position = inp.find(datatype);
                    for (int pos = 0; pos < datatype.size(); ++pos)
                    {
                        const QChar chr = Qdatatype.at(pos);
                        cursor.setPosition(position);
                        format.setForeground(QColor("#FF46D2"));
                        cursor.insertText(chr, format);
                        position++;
                    }
                    datatype.clear();
                }
                else if (character == '=')
                {
                    format.setForeground(Qt::green);
                    cursor.insertText("=", format);
                }
                else if (character == ';')
                {
                    format.setForeground(QColor("#00FFF3"));
                    cursor.insertText(";", format);
                }
                else if (character == '\"' or character == '\'')
                {
                    format.setForeground(QColor("#FFFFFF"));
                    cursor.insertText(character, format);
                }
                else if (character == '<' or character == '>')
                {
                    format.setForeground(Qt::red);
                    cursor.insertText(character, format);
                    datatype.clear();
                }
                else
                {
                    format.setForeground(QColor("#ba8602"));
                    cursor.insertText(character, format);
                }
            }
        }
        else // foute syntax
        {
            ///////// FRAME /////////
            if (inp == "")
            {
                frames[i]->setStyleSheet("background-color: #49006d;");
            }
            else
            {
                frames[i]->setStyleSheet("background-color: red;");
            }
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
        QTextCursor cursor = editors[i]->textCursor();
        QString text = cursor.document()->toPlainText();
        int semicolonIndex = text.indexOf(';');

        if (semicolonIndex != -1)
        {
            cursor.setPosition(semicolonIndex + 1);
            cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
        }
    }
}

void MainWindow::consoletest()
{
    ofstream file("debug.txt", std::ios::app);
    file << "consoletest called" << endl;
    file.close();
    // EXIT CODES
    QString exit0 = "Process finished with exit code 0";
    QString exit1 = "Process finished with exit code 1";
    QString execution = "Executing your program...\n";
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
    ui->console->insertPlainText(execution);
    for (int i = 0; i < 10; i++)
    {
        string inp = inputs[i];
        ///////// LEXER /////////
        Lexer lexer(inputs[i]);
        auto get_tokens = lexer.tokenize();
        ///////// CFG AND PARSER /////////
        bool accept = cfg.parse(get_tokens,parser);
        ///////// ACCEPTER /////////
        if (accept)  // juiste syntax
        {
            ///////// CONSOLE /////////
            QString exit0Line = exit0 + " for line: " + QString::fromStdString(to_string(i + 1)) + "\n";
            ui->console->insertPlainText(exit0Line);
        }
        else // foute syntax
        {
            ///////// CONSOLE /////////
            QString exit1Line = exit1 + " for line: " + QString::fromStdString(to_string(i + 1)) + "\n";
            ui->console->insertPlainText(exit1Line);
        }
    }
}

void MainWindow::oneLine(QTextEdit *editor)
{
    ofstream file("debug.txt", std::ios::app);
    file << "oneLine called" << endl;
    file.close();
    QString qin = editor->toPlainText();
    string in = qin.toStdString();
    string newInput;
    string msg = "Only 1 line is accepted as an input!         ";
    QString Qmsg = QString::fromStdString(msg);
    for (char ch : in)
    {
        if (ch == '\n')
        {
            message(Qmsg);
            editor->clear();
            editor->insertPlainText(QString::fromStdString(newInput));
            return;
        }
        newInput += ch;
    }
}

void MainWindow::clearALL(QPushButton *clearButton)
{
    if (clearButton->objectName().toStdString() == "clearInput")
    {
        for (QTextEdit *editor : editors)
        {
            editor->clear();
        }
        for (QFrame *frame : frames)
        {
            frame->setStyleSheet("background-color: #49006d;");
        }
    }
    else if (clearButton->objectName().toStdString() == "clearConsole")
    {
        ui->console->clear();
    }
}

void MainWindow::settings()
{
    QMessageBox c;
    c.setText("Choose Background Color:");
    QAbstractButton* white = reinterpret_cast<QAbstractButton *>(c.addButton("White",QMessageBox::ActionRole));
    QAbstractButton* defaultColor = reinterpret_cast<QAbstractButton *>(c.addButton("Default",QMessageBox::ActionRole));
    QAbstractButton* darkRed = reinterpret_cast<QAbstractButton *>(c.addButton("Dark Red",QMessageBox::ActionRole));
    QAbstractButton* darkTeal = reinterpret_cast<QAbstractButton *>(c.addButton("Dark Teal",QMessageBox::ActionRole));
    c.exec();
    if (c.clickedButton() == defaultColor)
    {
        this->setStyleSheet("background-color: #49006d;");
        ui->menubar->setStyleSheet("background-color: #49006d;");
    }
    else if (c.clickedButton() == white)
    {
        this->setStyleSheet("background-color: white;");
        ui->menubar->setStyleSheet("background-color: white;");
    }
    else if (c.clickedButton() == darkRed)
    {
        this->setStyleSheet("background-color: #4E0A2B;");
        ui->menubar->setStyleSheet("background-color: #4E0A2B;");
    }
    else if (c.clickedButton() == darkTeal)
    {
        this->setStyleSheet("background-color: #206267;");
        ui->menubar->setStyleSheet("background-color: #206267;");
    }
}

void MainWindow::save()
{
    ofstream file("SavedFile.txt");
    for (QTextEdit *editor : editors)
    {
        file << editor->toPlainText().toStdString() << endl;
    }
    file.close();
}

void MainWindow::open()
{
    ofstream file1("debug.txt", std::ios::app);
    file1 << "open called" << endl;
    file1.close();
    ifstream file("SavedFile.txt");
    if (file.is_open())
    {
        for (QTextEdit* editor : editors)
        {
            std::string line;
            std::getline(file, line);
            editor->setPlainText(QString::fromStdString(line));
        }
        file.close();
    }
    message("The most recent saved file has been opened!         ");
}
