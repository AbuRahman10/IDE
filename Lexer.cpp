//
// Created by riwi on 27/11/23.
//

#include "Lexer.h"

Token Lexer::lexNumber() {
    std::string number;
    bool hasDecimalPoint = false;
    // Identify the decimal point
    while (isdigit(code[char_pointer]) ||(!hasDecimalPoint && code[char_pointer] == '.')) {
        if (code[char_pointer] == '.') {
            hasDecimalPoint = true; // We have a decimal point here in the numbers value
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
    return {Token::VALUE, number};
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
                return {Token::ERROR, "Incomplete escape sequence"};
            }
        } else {
            str += code[char_pointer];
        }
        char_pointer++;
    }

    if (char_pointer < code.length() && code[char_pointer] == '\"') {
        str += code[char_pointer++];
        return {Token::VALUE, str};
    } else {
        return {Token::ERROR, "Unclosed string literal"};
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
        return {Token::ERROR,"Unclosed char!"};
    }
    return {Token::VALUE, character};
}

Token Lexer::lexOperatorOrPunctuation() {
    char current = code[char_pointer++];
    std::string tokenValue(1, current);

    // Check for multi-character operators
    if (char_pointer < code.length()) {
        char next = code[char_pointer];
        if ((current == '<' && next == '=') ||(current == '>' && next == '=') ||(current == '&' && next == '&') ||(current == '|' && next == '|')||(current == '<' && next == '<')|| (current == '>'||next == '>')){
            tokenValue += next;
            char_pointer++;  // Consume the next character as well
        }
    }
    // Determine the token type
    if (tokenValue == "<=" || tokenValue == ">=" ||tokenValue == "&&" || tokenValue == "||") {
        return {Token::OPERATOR, tokenValue};
    } else if (tokenValue == ";" || tokenValue == "," || tokenValue == ".") {
        return {Token::PUNCTUATION, tokenValue};
    } else if (tokenValue == "(" || tokenValue == ")") {
        return {Token::BRACKETS, tokenValue};
    } else if (tokenValue == "{" || tokenValue == "}") {
        return {Token::PARENTHESIS, tokenValue};
    } else {
        return {Token::OPERATOR, tokenValue};  // Single char operator
    }
}

bool Lexer::isKeyword(const std::string& str)const {
    const std::set<std::string> keywords = {"string","bool", "int", "float", "double", "char", "void", "if", "else", "return","and","or","not"};
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
        return {Token::DATATYPE, identifier};
    }
        // aka variable naam
    else {
        return {Token::DECLARATION_NAME, identifier};
    }
}

std::vector<Token> Lexer::tokenize() noexcept {
    std::vector<Token> tokens;
    while (char_pointer < code.length()) {
        const char current = code[char_pointer];
        if (isspace(current)) {
            if (current == '\n') {
                tokens.emplace_back(Token::NEWLINE, "newline");
            }
            char_pointer++;
        } else if (char_pointer + 1 < code.length() && code[char_pointer] == '/' && code[char_pointer + 1] == '/') {
            skipSingleLineComment();
            // multiline comments
        } else if (char_pointer + 1 < code.length() && code[char_pointer] == '/' && code[char_pointer + 1] == '*') {
            skipMultiLineComment();
        } else if (isdigit(current)) {
            tokens.push_back(lexNumber());
        } else if (current == '\"') {
            tokens.push_back(lexString());
        } else if (current == '\'') {
            tokens.push_back(lexCharacter());
        } else if (isalpha(current) || current == '_') {
            tokens.push_back(lexIdentifierOrKeyword());
        } else {
            tokens.push_back(lexOperatorOrPunctuation());
        }
    }
    tokens.emplace_back(Token::END_OF_FILE, "<EOS>");
    if (tokens[0].type == Token::END_OF_FILE && tokens.size() == 1) {
        tokens.pop_back();
        tokens.emplace_back(Token::ERROR, "no main function present");
    }
    return tokens;
}


