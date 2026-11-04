%option c++ yyclass="Lexer" noyywrap noinput nounput

%{
#include "lexer.hpp"
#include <cstdlib>
%}

%%

[ \t\r\n]+                 ; /* Skip whitespace. */
([0-9]+(\.[0-9]*)?|\.[0-9]+) {
    token = Token::FromFloat(std::strtod(yytext, nullptr));
    return token.type;
}
[-+*/%()]                    {
    token = Token::FromType(static_cast<unsigned char>(yytext[0]));
    return token.type;
}
<<EOF>>                      {
    token = Token::FromType(Token::End);
    return token.type;
}
.                            {
    token = Token::FromType(Token::Invalid);
    return token.type;
}

%%
