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

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ///////// FRAME /////////
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
    timer->start(10);
    ///////// PROPERTIES /////////
    this->setWindowTitle("IDE");
    setWindowIcon(QIcon("logo.png"));
    this->setStyleSheet("background-color: #49006d;");
    //////// NO HORIZONTAL SCROLLBARS ////////
    for (QTextEdit* editor : editors)
    {
        editor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::simulateRunButtonClick()
{
    //////// SAVING CURSOR LOCATION ////////
    vector<int> originalCursors;
    for (QTextEdit *editor : editors)
    {
        originalCursors.push_back(editor->textCursor().position());
    }
    //////// INPUT CHECK ////////
    inputtest();
    //////// ONLY ONE LINE ////////
    for (int i = 0; i < editors.size(); ++i)
    {
        if (enterKey(editors[i], frames[i]))
        {
            originalCursors[i]--;
        }
        /////// SET ORIGINAL CURSOR LOCATION ///////
        QTextCursor cursor = editors[i]->textCursor();
        cursor.setPosition(originalCursors[i]);
        editors[i]->setTextCursor(cursor);
    }
}

void MainWindow::onButtonClicked()
{
    ui->RUN->setChecked(false);
}

void MainWindow::inputtest()
{
    //////// INPUT AND CURSOR LOCATION ////////
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
            bool isStrChar = false;
            bool equalPassed = false;
            string type;
            QString text = QString::fromStdString(inp);
            QTextCursor cursor = editors[i]->textCursor();
            QTextCharFormat format;
            editors[i]->clear();
            for (int charIndex = 0; charIndex < text.length(); ++charIndex)
            {
                type += inp[charIndex];
                QString Qdatatype = QString::fromStdString(type);
                const QChar character = text.at(charIndex);
                if (inp[0] == '/' and inp[1] == '/')
                {
                    format.setForeground(Qt::gray);
                    cursor.insertText(character, format);
                    type.clear();
                }
                else if (typesNames.contains(QString::fromStdString(type)))
                {
                    int datatypeSize = type.size() - 1;
                    int positionsToGoBack = charIndex - datatypeSize;
                    cursor.setPosition(charIndex);
                    cursor.setPosition(positionsToGoBack, QTextCursor::KeepAnchor);
                    cursor.removeSelectedText();
                    editors[i]->setTextCursor(cursor);
                    format.setForeground(QColor("#FF46D2"));
                    cursor.insertText(Qdatatype, format);
                    type.clear();
                    equalPassed = false;
                }
                else if (character == ' ')
                {
                    cursor.insertText(" ", format);
                    if (!isStrChar)
                    {
                        type.clear();
                    }
                }
                else if (character == '=')
                {
                    format.setForeground(Qt::green);
                    cursor.insertText("=", format);
                    equalPassed = true;
                }
                else if (character == ';')
                {
                    format.setForeground(QColor("#00FFF3"));
                    cursor.insertText(";", format);
                    type.clear();
                    equalPassed = false;
                }
                else if (character == '\"' or character == '\'')
                {
                    format.setForeground(QColor("#FF6100"));
                    cursor.insertText(character, format);
                    if (!isStrChar)
                    {
                        isStrChar = true;
                    }
                    else
                    {
                        isStrChar = false;
                    }
                }
                else if (character == '<' or character == '>')
                {
                    format.setForeground(Qt::red);
                    cursor.insertText(character, format);
                    type.clear();
                }
                else if (character == '(' or character == ')')
                {
                    format.setForeground(Qt::white);
                    cursor.insertText(character, format);
                    type.clear();
                }
                else if (character == '{' or character == '}')
                {
                    format.setForeground(QColor("#0051FF"));
                    cursor.insertText(character, format);
                    type.clear();
                }
                else
                {
                    if (isStrChar)
                    {
                        format.setForeground(QColor("#FBFF00"));
                        cursor.insertText(character, format);
                    }
                    else if (isdigit(inp[charIndex]) and equalPassed and !isStrChar)
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
    }
}

void MainWindow::consoletest()
{
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

bool MainWindow::enterKey(QTextEdit *editor, QFrame *frame)
{
    bool newLine = false;
    QString qin = editor->toPlainText();
    string in = qin.toStdString();
    string newInput;
    for (char ch : in)
    {
        if (ch == '\n')
        {
            newLine = true;
        }
        else
        {
            newInput += ch;
        }
    }
    if (newLine)
    {
        editor->clear();
        frame->setStyleSheet("background-color: green;");
        editor->insertPlainText(QString::fromStdString(newInput));
    }
    return newLine;
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
