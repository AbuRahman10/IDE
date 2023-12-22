//
// Created by riwi on 27/11/23.
//

#ifndef IDE_LEXER_H
#define IDE_LEXER_H
#include <iostream>
#include <sstream>
#include <memory>
#include <fstream>
#include <utility>
#include <vector>
#include <set>
#include "string"
#include "stack"

struct Token {
    enum Type {DATATYPE, DECLARATION_NAME, VALUE, OPERATOR, PUNCTUATION, END_OF_FILE, ERROR,BRACKETS,PARENTHESIS,NEWLINE,SQUARE_BRACKETS,KEYWORD};
    std::string typeToString() const{
        switch (this->type) {
            case DATATYPE:
                return "D";
            case DECLARATION_NAME:
                return "N";
            case VALUE:
                return "V";
            case OPERATOR:
                return "O";
            case PUNCTUATION:
                return "P";
            case END_OF_FILE:
                return "E";
            case ERROR:
                return "X";
            case KEYWORD:
                return "K";
            default:
                return "false";
        }
    }
    Type type;
    std::string word;
    size_t pos;
    int line,column;
    // normal constructor
    Token(Type type, std::string  lexeme): type(type), word(std::move(lexeme)) {}
    // line / column constructor
    Token(Type type, std::string word, int line, int column) : type(type), word(std::move(word)), line(line),column(column) {}

    Token(Type type, std::string word,int line, int column,  size_t pos) : type(type), word(std::move(word)), pos(pos),
                                                                                  line(line), column(column) {}
};

class Lexer {
private:
    std::string code;
    size_t char_pointer;
    int line = 0;
    int column = 0;
    [[nodiscard]] Token lexNumber();
    [[nodiscard]] Token lexString();
    [[nodiscard]] Token lexCharacter();
    [[nodiscard]] Token lexOperatorOrPunctuation();
    [[nodiscard]] Token lexIdentifierOrKeyword();
    inline bool isKeyword(const std::string& str) const;
    inline void skipSingleLineComment();
    inline void skipMultiLineComment();
    std::vector<std::vector<Token>> statements;
    std::stack<char> parenthesis;
    bool else_if_exits = false;
    bool if_exits = false;

public:
    Lexer(std::string  source) : code(std::move(source)), char_pointer(0) {}

    Lexer() = default;

    std::vector<int> pos_dollar;

    [[nodiscard]] std::vector<std::vector<Token>>tokenize() noexcept;
    void print();

};


#endif //IDE_LEXER_H
