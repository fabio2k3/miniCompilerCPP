#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.cpp"
#include <sstream>
#include <map>
#include <vector>
#include <cmath>


struct Instruction {
    std::string op;     
    std::string arg1;    
    std::string arg2;    
    std::string result;  
    
    std::string toString() const {
        if (op == "=") {
            return result + " = " + arg1;
        } else if (op == "print") {
            return "print " + arg1;
        } else {
            return result + " = " + arg1 + " " + op + " " + arg2;
        }
    }
};


class CodeGenerator {
private:
    std::vector<Instruction> instructions;
    int tempCounter;
    std::map<std::string, double> memory; 
    
    std::string newTemp() {
        return "t" + std::to_string(tempCounter++);
    }
    
    std::string generateExpression(const ASTNode* node) {
        if (auto* numNode = dynamic_cast<const NumberNode*>(node)) {
            return std::to_string(numNode->value);
        }

        if (auto* idNode = dynamic_cast<const IdentifierNode*>(node)) {
            return idNode->name;
        }
        
        if (auto* binOp = dynamic_cast<const BinaryOpNode*>(node)) {
            std::string left = generateExpression(binOp->left.get());
            std::string right = generateExpression(binOp->right.get());
            std::string temp = newTemp();
            
            instructions.push_back({binOp->op, left, right, temp});
            
            return temp;
        }
        
        return "";
    }
    
    void generateStatement(const ASTNode* node) {
        if (auto* assignNode = dynamic_cast<const AssigmentNode*>(node)) {
            std::string exprResult = generateExpression(assignNode->expression.get());
            instructions.push_back({"=", exprResult, "", assignNode->variable});
        }
        else if (auto* printNode = dynamic_cast<const PrintNode*>(node)) {
            std::string exprResult = generateExpression(printNode->expression.get());
            instructions.push_back({"print", exprResult, "", ""});
        }
    }
    
public:
    CodeGenerator() : tempCounter(0) {}
    
    std::string generate(const std::vector<std::unique_ptr<ASTNode>>& statements) {
        instructions.clear();
        tempCounter = 0;
        
        for (const auto& stmt : statements) {
            generateStatement(stmt.get());
        }
        
        std::stringstream ss;
        for (size_t i = 0; i < instructions.size(); i++) {
            ss << i + 1 << ": " << instructions[i].toString() << "\n";
        }
        
        return ss.str();
    }
    
    void execute() {
        memory.clear();
        
        for (const auto& inst : instructions) {
            if (inst.op == "=") {
                double value;
                if (memory.find(inst.arg1) != memory.end()) {
                    value = memory[inst.arg1];
                } else {
                    value = std::stod(inst.arg1);
                }
                memory[inst.result] = value;
            }
            else if (inst.op == "print") {
                double value;
                if (memory.find(inst.arg1) != memory.end()) {
                    value = memory[inst.arg1];
                } else {
                    value = std::stod(inst.arg1);
                }
                std::cout << value << "\n";
            }
            else {
                double left, right;
                
                if (memory.find(inst.arg1) != memory.end()) {
                    left = memory[inst.arg1];
                } else {
                    left = std::stod(inst.arg1);
                }
                
                if (memory.find(inst.arg2) != memory.end()) {
                    right = memory[inst.arg2];
                } else {
                    right = std::stod(inst.arg2);
                }
                
                double result = 0;
                if (inst.op == "+") result = left + right;
                else if (inst.op == "-") result = left - right;
                else if (inst.op == "*") result = left * right;
                else if (inst.op == "/") {
                    if (right == 0) {
                        throw std::runtime_error("Error de ejecución: división por cero");
                    }
                    result = left / right;
                }
                
                memory[inst.result] = result;
            }
        }
    }
};

#endif 