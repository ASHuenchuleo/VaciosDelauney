/* Encabezados para funciones auxiliares para leer y comparar vacíos. */

extern Tuple *read_onlogn_voids(char *vpath, int *vnumber);
extern Tuple *read_on_voids(char *vpath, int *vnumber);
extern int compare_voids(Tuple *a, Tuple *b, int vnumber);
