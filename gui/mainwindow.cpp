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
#include <QTextBlock>
#include <QPropertyAnimation>

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
    timer->start(200);

    QTimer* lineTimer = new QTimer(this);
    connect(lineTimer, SIGNAL(timeout()), this, SLOT(updateLineNumber()));
    lineTimer->start(10);
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
    lexer = Lexer(inp);
    auto get_tokens = lexer.tokenize();
    ///////// CFG AND PARSER /////////
    cfg.setLex(lexer);
    vector<tuple<string,int,bool>> segments = cfg.parse(get_tokens,parser);
    bool errorFound = false;
    vector<int> errors;
    for(tuple<string,int,bool> &i : segments)
    {
        if(!get<2>(i))
        {
            errorFound = true;
            errors.push_back(get<1>(i)-2);
        }
    }
    ///////// ACCEPTER /////////
    ///////// FRAME /////////
    if (ui->editor->toPlainText() == "")
    {
        ui->frame->setStyleSheet("background-color: #49006d;");
        ui->label->setText("");
    }
    else if (errorFound)
    {
        ui->frame->setStyleSheet("background-color: red;");
        ui->label->setText(QString::fromStdString(to_string(errors.size())));
    }
    else
    {
        ui->frame->setStyleSheet("background-color: green;");
        ui->label->setText("");
    }
    ///////// COLOR /////////
    bool stringPassed = false;
    bool charPassed = false;
    bool equalPassed = false;
    bool singleLineComment = false;
    bool multiLineComment = false;
    QTextCursor cursor = ui->editor->textCursor();
    ui->editor->clear();
    QString type;
    QString commentBegin;
    QTextCharFormat format;
    QString text = QString::fromStdString(inp);
    for (int charIndex = 0; charIndex < text.length(); ++charIndex)
    {
        const QChar character = text.at(charIndex);
        QString Qkar = character;
        string kar = Qkar.toStdString();
        type += character;
        vector<int>::iterator it = std::find(errors.begin(), errors.end(), charIndex);
        bool found = (it != errors.end());
        if (found)
        {
            format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
            format.setUnderlineColor(Qt::red);
            cursor.insertText(character,format);
        }
        else if ((character == "/" or character == "*") and !singleLineComment and !multiLineComment)
        {
            commentBegin += character;
            if (commentBegin == "//")
            {
                format.setForeground(Qt::gray);
                cursor.setPosition(cursor.position()-1);
                cursor.setPosition(cursor.position()+1, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
                ui->editor->setTextCursor(cursor);
                cursor.insertText("//", format);
                singleLineComment = true;
                commentBegin.clear();
                type.clear();
            }
            else if (commentBegin == "/*")
            {
                format.setForeground(Qt::gray);
                cursor.setPosition(cursor.position()-1);
                cursor.setPosition(cursor.position()+1, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
                ui->editor->setTextCursor(cursor);
                cursor.insertText("/*", format);
                multiLineComment = true;
                commentBegin.clear();
                type.clear();
            }
            else if (commentBegin.size() > 1)
            {
                format.setForeground(QColor("#d2a523"));
                cursor.insertText(character, format);
                commentBegin.clear();
                type.clear();
            }
            else
            {
                format.setForeground(QColor("#d2a523"));
                cursor.insertText(character, format);
                type.clear();
            }
        }
        else if (singleLineComment or multiLineComment)
        {
            format.setForeground(Qt::gray);
            cursor.insertText(character, format);
            if ((character == '*' or character == '/'))
            {
                commentBegin += character;
                if (commentBegin == "*/")
                {
                    multiLineComment = false;
                    format.setForeground(QColor("#d2a523"));
                    commentBegin.clear();
                    type.clear();
                }
                else if (commentBegin.size() > 1)
                {
                    commentBegin.clear();
                    type.clear();
                }
            }
        }
        else if (charIndex > 1 and (character == '\\' or text.at(charIndex-1) == '\\'))
        {
            format.setForeground(Qt::green);
            cursor.insertText(character, format);
        }
        else if (escapeCharacters.contains(character) and !stringPassed)
        {
            cursor.insertText(character);
            type.clear();
        }
        else if (typesNames.contains(type))
        {
            int datatypeSize = type.size() - 1;
            int positionsToGoBack = charIndex - datatypeSize;
            /////////////////////////////////////////////////////////////////////////
            cursor.setPosition(charIndex);
            cursor.setPosition(positionsToGoBack, QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
            ui->editor->setTextCursor(cursor);
            /////////////////////////////////////////////////////////////////////////
            format.setForeground(QColor("#FF46D2"));
            cursor.insertText(type, format);
            format.setForeground(QColor("#d2a523"));
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
            format.setForeground(QColor("#d2a523"));
            equalPassed = true;
        }
        else if (character == ';' and (!stringPassed and !charPassed))
        {
            format.setForeground(QColor("#00FFF3"));
            cursor.insertText(";", format);
            format.setUnderlineStyle(QTextCharFormat::NoUnderline);
            format.setForeground(QColor("#d2a523"));
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
            format.setForeground(QColor("d2a523"));
            type.clear();
        }
        else if ((character == '(' or character == ')') and (!stringPassed and !charPassed))
        {
            format.setForeground(Qt::white);
            cursor.insertText(character, format);
            format.setForeground(QColor("#d2a523"));
            type.clear();
        }
        else if ((character == '{' or character == '}') and (!stringPassed and !charPassed))
        {
            format.setForeground(QColor("#0051FF"));
            cursor.insertText(character, format);
            format.setUnderlineStyle(QTextCharFormat::NoUnderline);
            format.setForeground(QColor("#d2a523"));
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
        if (character == '\n')
        {
            format.setForeground(QColor("#d2a523"));
            singleLineComment = false;
            type.clear();
        }
    }
}

void MainWindow::consoletest()
{
    // EXIT CODES
    QString exit0 = "\nProcess finished with exit code 0";
    QString exit1 = "\nProcess finished with exit code 1";
    QString execution = "Executing your program...\n";
    ui->console->clear();
    QTextCharFormat format;
    QTextCursor cursor = ui->console->textCursor();
    format.setForeground(Qt::lightGray);
    cursor.insertText(execution,format);
    string inp = ui->editor->toPlainText().toStdString();
    ///////// LEXER /////////
    Lexer lexer(inp);
    auto get_tokens = lexer.tokenize();
    ///////// CFG AND PARSER /////////
    vector<tuple<string,int,bool>> segments = cfg.parse(get_tokens,parser);
    bool errorFound = false;
    vector<int> errors;
    for(tuple<string,int,bool> &i : segments)
    {
        if(!get<2>(i))
        {
            errorFound = true;
            errors.push_back(get<1>(i)-2);
        }
    }
    ///////// ACCEPTER /////////
    QPropertyAnimation *animation = new QPropertyAnimation(ui->progressBar, "value");
    animation->setStartValue(0);
    animation->setEndValue(100);
    animation->setDuration(300);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start();
    if (errorFound)  // juiste syntax
    {
        ///////// CONSOLE /////////
        //for row and column
        int row = 0;
        int column = 0;
        for(int i = 0; i < inp.size(); i++)
        {
            auto it = ::find(errors.begin(),errors.end(),i);
            if(it != errors.end())
            {
                string errorLines = "Error found at row: " + to_string(row+1) + " and position: " + to_string(column) + "\n";
                QTextCharFormat format;
                QTextCursor cursor = ui->console->textCursor();
                format.setForeground(Qt::red);
                cursor.insertText(QString::fromStdString(errorLines),format);
            }
            if(inp[i] == '\n')
            {
                row++;
                column = 0;
            }
            else
            {
                column++;
            }
        }
        for (int i = 0; i < errors.size(); i++)
        {
            if (errors[i] >= inp.size())
            {
                string errorLines = "Error found at row: " + to_string(row+1) + " and position: " + to_string(column+1) + "\n";
                QTextCharFormat format;
                QTextCursor cursor = ui->console->textCursor();
                format.setForeground(Qt::red);
                cursor.insertText(QString::fromStdString(errorLines),format);
            }
        }
        QTextCharFormat format;
        QTextCursor cursor = ui->console->textCursor();
        format.setForeground(Qt::magenta);
        cursor.insertText(exit1,format);
    }
    else // foute syntax
    {
        ///////// CONSOLE /////////
        QTextCharFormat format;
        QTextCursor cursor = ui->console->textCursor();
        format.setForeground(Qt::green);
        cursor.insertText(exit0,format);
    }
}

bool MainWindow::keyEnterBackspace()
{
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
        QPropertyAnimation *animation = new QPropertyAnimation(ui->progressBar, "value");
        animation->setEndValue(0);
        animation->setDuration(300);
        animation->setEasingCurve(QEasingCurve::OutCubic);
        animation->start();
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

void MainWindow::updateLineNumber()
{
    bool newLine = false;
    QString qin = ui->editor->toPlainText();
    string in = qin.toStdString();
    counter = 1;
    for (char ch : in)
    {
        if (ch == '\n')
        {
            counter++;
            newLine = true;
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
}
