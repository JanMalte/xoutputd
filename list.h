#ifndef LIST_H
# define LIST_H

#define DECLARE_LIST(name)				\
	typedef struct {				\
		name ## _t *first;			\
		name ## _t **last;			\
	} name ## _list_t

#define LIST_FIRST(list)	(list)->first
#define LIST_NEXT(item)		(item)->next
#define LIST_INITIALIZER(list)	{NULL, &LIST_FIRST(list)}

#define INITIALIZE_LIST(list)				\
	do {						\
		(list)->first = NULL;			\
		(list)->last = &LIST_FIRST(list);	\
	} while (0)

#define ADD_TO_LIST(list, item)				\
	do {						\
		*(list)->last = (item);			\
		(list)->last = &LIST_NEXT(item);	\
	} while (0)

#define FOREACH(list, item)				\
	for((item) = LIST_FIRST(list);			\
	    (item);					\
	    (item) = LIST_NEXT(item))

#endif /* !LIST_H */
