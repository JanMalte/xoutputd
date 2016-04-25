%{

#include <X11/extensions/Xrandr.h>

#include "log.h"
#include "list.h"
#include "xmalloc.h"
#include "xoutputd.h"

int yylex(void);
__attribute__((noreturn)) void yyerror(const char *);


%}

%union {
    Connection conn;
    char *str;
    output_t *output;
    output_list_t *outputs;
}

%token	<conn>		CONNECTED DISCONNECTED
%token			EXEC WHEN
%token	<str>		OUTPUT_NAME STRING_LITERAL

%type	<output>	output
%type	<outputs>	output_list
%type	<conn>		status

%%

rules:		rule
	|	rules rule
	;

rule:		WHEN output_list status EXEC STRING_LITERAL {
rule_t *rule = new_rule($2, $3, $5);

ADD_TO_LIST(&rules, rule);
}
	;

output_list:	output {
$$ = xcalloc(1, sizeof(*$$));
INITIALIZE_LIST($$);
ADD_TO_LIST($$, $1);
}
	|	output_list '+' output {
ADD_TO_LIST($$, $3);
}
	;

output:		OUTPUT_NAME {
const output_t *output = lookup_output_by_name($1);

if (output == NULL)
    fatal("unknown output (%s)", $1);
$$ = new_output(output->name, output->id, RR_UnknownConnection);
}
	;

status:		CONNECTED | DISCONNECTED
	;

%%

void yyerror(const char *msg)
{
    fatal("config error: %s", msg);
}


