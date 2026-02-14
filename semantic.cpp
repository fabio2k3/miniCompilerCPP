#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.cpp"
#include <map>
#include <iostream>

class SymbolTable{
private:
	std::map<std::string, std::string> symbols;

public:
	void define(const std::string& name, const std::string& type){
		symbols[name] = type;
	}

	bool isDefined(const std::string& name) const{
		return symbols.find(name) != symbols.end();
	}

	std::string getType(const std::string& name) const{
		auto it = symbols.find(name);
		if(it == symbols.end()){
			throw std::runtime_error("Error semántico: variable '" + name + "' no definida");
		}
		return it -> second;
	}

	void print() const{
		for(const auto& entry : symbols){
			std::cout << "  " << entry.first << " : " << entry.second << "\n";
		}
	}

	const std::map<std::string, std::string>& getSymbols() const {
		return symbols;
	}
};

class SemanticAnalyzer{
private: 
	SymbolTable symbolTable;

	std::string analyzeExpression(const ASTNode* node){
		if(dynamic_cast<const NumverNode*>(node))
			return "number";

		if (auto* idNode = dynamic_cast<const IdentifierNode*>(node)){
			if(!symbolTable.isDefined(idNode -> name))
				throw std::runtime_error("Error semantico variable '" + idNode ->name + "' no ha sido definida");
			return symbolTable.getType(idNode -> name);
		}

		if(auto* binOp = dynamic_cast<constBinaryOpNode*>(node)){
			std::string leftType = analyzeExpression(binOp-> left.get());
			std::string rightType = analyzeExpression(binOp-> right.get());

			if(leftType != "number" || rightType != "number")
				throw std::runtime_error("Error semántico: operación '" + binOp ->op + "' requiere operandos numéricos");

			return "number";
		}
		throw std::runtime_error("Error semántico: nodo de expresión desconocido");
	}

	void analyzeStatement(const ASTNode* node){
		if(auto* assigNode = dynamic_cast<const AssigmentNode*>(node)){
			std::string exprType = analyzeExpression(assigNode -> expression.get());
			SymbolTable.define(assigNode -> variable, exprType);
		}
		else if(auto* printNode = dynamic_cast<const PrintNode*>(node))
			analyzeExpression(printNode -> expression.get());
	}
public:
	void analyze(const std::vector<std::unique_ptr<ASTNode>>& statements){
		for(const auto& stmt : statements)
			analyzeStatement(stmt.get());
	}
	void printSymbolTable() const{
		symbolTable.print();
	}

	const SymbolTable& getSymbolTable() const{
		return symbolTable
	}
};
#endif