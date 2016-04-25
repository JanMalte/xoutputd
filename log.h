#ifndef _LOG_H_
# define _LOG_H_

void log_info(const char *, ...) __attribute__((format (printf, 1, 2)));
void log_warn(const char *, ...) __attribute__((format (printf, 1, 2)));
__attribute__((noreturn)) void fatal(const char *, ...) __attribute__((format (printf, 1, 2)));

#endif /* !_LOG_H_ */
