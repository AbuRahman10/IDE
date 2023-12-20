//
// Created by riwi on 27/11/23.
//

#include "Lexer.h"
Token Lexer::lexNumber() {
    std::string number;
    bool hasDecimalPoint = false;
    // identify if it start with number then char cause then it's a declaration_name
    int startCharPointer = char_pointer;

    // Handle negative numbers
    if (code[char_pointer] == '-' && isdigit(code[char_pointer + 1])) {
        number += code[char_pointer++]; // Add '-' to number
    }

    // Identify the decimal point
    while (isdigit(code[char_pointer]) || (!hasDecimalPoint && code[char_pointer] == '.')) {
        if (code[char_pointer] == '.') {
            hasDecimalPoint = true; // Decimal point in the number
        }
        number += code[char_pointer++];
    }

    // Handling the exponent part (e.g., 2.3e-4)
    if (tolower(code[char_pointer]) == 'e') {
        number += code[char_pointer++]; // Add 'e' or 'E'
        if (code[char_pointer] == '+' || code[char_pointer] == '-') {
            number += code[char_pointer++]; // Add sign of exponent if present
        }
        while (isdigit(code[char_pointer])) {
            number += code[char_pointer++];
        }
    }

    // Check if the number is immediately followed by an alphanumeric character or underscore cause then this will be a declaration_name
    if (isalnum(code[char_pointer]) || code[char_pointer] == '_') {
        std::string declarationName = number;
        // get the whole word out
        while (isalnum(code[char_pointer]) || code[char_pointer] == '_') {
            declarationName += code[char_pointer++];
        }
        return {Token::DECLARATION_NAME, declarationName, this->line, this->column};
        }
    return {Token::VALUE,number,this->line,this->column};
}

Token Lexer::lexString() {
    std::string str;
    str += code[char_pointer++]; // Add opening quote
    while (char_pointer < code.length() && code[char_pointer] != '\"') {
        if (code[char_pointer] == '\\') { // Check for escape character
            char_pointer++; // Move to the character after the backslash
            if (char_pointer < code.length()) {
                // Handle common escape sequences
                switch (code[char_pointer]) {
                    // carriage return
                    case 'r': str += '\r'; break;
                    // audible bell
                    case 'a': str += '\a'; break;
                    //form feed
                    case 'f': str += '\f'; break;
                    //vertical tab
                    case 'v': str += '\v'; break;
                    // backspace
                    case 'b': str += '\b'; break;
                    // null character
                    case '0': str += '\0'; break;
                    // newline
                    case 'n': str += '\n'; break;
                        // tab
                    case 't': str += '\t'; break;
                        // backslashes
                    case '\\': str += '\\'; break;
                        // quotes
                    case '\"': str += '\"'; break;
                    default: str += code[char_pointer]; // niet gewenste chars as escape sequence
                }
            } else {
                return {Token::ERROR, "Incomplete escape sequence",this->line,this->column};
            }
        } else {
            str += code[char_pointer];
        }
        char_pointer++;
    }

    if (char_pointer < code.length() && code[char_pointer] == '\"') {
        str += code[char_pointer++];
        return {Token::VALUE, str,this->line,this->column};
    } else {
        return {Token::ERROR, "Unclosed string literal",this->line,this->column};
    }
}

Token Lexer::lexCharacter() {
    std::string character;
    character += code[char_pointer++]; // Add opening quote
    while (char_pointer < code.length() && code[char_pointer] != '\'') {
        character += code[char_pointer++];
    }
    character += code[char_pointer++]; // Add closing quote

    if (character[character.length()-1] != '\''){
        return {Token::ERROR,"Unclosed char!",this->line,this->column};
    }
    return {Token::VALUE, character,this->line,this->column};
}

Token Lexer::lexOperatorOrPunctuation() {
    char current = code[char_pointer];
    std::string tokenValue(1, current);

    // Check for minus sign before a number (this is for signed integers!!!!)
    if (current == '-' && isdigit(code[char_pointer + 1])) {
        char_pointer++;
        return lexNumber();
    }

    char_pointer++; // Increment char_pointer after the check

    // Check for multi-character operators
    if (char_pointer < code.length()) {
        char next = code[char_pointer];
        // Check for multi-character operators (like <=, >=, etc.)
        if ((current == '<' && next == '=') || (current == '>' && next == '=') ||
            (current == '&' && next == '&') || (current == '|' && next == '|') ||
            (current == '<' && next == '<') || (current == '>' && next == '>') ||
            (current == '+' || current == '-' || current == '*' || current == '/' && next == '=')) {
            tokenValue += next;
            char_pointer++; // Consume the next character as well
        }
    }

    // Determine the token type
    if (tokenValue == "<=" || tokenValue == ">=" || tokenValue == "&&" || tokenValue == "||" ||
        tokenValue == "<<" || tokenValue == ">>" || tokenValue == "+=" || tokenValue == "-=" ||
        tokenValue == "*=" || tokenValue == "/=") {
        return {Token::OPERATOR, tokenValue, this->line, this->column};
    } else if (tokenValue == ";" || tokenValue == "," || tokenValue == ".") {
        return {Token::PUNCTUATION, tokenValue, this->line, this->column};
    } else if (tokenValue == "(" || tokenValue == ")") {
        return {Token::BRACKETS, tokenValue, this->line, this->column};
    } else if (tokenValue == "{" || tokenValue == "}") {
        return {Token::PARENTHESIS, tokenValue, this->line, this->column};
    }else if(tokenValue == "[" || tokenValue == "]"){
        return {Token::SQUARE_BRACKETS,tokenValue,this->line,this->column};
    }
    else {
        return {Token::OPERATOR, tokenValue, this->line, this->column}; // Single char operator
    }
}

bool Lexer::isKeyword(const std::string& str)const {
    const std::set<std::string> keywords = {"string","bool", "int", "float", "double", "char", "vector", "set"};
    return keywords.find(str) != keywords.end();
}

void Lexer::skipSingleLineComment() {
    char_pointer += 2; // Skip the "//" characters
    // Move the cursor forward until the end of the line or the end of the file
    while (char_pointer < code.length() && code[char_pointer] != '\n') {
        char_pointer++;
    }
}

void Lexer::skipMultiLineComment() {
    char_pointer += 2; // Skip the "/*"
    // Move the cursor forward until the end of the comment or the end of the file
    while (char_pointer + 1 < code.length()) {
        if (code[char_pointer] == '*' && code[char_pointer + 1] == '/') {
            char_pointer += 2; // Skip the "*/"
            return;
        }
        char_pointer++;
    }
}

Token Lexer::lexIdentifierOrKeyword() {
    std::string identifier;
    while (isalnum(code[char_pointer]) || code[char_pointer] == '_') {
        identifier += code[char_pointer++];
    }
    // datatype
    if (isKeyword(identifier)) {
        return {Token::DATATYPE, identifier, this->line, this->column};
    }
        // values
    else if (identifier == "true" || identifier == "false") {
        return {Token::VALUE, identifier, this->line, this->column};
    }
    else if (identifier == "else") {
        // Save the current position
        int savedCharPointer = char_pointer;
        // tussenin kunnen er white spaces zijn dus skip
        while (isspace(code[char_pointer])) {
            char_pointer++;
        }
        // Check if 'if' comes after 'else'
        std::string getnextWord;
        while (isalnum(code[char_pointer])) {
            getnextWord += code[char_pointer++];
        }
        if (getnextWord == "if") {
            // 'else if' herkend door onze lexer
            identifier += " " + getnextWord;
            return {Token::KEYWORD, identifier, this->line, this->column};
        } else {
            // It was just 'else', reset the pointer
            char_pointer = savedCharPointer;
        }
    }
        // loops and control-statement
    else if (identifier == "while" || identifier == "if") {
        return {Token::KEYWORD, identifier, this->line, this->column};
    }
        // variable name
    else {
        return {Token::DECLARATION_NAME, identifier, this->line, this->column};
    }
}


std::vector<Token> Lexer::tokenize() noexcept {
    std::vector<Token> tokens;
    while (char_pointer < code.length()) {
        const char current = code[char_pointer];
        if (isspace(current)) {
            if (current == '\n') {
                tokens.emplace_back(Token::NEWLINE, "newline",this->line,this->column);
                this->line++;
                this->column = 0;
                char_pointer++;
                continue;
            }else{
                // it's a space
                char_pointer++;
                this->column++;
            }
        } else if (char_pointer + 1 < code.length() && code[char_pointer] == '/' && code[char_pointer + 1] == '/') {
            skipSingleLineComment();
            this->column++;
            // multiline comments
        } else if (char_pointer + 1 < code.length() && code[char_pointer] == '/' && code[char_pointer + 1] == '*') {
            skipMultiLineComment();
            this->column++;
        } else if (isdigit(current)) {
            tokens.push_back(lexNumber());
            this->column++;
        } else if (current == '\"') {
            tokens.push_back(lexString());
            this->column++;
        } else if (current == '\'') {
            tokens.push_back(lexCharacter());
            this->column++;
        } else if (isalpha(current) || current == '_') {
            tokens.push_back(lexIdentifierOrKeyword());
            this->column++;
        } else {
            tokens.push_back(lexOperatorOrPunctuation());
            this->column++;
        }
    }
    /*tokens.emplace_back(Token::END_OF_FILE, "<EOS>",this->line,this->column);
    // we have nothing in our file to compile even with all the comments it's going to skip everything! so ERROR !*/
    /*if (tokens.empty()){
        tokens.emplace_back(Token::ERROR,"empty file",this->line,this->column);
        return tokens;
    }*/
    return tokens;
}