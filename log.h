#ifndef LOG_H
# define LOG_H

void log_info(const char *, ...) __attribute__((format (printf, 1, 2)));
void log_warn(const char *, ...) __attribute__((format (printf, 1, 2)));
__attribute__((noreturn)) void fatal(const char *, ...) __attribute__((format (printf, 1, 2)));

#endif /* !LOG_H */
