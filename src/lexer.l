%option c++ yyclass="Lexer" noyywrap noinput nounput

%{
#include "lexer.hpp"
#include <cerrno>
#include <cinttypes>
#include <cstdlib>
#include <limits>

#undef YY_DECL
#define YY_DECL LexResult Lexer::NextToken()
#define YY_USER_ACTION Track(std::string_view(yytext, yyleng));
%}

%%

[ \t\r\n]+ ; /* Skip whitespace. */

[0-9]+ {
    errno = 0;
    const std::intmax_t value = std::strtoimax(yytext, nullptr, 10);
    if (errno == ERANGE || value > std::numeric_limits<int64_t>::max()) {
        return std::unexpected(LexError{ matchedSpan, "Integer literal out of range: {}", std::string_view(yytext, yyleng) });
    }
    return Token::FromInteger(static_cast<int64_t>(value), matchedSpan);
}

[0-9]+(\.[0-9]*)?([eE][+-]?[0-9]+)? {
    errno = 0;
    const double value = std::strtod(yytext, nullptr);
    if (errno == ERANGE) return std::unexpected(LexError{ matchedSpan, "Float literal out of range: {}", std::string_view(yytext, yyleng) });
    return Token::FromFloat(value, matchedSpan);
}

[-+*/%()] {
    return Token::FromType(static_cast<unsigned char>(yytext[0]), matchedSpan);
}

<<EOF>> {
    return Token::FromType(Token::End, { offset, 0, location });
}

. {
    return std::unexpected(LexError{ matchedSpan, "Unknown character: '{}'", *yytext });
}

%%
