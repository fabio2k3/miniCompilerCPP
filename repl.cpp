#include <iostream>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"

class REPL {
private:
    SemanticAnalyzer semantic;
    CodeGenerator codegen;
    std::vector<std::unique_ptr<ASTNode>> allStatements;
    
    void printBanner() {
        std::cout << "\n╔════════════════════════════════════════╗\n";
        std::cout << "║   MINI COMPILADOR INTERACTIVO (REPL)   ║\n";
        std::cout << "╚════════════════════════════════════════╝\n\n";
        std::cout << "Comandos especiales:\n";
        std::cout << "  :help    - Ayuda\n";
        std::cout << "  :vars    - Ver variables\n";
        std::cout << "  :clear   - Limpiar variables\n";
        std::cout << "  :exit    - Salir\n\n";
        std::cout << "Ejemplos:\n";
        std::cout << "  x = 5 + 3;\n";
        std::cout << "  print(x);\n\n";
    }
    
    void printHelp() {
        std::cout << "\n=== SINTAXIS ===\n\n";
        std::cout << "Asignacion: variable = expresion;\n";
        std::cout << "Print:      print(expresion);\n";
        std::cout << "Operadores: + - * /\n";
        std::cout << "Ejemplos:\n";
        std::cout << "  x = 10;\n";
        std::cout << "  y = x * 2 + 5;\n";
        std::cout << "  print(y);\n\n";
    }
    
    void showVariables() {
        std::cout << "\n=== VARIABLES ===\n";
        const auto& symbols = semantic.getSymbolTable().getSymbols();
        if (symbols.empty()) {
            std::cout << "  (ninguna)\n";
        } else {
            for (const auto& entry : symbols) {
                std::cout << "  " << entry.first << " : " << entry.second << "\n";
            }
        }
        std::cout << "\n";
    }
    
    void clearVariables() {
        semantic = SemanticAnalyzer();
        codegen = CodeGenerator();
        allStatements.clear();
        std::cout << "\nVariables limpiadas\n\n";
    }
    
    bool processCommand(const std::string& input) {
        std::string cmd = input;
        size_t start = cmd.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return true;
        cmd = cmd.substr(start);
        cmd = cmd.substr(0, cmd.find_last_not_of(" \t\n\r") + 1);
        
        if (cmd.empty()) return true;
        if (cmd == ":help" || cmd == ":h") { printHelp(); return true; }
        if (cmd == ":vars" || cmd == ":v") { showVariables(); return true; }
        if (cmd == ":clear" || cmd == ":c") { clearVariables(); return true; }
        if (cmd == ":exit" || cmd == ":quit" || cmd == ":q") return false;
        return true;
    }
    
    void evaluateLine(const std::string& line) {
        try {
            if (!line.empty() && line[0] == ':') {
                if (!processCommand(line)) {
                    std::cout << "\nAdios!\n\n";
                    exit(0);
                }
                return;
            }
            
            std::string trimmed = line;
            size_t start = trimmed.find_first_not_of(" \t\n\r");
            if (start == std::string::npos) return;
            trimmed = trimmed.substr(start);
            trimmed = trimmed.substr(0, trimmed.find_last_not_of(" \t\n\r") + 1);
            
            if (trimmed.empty()) return;
            if (trimmed.back() != ';') {
                std::cerr << "Error: falta ';' al final\n";
                return;
            }
            
            Lexer lexer(line);
            std::vector<Token> tokens = lexer.tokenize();
            Parser parser(tokens);
            std::vector<std::unique_ptr<ASTNode>> statements = parser.parse();
            semantic.analyze(statements);
            
            for (auto& stmt : statements) {
                allStatements.push_back(std::move(stmt));
            }
            
            codegen.generate(allStatements);
            codegen.execute();
            
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
    }
    
public:
    void run() {
        printBanner();
        std::string line;
        while (true) {
            std::cout << ">>> ";
            if (!std::getline(std::cin, line)) {
                std::cout << "\n\nAdios!\n\n";
                break;
            }
            evaluateLine(line);
        }
    }
};

int main() {
    REPL repl;
    repl.run();
    return 0;
}