#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <cctype>
#include <stdexcept>
#include <map>

enum class TokenType{
    // Literales
    NUMBER,
    IDENTIFIER,

    // Operadores aritmeticos
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    ASSIGN,

    // Delimitadores
    LPAREN,
    RPAREN,
    SEMICOLON,

    // Palabras Clave
    PRINT,

    // Especiales
    END_OF_FILE,
    UNKNOWN
};

struct Token
{
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    Token(TokenType t, const std::string& lex, int l = 1, int c = 1)
        : type(t), lexeme(lex), line(l), column(c){}

    std::string toString() const{
        static std::map<TokenType, std::string> typeNames = { 
            {TokenType::NUMBER, "NUMBER"},
            {TokenType::IDENTIFIER, "IDENTIFIER"},
            {TokenType::PLUS, "PLUS"},
            {TokenType::MINUS, "MINUS"},
            {TokenType::MULTIPLY, "MULTIPLY"},
            {TokenType::DIVIDE, "DIVIDE"},
            {TokenType::ASSIGN, "ASSIGN"},
            {TokenType::LPAREN, "LPAREN"},
            {TokenType::RPAREN, "RPAREN"},
            {TokenType::SEMICOLON, "SEMICOLON"},
            {TokenType::PRINT, "PRINT"},
            {TokenType::END_OF_FILE, "END_OF_FILE"},
            {TokenType::UNKNOWN, "UNKNOWN"}
            
        };

        return typeNames[type] + " '" + lexeme + "' [" + std::to_string(line) + ": " + std::to_string(column) + "]";
    }
};

class Lexer{
private:
    std::string source;
    size_t position;
    int line;
    int column;

    char current() const{
        if (position >= source.length()) return '\0';
        return source[position];
    }

    char peek(int offset = 1) const{
        if(position + offset >= source.length()) return '\0';
        return source[position + offset];
    }

    void advance() {
        if (position < source.length()){
            if(source[position] == '\n'){
                line++;
                column = 1;
            }
            else{
                column ++;
            }
            position++;
        }
    }

    void skipWhitespace(){
        while(std::isspace(current())){
            advance();
        }
    }

    Token number(){
        int startColumn = column;
        std::string num;

        while(std::isdigit(current()) || current() == '.'){
            num += current();
            advance();
        }

        return Token(TokenType::NUMBER, num, line, startColumn);
    }

    Token identifier(){
        int startColumn = column;
        std::string id;

        while(std::isalnum(current()) || current() == '_'){
            id += current();
            advance();
        }

        if (id == "print"){
            return Token(TokenType::PRINT, id, line, startColumn);
        }

        return Token(TokenType::IDENTIFIER, id, line, startColumn);
    }

public:
    Lexer(const std::string& src)
        : source(src), position(0), line(1), column(1) {}
    
        std::vector<Token> tokenize() {
            std::vector<Token> tokens;

            while(position < source.length()){
                skipWhitespace();

                if(position >= source.length()) break;

                char ch = current();
                int col = column;

                if(std::isdigit(ch)){
                    tokens.push_back(number());
                }
                else if(std::isalpha(ch) || ch == '_'){
                    tokens.push_back(identifier());
                }
                else if(ch == '+'){
                    tokens.push_back(Token(TokenType::PLUS, "+", line, col));
                    advance();
                }
                else if(ch == '-'){
                    tokens.push_back(Token(TokenType::MINUS, "-", line, col));
                    advance();
                }
                else if(ch == '*'){
                    tokens.push_back(Token(TokenType::MULTIPLY, "*", line, col));
                    advance();
                }
                else if(ch == '/'){
                    tokens.push_back(Token(TokenType::DIVIDE, "/", line, col));
                    advance();
                }
                else if(ch == '='){
                    tokens.push_back(Token(TokenType::ASSIGN, "=", line, col));
                    advance();
                }
                else if(ch == '('){
                    tokens.push_back(Token(TokenType::LPAREN, "(", line, col));
                    advance();
                }
                else if(ch == ')'){
                    tokens.push_back(Token(TokenType::RPAREN, ")", line, col));
                    advance();
                }
                else if(ch == ';'){
                    tokens.push_back(Token(TokenType::SEMICOLON, ";", line, col));
                    advance();
                }
                else{
                    throw std::runtime_error("Error léxico: carácter no reconocido '"+ 
                                            std::string(1,ch) + "' en línea " + 
                                            std::to_string(line) + ":" + std::to_string(col)); 
                }
            }

            tokens.push_back(Token(TokenType::END_OF_FILE, "", line, column));
            return tokens;
        }
};


#endif