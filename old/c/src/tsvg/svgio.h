/* Prototipos de funciones para manejar entrada y salida de datos de SVG. */

#include <stdio.h>

extern void write_svg_header(FILE *sp, double x0, double y0, double x1, double y1);
extern void write_svg_point(FILE *sp, double x, double y, char *text);
extern void write_svg_edge(FILE *sp, double x0, double y0, double x1, double y1, int type);
extern void write_svg_label(FILE *sp, double x, double y, char *text);
extern void write_svg_comment(FILE *sp, char *text);
extern void write_svg_triangle(FILE *sp, double x0, double y0, double x1, double y1, double x2, double y2);
extern void write_svg_circumcircle(FILE *sp, double x, double y, double r);
extern void write_svg_end(FILE *sp);
