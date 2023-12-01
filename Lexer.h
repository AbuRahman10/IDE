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
struct Token {
    enum Type { DATATYPE, DECLARATION_NAME, VALUE, OPERATOR, PUNCTUATION, END_OF_FILE, ERROR,BRACKETS,PARENTHESIS,NEWLINE};
    Type type;
    std::string word;
    // normal constructor
    Token(Type type, std::string  lexeme): type(type), word(std::move(lexeme)) {}
    // constructor for keep tracking of line and column
};

class Lexer {
private:
    std::string code;
    size_t char_pointer;

    [[nodiscard]] Token lexNumber();
    [[nodiscard]] Token lexString();
    [[nodiscard]] Token lexCharacter();
    [[nodiscard]] Token lexOperatorOrPunctuation();
    [[nodiscard]] Token lexIdentifierOrKeyword();
    inline bool isKeyword(const std::string& str)const;
    inline void skipSingleLineComment();
    inline void skipMultiLineComment();
public:
    Lexer(std::string  source) : code(std::move(source)), char_pointer(0) {}

    [[nodiscard]] std::vector<Token>tokenize()noexcept;
};


#endif //IDE_LEXER_H
