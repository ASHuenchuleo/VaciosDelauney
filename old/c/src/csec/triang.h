/* Prototipos de funciones para manejar triangulación. */

extern double dist(double x0, double y0, double x1, double y1);
extern int max_edge_index(int i, double *r, int *p);
extern int is_nomax_nomax(int i, int j, int *p, int *max);
extern int is_max_max(int i, int j, int *p, int *max);
extern int get_adjacent_triangle(int i, int k, int l, int *p, int *ady);
