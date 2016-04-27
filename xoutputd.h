#ifndef XOUTPUTD_H
# define XOUTPUTD_H

typedef struct output_s {
	char* name;
	RROutput id;
	Connection conn;
	struct output_s *next;
} output_t;

DECLARE_LIST(output);

typedef struct rule_s {
	output_list_t *outputs;
	const char *cmd;
	struct rule_s *next;
} rule_t;

DECLARE_LIST(rule);

extern rule_list_t rules;

output_t *lookup_output_by_id(RROutput);
output_t *lookup_output_by_name(const char *);
output_t *new_output(char *, RROutput, Connection);
rule_t *new_rule(output_list_t *, Connection, char *);

#endif /* !XOUTPUTD_H */
