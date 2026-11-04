%option c++ yyclass="Lexer" noyywrap noinput nounput

%{
#include "lexer.hpp"
#include <cerrno>
#include <cinttypes>
#include <cstdlib>
#include <limits>
%}

%%

[ \t\r\n]+                 ; /* Skip whitespace. */

[0-9]+(\.[0-9]*)?([eE][+-]?[0-9]+)? {
    errno = 0;
    const double value = std::strtod(yytext, nullptr);
    token = errno == ERANGE ? Token::FromType(Token::Invalid) : Token::FromFloat(value);
    return token.type;
}

[0-9]+                       {
    errno = 0;
    const std::intmax_t value = std::strtoimax(yytext, nullptr, 10);
    if (errno == ERANGE || value > std::numeric_limits<int64_t>::max()) {
        token = Token::FromType(Token::Invalid);
    } else {
        token = Token::FromInteger(static_cast<int64_t>(value));
    }
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
