/* Prototipos de funciones auxiliares para imprimir triangulación como SVG. */

extern void extremal_coords(double *r, int pnumber, double *xmin, double *ymin,
											double *xmax, double *ymax);

extern void svg_adj_graph_DFS(int i, FILE *sp, double *r, int *p, int *adj,
												int *visited, int *max, int labels);

extern void triangle_circumcircle(double x0, double y0, double x1, double y1,
											double x2, double y2, double *x, double *y, double *r);
