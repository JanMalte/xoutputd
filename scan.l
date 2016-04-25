%{

#include <X11/extensions/Xrandr.h>

#include "list.h"
#include "xmalloc.h"
#include "xoutputd.h"
#include "y.tab.h"

%}

%option noinput
%option nounput

%x COMMENT

%%
[ \t\n]+

#[^\n]*\n	{ }

"connected"	{
    yylval.conn = RR_Connected;
    return CONNECTED;
}

"disconnected"	{
    yylval.conn = RR_Disconnected;
    return DISCONNECTED;
}

"exec"		{ return EXEC; }
"when"		{ return WHEN; }

[a-zA-Z0-9]+	{
    yylval.str = yytext;
    return OUTPUT_NAME;
}

\"[^"\n]*\" {
    yylval.str = xstrdup(yytext + 1);
    yylval.str[strlen(yylval.str) - 1] = '\0';
    return STRING_LITERAL;
}

.		{ return yytext[0]; }

%%
