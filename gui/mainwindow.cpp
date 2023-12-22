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
#include "message.h"
#include <QTimer>
#include <QThread>
#include <QMessageBox>
#include "QDebug"
#include "QKeyEvent"
#include <QScrollBar>
#include <QChar>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::settings);
    connect(ui->actionSave_Project, &QAction::triggered, this, &MainWindow::save);
    connect(ui->actionOpen_Project, &QAction::triggered, this, &MainWindow::open);
    connect(ui->RUN, &QPushButton::clicked, this, &MainWindow::consoletest);
    connect(ui->RUN_2, &QPushButton::clicked, this, &MainWindow::consoletest);
    connect(ui->clearConsole, &QPushButton::clicked, [=]() {clearALL(ui->clearConsole);});
    connect(ui->clearInput, &QPushButton::clicked, [=]() {clearALL(ui->clearInput);});
    connect(ui->RUN, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
    connect(ui->RUN_2, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
    cfg = CFG("grammar.json");
    parser = cfg.createTable();
    ///////// LIVE EDITOR /////////
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(simulateRunButtonClick()));
    timer->start(10);
    ///////// PROPERTIES /////////
    this->setWindowTitle("IDE");
    setWindowIcon(QIcon("logo.png"));
    this->setStyleSheet("background-color: #49006d;");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::simulateRunButtonClick()
{
    //////// SAVING CURSOR LOCATION ////////
    int originalCursor;
    originalCursor = ui->editor->textCursor().position();
    //////// INPUT CHECK ////////
    inputtest();
    //////// LINE NUMBERS ////////
    if (keyEnterBackspace())
    {
        originalCursor--;
    }
    /////// SET ORIGINAL CURSOR LOCATION ///////
    QTextCursor cursor = ui->editor->textCursor();
    cursor.setPosition(originalCursor);
    ui->editor->setTextCursor(cursor);
}

void MainWindow::onButtonClicked()
{
    ui->RUN->setChecked(false);
    ui->RUN_2->setChecked(false);
}

void MainWindow::inputtest()
{
    ///////// LEXER /////////
    string inp = ui->editor->toPlainText().toStdString();
    Lexer lexer(inp);
    auto get_tokens = lexer.tokenize();
    ///////// CFG AND PARSER /////////
    bool accept = cfg.parse(get_tokens,parser);
    ///////// ACCEPTER /////////
    if (accept)  // juiste syntax
    {
        ///////// FRAME /////////
        if (inp == "")
        {
            ui->frame->setStyleSheet("background-color: #49006d;");
        }
        else
        {
            ui->frame->setStyleSheet("background-color: green;");
        }
        ///////// COLOR /////////
        bool stringPassed = false;
        bool charPassed = false;
        bool equalPassed = false;
        QString type;
        QString text = QString::fromStdString(inp);
        QTextCursor cursor = ui->editor->textCursor();
        QTextCharFormat format;
        ui->editor->clear();
        for (int charIndex = 0; charIndex < text.length(); ++charIndex)
        {
            const QChar character = text.at(charIndex);
            type += character;
            if (inp[0] == '/' and inp[1] == '/')
            {
                format.setForeground(Qt::gray);
                cursor.insertText(character, format);
                type.clear();
            }
            else if (charIndex > 1 and (character == '\\' or text.at(charIndex-1) == '\\'))
            {
                format.setForeground(Qt::green);
                cursor.insertText(character, format);
            }
            else if (escapeCharacters.contains(character))
            {
                cursor.insertText(character);
                type.clear();
            }
            else if (typesNames.contains(type))
            {
                int datatypeSize = type.size() - 1;
                int positionsToGoBack = charIndex - datatypeSize;
                cursor.setPosition(charIndex);
                cursor.setPosition(positionsToGoBack, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
                ui->editor->setTextCursor(cursor);
                format.setForeground(QColor("#FF46D2"));
                cursor.insertText(type, format);
                type.clear();
                equalPassed = false;
            }
            else if (character == ' ' and (!stringPassed and !charPassed))
            {
                cursor.insertText(" ", format);
                type.clear();
            }
            else if (character == '=' and (!stringPassed and !charPassed))
            {
                format.setForeground(Qt::green);
                cursor.insertText("=", format);
                equalPassed = true;
            }
            else if (character == ';' and (!stringPassed and !charPassed))
            {
                format.setForeground(QColor("#00FFF3"));
                cursor.insertText(";", format);
                type.clear();
                equalPassed = false;
            }
            else if (character == '\"' and !charPassed)
            {
                format.setForeground(QColor("#FF6100"));
                cursor.insertText(character, format);
                if (!stringPassed)
                {
                    stringPassed = true;
                }
                else
                {
                    stringPassed = false;
                }
            }
            else if (character == '\'' and !stringPassed)
            {
                format.setForeground(QColor("#FF6100"));
                cursor.insertText(character, format);
                if (!charPassed)
                {
                    charPassed = true;
                }
                else
                {
                    charPassed = false;
                }
            }
            else if ((character == '<' or character == '>') and (!stringPassed and !charPassed))
            {
                format.setForeground(Qt::red);
                cursor.insertText(character, format);
                type.clear();
            }
            else if ((character == '(' or character == ')') and (!stringPassed and !charPassed))
            {
                format.setForeground(Qt::white);
                cursor.insertText(character, format);
                type.clear();
            }
            else if ((character == '{' or character == '}') and (!stringPassed and !charPassed))
            {
                format.setForeground(QColor("#0051FF"));
                cursor.insertText(character, format);
                type.clear();
            }
            else
            {
                if (stringPassed or charPassed)
                {
                    format.setForeground(QColor("#FBFF00"));
                    cursor.insertText(character, format);
                }
                else if (character.isDigit() and equalPassed and !stringPassed)
                {
                    format.setForeground(QColor("#A951FB"));
                    cursor.insertText(character, format);
                }
                else
                {
                    format.setForeground(QColor("#d2a523"));
                    cursor.insertText(character, format);
                }
            }
        }
    }
    else // foute syntax
    {
        ///////// FRAME /////////
        if (inp == "")
        {
            ui->frame->setStyleSheet("background-color: #49006d;");
        }
        else
        {
            ui->frame->setStyleSheet("background-color: red;");
        }
        ///////// EDITOR /////////
        QTextDocument* document = new QTextDocument(this);
        QTextCursor cursor(document);
        cursor.insertText(ui->editor->toPlainText());
        // wavy underline
        QTextCharFormat format;
        format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
        format.setUnderlineColor(Qt::red);
        cursor.setPosition(0); // Move cursor to the beginning
        cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor); // Select the entire text
        cursor.mergeCharFormat(format); // Apply formatting to the selected text
        ui->editor->setDocument(document);
        // changing font
        QString styleSheet = "QTextEdit {font-family: 'Fira Code', monospace;font-size: 17px; line-height: 1.5; background-color: #282a36; color: #ba8602; }";
        ui->editor->setStyleSheet(styleSheet);
    }
}

void MainWindow::consoletest()
{
    // EXIT CODES
    QString exit0 = "Process finished with exit code 0";
    QString exit1 = "Process finished with exit code 1";
    QString execution = "Executing your program...\n";
    ui->console->clear();
    ui->console->insertPlainText(execution);
    string inp = ui->editor->toPlainText().toStdString();
    ///////// LEXER /////////
    Lexer lexer(inp);
    auto get_tokens = lexer.tokenize();
    ///////// CFG AND PARSER /////////
    bool accept = cfg.parse(get_tokens,parser);
    ///////// ACCEPTER /////////
    if (accept)  // juiste syntax
    {
        ///////// CONSOLE /////////
        QString exit0Line = exit0;
        ui->console->insertPlainText(exit0Line);
    }
    else // foute syntax
    {
        ///////// CONSOLE /////////
        QString exit1Line = exit1;
        ui->console->insertPlainText(exit1Line);
    }
}

bool MainWindow::keyEnterBackspace()
{
    bool newLine = false;
    bool maxReached = false;
    QString qin = ui->editor->toPlainText();
    string in = qin.toStdString();
    counter = 1;
    string newInput;
    for (char ch : in)
    {
        if (ch == '\n')
        {
            counter++;
            newLine = true;
        }
        if (counter > 22)
        {
            maxReached = true;
            if (ch != '\n')
            {
                newInput += ch;
            }
        }
        else
        {
            newInput += ch;
        }
    }
    if (!newLine)
    {
        ui->editor_11->clear();
        ui->editor_11->insertPlainText(QString::fromStdString("  "+ to_string(1)));
    }
    if (newLine)
    {
        ui->editor_11->clear();
        ui->editor_11->insertPlainText(QString::fromStdString("  "+ to_string(1)));
        for (int i = 2; i <= counter; i++)
        {
            string lineNumber;
            if (i < 10 and i >= 0)
            {
                lineNumber = "\n  " + to_string(i);
            }
            else
            {
                lineNumber = "\n " + to_string(i);
            }
            ui->editor_11->insertPlainText(QString::fromStdString(lineNumber));
        }
    }
    if (maxReached)
    {
        ui->editor->clear();
        ui->editor->insertPlainText(QString::fromStdString(newInput));
    }
    return maxReached;
}

void MainWindow::clearALL(QPushButton *clearButton)
{
    if (clearButton->objectName().toStdString() == "clearInput")
    {
        ui->editor->clear();
        ui->frame->setStyleSheet("background-color: #49006d;");
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
    file << ui->editor->toPlainText().toStdString() << endl;
    file.close();
}

void MainWindow::open()
{
    ifstream file("SavedFile.txt");
    stringstream buffer;
    buffer << file.rdbuf();
    string fileContent = buffer.str();
    ui->editor->setPlainText(QString::fromStdString(fileContent));
    message("The most recent saved file has been opened!         ");
}
