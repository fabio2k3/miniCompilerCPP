#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.h"
#include <map>

class SymbolTable {
private:
    std::map<std::string, std::string> symbols;
    
public:
    void define(const std::string& name, const std::string& type) {
        symbols[name] = type;
    }
    
    bool isDefined(const std::string& name) const {
        return symbols.find(name) != symbols.end();
    }
    
    std::string getType(const std::string& name) const {
        auto it = symbols.find(name);
        if (it == symbols.end()) {
            throw std::runtime_error("Error semantico: variable '" + name + "' no definida");
        }
        return it->second;
    }
    
    void print() const {
        for (const auto& entry : symbols) {
            std::cout << "  " << entry.first << " : " << entry.second << "\n";
        }
    }
    
    const std::map<std::string, std::string>& getSymbols() const {
        return symbols;
    }
};

class SemanticAnalyzer {
private:
    SymbolTable symbolTable;
    
    std::string analyzeExpression(const ASTNode* node) {
        if (dynamic_cast<const NumberNode*>(node)) {
            return "number";
        }
        if (auto* idNode = dynamic_cast<const IdentifierNode*>(node)) {
            if (!symbolTable.isDefined(idNode->name)) {
                throw std::runtime_error("Error semantico: variable '" + 
                                       idNode->name + "' no definida");
            }
            return symbolTable.getType(idNode->name);
        }
        if (auto* binOp = dynamic_cast<const BinaryOpNode*>(node)) {
            std::string leftType = analyzeExpression(binOp->left.get());
            std::string rightType = analyzeExpression(binOp->right.get());
            if (leftType != "number" || rightType != "number") {
                throw std::runtime_error("Error semantico: operacion requiere numeros");
            }
            return "number";
        }
        throw std::runtime_error("Error semantico: nodo desconocido");
    }
    
    void analyzeStatement(const ASTNode* node) {
        if (auto* assignNode = dynamic_cast<const AssignmentNode*>(node)) {
            std::string exprType = analyzeExpression(assignNode->expression.get());
            symbolTable.define(assignNode->variable, exprType);
        }
        else if (auto* printNode = dynamic_cast<const PrintNode*>(node)) {
            analyzeExpression(printNode->expression.get());
        }
    }
    
public:
    void analyze(const std::vector<std::unique_ptr<ASTNode>>& statements) {
        for (const auto& stmt : statements) {
            analyzeStatement(stmt.get());
        }
    }
    
    void printSymbolTable() const {
        symbolTable.print();
    }
    
    const SymbolTable& getSymbolTable() const {
        return symbolTable;
    }
};

#endif