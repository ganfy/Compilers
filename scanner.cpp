#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>

using namespace std;

enum TokenType {
    Keyword,
    Identifier,
    Operator,
    LetterLiteral,
    NumLiteral,
    Delimiter,
    Comment,
    EOP
};

struct Token {
    TokenType tokentype = TokenType::Identifier;
    string tokenvalue;
};

// SCANNER
class Scanner{
    public:
    string source_code;
    int pos;

    Scanner() {
        this->source_code = "";
        this->pos = 0;
    }

    Scanner(string source_code) {
        this->source_code = source_code;
        this->pos = 0;
    }

    char getchar() {
        if (this->pos >= this->source_code.length()) {
            return '\0';
        }
        return this->source_code[this->pos++];
    }
    char peekchar() {
        if (this->pos >= this->source_code.length()) {
            return '\0';
        }
        return this->source_code[this->pos];
    }

    bool is_operator(char c) {
        static const std::string operators = "+-/*%^&|~=<>";
        return operators.find(c) != std::string::npos;
    }

    bool is_delimiter(char c) {
        return (c == 'c' && peekchar() == ':') || (c == ':' && peekchar() == 'c'  || c == '{' || c == '}' || c == '(' || c == ')');
    }

    bool is_comment(char c) {
        return (c == '*' && peekchar() == '*');
    }

    bool is_lliteral(char c) {
        return (c == '"' || c == '\'');
    }

    bool is_nliteral(char c) {
        return isdigit(c);
    }

    bool is_keyword(string token) {
        vector<string> keywords = {
            "until", "verify", "planb", "abort", "kg", "print", "read", "open", "close", "num", "letter", "list", "logic", "yup", "nope"
        };
        return std::find(keywords.begin(), keywords.end(), token) != keywords.end();
    }

    Token gettoken(){

        Token token;
        token.tokentype = TokenType::EOP;

        if (pos >= source_code.size()) return token;
        
        while (pos < source_code.length()) {
            char c = getchar();
            //Ignorar espacio
            if (isspace(c) || c == '\t' || c == '\n' || c == '\r') {
                continue;
            }
            cout << pos << " of ";
            //Token comentario hasta segundo **
            if (is_comment(c)) {
                string comment;
                comment += c;
                c = getchar();
                comment += c;
                while (pos < source_code.size() && !is_comment(c)) {
                    c = getchar();
                    comment += c;
                }

                c = peekchar();
                //verificar comentario no cerrado
                if(c!='*'){
                    token.tokentype = TokenType::Comment;
                    token.tokenvalue = comment + "Error: Comentario no cerrado";
                    return token;
                }
                c = getchar();
                comment += c;

                token.tokentype = TokenType::Comment;
                token.tokenvalue = comment;

                return token;
            }

            //Token delimitador
            else if (is_delimiter(c)) {
                string delimiter;
                delimiter += c;
                c = getchar();
                delimiter += c;

                token.tokentype = TokenType::Delimiter;
                token.tokenvalue = delimiter;

                return token;
            }

            //Token operador
            else if (is_operator(c)) {
                string op;
                op += c;
                c = peekchar();
                if (c == '=') {
                    op += c;
                    getchar();
                }

                token.tokentype = TokenType::Operator;
                token.tokenvalue = op;

                return token;
            }

            //Token letterliteral
            else if (is_lliteral(c)) {
                string literal;
                literal += c;
                c = getchar();
                while (pos < source_code.size() && !is_lliteral(c)) {
                    literal += c;
                    c = getchar();
                }

                if(c!='\'' && c!='\"'){
                    token.tokenvalue = literal + " Error: Cadena no cerrada";
                    token.tokentype = TokenType::LetterLiteral;
                    return token;
                }

                literal += c;            

                token.tokentype = TokenType::LetterLiteral;
                token.tokenvalue = literal;

                return token;
            }

            //Token numliteral incluyendo decimales ej: 3.14
            else if (is_nliteral(c)) {
                string literal;
                literal += c;
                c = getchar();
                while (pos < source_code.size() && is_nliteral(c)) {
                    literal += c;
                    c = getchar();
                }

                
                if(c=='.'){
                    literal += c;
                    c = getchar();
                    while (pos < source_code.size() && is_nliteral(c)) {
                        literal += c;
                        c = getchar();
                    }
                }

                //verificar que no haya letras despues del numero o doble punto
                if(isalpha(c) || c=='.'){
                    literal += c;
                    c = getchar();
                    while (pos < source_code.size() && is_nliteral(c) || isalpha(c) || c=='.') {
                        literal += c;
                        c = getchar();
                    }
                    token.tokenvalue = "Literal no valido ";
                }

                literal += c;
                token.tokentype = TokenType::NumLiteral;
                token.tokenvalue += literal;

                return token;
            }

            //Token keyword e identifier
            else if (isalpha(c)) {
                string keyword;
                keyword += c;
                c = getchar();
                while (pos < source_code.size() && isalpha(c)) {
                    keyword += c;
                    c = getchar();
                }
                keyword += c;
              
                if(is_keyword(keyword)){
                    token.tokentype = TokenType::Keyword;
                    token.tokenvalue = keyword;
                }
                else{
                    token.tokentype = TokenType::Identifier;
                    token.tokenvalue = keyword;
                }
                return token;
            }

        }
        return token;
      }
};

int main() {
    std::ifstream file("lizzi_code.txt");

    if (!file) {
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }


    Scanner scanner;
    string line;
    string linesString;
    int lineCount = 0;

    while (std::getline(file, line)) {
        linesString += line + '\n'; 
        lineCount++;
        scanner.source_code = line;
        scanner.pos = 0;
        Token token;
        while (token.tokentype != TokenType::EOP) {
            token = scanner.gettoken();
            if (token.tokentype != TokenType::EOP){
              cout << "LINE " << lineCount;
              std::cout << " " << token.tokentype << ": " << token.tokenvalue << ::endl;
            }
        };
    }


    return 0;
}
