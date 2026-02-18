#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <memory>
#include <iostream>

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
    virtual std::string getNodeType() const = 0;
};

class NumberNode : public ASTNode {
public:
    double value;
    NumberNode(double val) : value(val) {}
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Number: " << value << "\n";
    }
    std::string getNodeType() const override { return "Number"; }
};

class IdentifierNode : public ASTNode {
public:
    std::string name;
    IdentifierNode(const std::string& n) : name(n) {}
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Identifier: " << name << "\n";
    }
    std::string getNodeType() const override { return "Identifier"; }
};

class BinaryOpNode : public ASTNode {
public:
    std::string op;
    std::unique_ptr<ASTNode> left, right;
    
    BinaryOpNode(const std::string& operation, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : op(operation), left(std::move(l)), right(std::move(r)) {}
    
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "BinaryOp: " << op << "\n";
        left->print(indent + 2);
        right->print(indent + 2);
    }
    std::string getNodeType() const override { return "BinaryOp"; }
};

class AssignmentNode : public ASTNode {
public:
    std::string variable;
    std::unique_ptr<ASTNode> expression;
    
    AssignmentNode(const std::string& var, std::unique_ptr<ASTNode> expr)
        : variable(var), expression(std::move(expr)) {}
    
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Assignment: " << variable << "\n";
        expression->print(indent + 2);
    }
    std::string getNodeType() const override { return "Assignment"; }
};

class PrintNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression;
    PrintNode(std::unique_ptr<ASTNode> expr) : expression(std::move(expr)) {}
    
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Print:\n";
        expression->print(indent + 2);
    }
    std::string getNodeType() const override { return "Print"; }
};

class Parser {
private:
    std::vector<Token> tokens;
    size_t position;
    
    Token current() const {
        return (position >= tokens.size()) ? tokens.back() : tokens[position];
    }
    
    Token peek(int offset = 1) const {
        return (position + offset >= tokens.size()) ? tokens.back() : tokens[position + offset];
    }
    
    void advance() {
        if (position < tokens.size() - 1) position++;
    }
    
    bool match(TokenType type) {
        if (current().type == type) { advance(); return true; }
        return false;
    }
    
    void expect(TokenType type, const std::string& message) {
        if (current().type != type) {
            throw std::runtime_error("Error sintaxis: " + message + 
                                   " linea " + std::to_string(current().line));
        }
        advance();
    }
    
    std::unique_ptr<ASTNode> factor() {
        Token tok = current();
        if (tok.type == TokenType::NUMBER) {
            advance();
            return std::make_unique<NumberNode>(std::stod(tok.lexeme));
        }
        if (tok.type == TokenType::IDENTIFIER) {
            advance();
            return std::make_unique<IdentifierNode>(tok.lexeme);
        }
        if (match(TokenType::LPAREN)) {
            auto expr = expression();
            expect(TokenType::RPAREN, "esperaba ')'");
            return expr;
        }
        throw std::runtime_error("Error sintaxis linea " + std::to_string(tok.line));
    }
    
    std::unique_ptr<ASTNode> term() {
        auto left = factor();
        while (current().type == TokenType::MULTIPLY || current().type == TokenType::DIVIDE) {
            Token op = current();
            advance();
            auto right = factor();
            left = std::make_unique<BinaryOpNode>(op.lexeme, std::move(left), std::move(right));
        }
        return left;
    }
    
    std::unique_ptr<ASTNode> expression() {
        auto left = term();
        while (current().type == TokenType::PLUS || current().type == TokenType::MINUS) {
            Token op = current();
            advance();
            auto right = term();
            left = std::make_unique<BinaryOpNode>(op.lexeme, std::move(left), std::move(right));
        }
        return left;
    }
    
    std::unique_ptr<ASTNode> statement() {
        if (current().type == TokenType::PRINT) {
            advance();
            expect(TokenType::LPAREN, "esperaba '(' despues de print");
            auto expr = expression();
            expect(TokenType::RPAREN, "esperaba ')'");
            expect(TokenType::SEMICOLON, "esperaba ';'");
            return std::make_unique<PrintNode>(std::move(expr));
        }
        if (current().type == TokenType::IDENTIFIER && peek().type == TokenType::ASSIGN) {
            std::string varName = current().lexeme;
            advance();
            advance();
            auto expr = expression();
            expect(TokenType::SEMICOLON, "esperaba ';'");
            return std::make_unique<AssignmentNode>(varName, std::move(expr));
        }
        throw std::runtime_error("Error sintaxis linea " + std::to_string(current().line));
    }
    
public:
    Parser(const std::vector<Token>& toks) : tokens(toks), position(0) {}
    
    std::vector<std::unique_ptr<ASTNode>> parse() {
        std::vector<std::unique_ptr<ASTNode>> statements;
        while (current().type != TokenType::END_OF_FILE) {
            statements.push_back(statement());
        }
        return statements;
    }
};

#endif