/* Definición de tipo tupla y prototipos de funciones para manejarlas. */

typedef struct{ int r; int t; } Tuple;

extern int compare_tuple_r(const void *t, const void *u);
extern int compare_tuple_t(const void *t, const void *u);
