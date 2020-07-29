/* Funciones para manejar entrada y salida de datos de SVG. */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "consts.h"

#define SVG_HEIGHT 600

#define STRONG_LINE_SCALE 0.009
#define LINE_SCALE 0.006
#define MEDIUM_LINE_SCALE 0.0035
#define THIN_LINE_SCALE 0.002
#define FONT_SCALE 0.04
#define POINT_CIRCLE_SCALE 0.020

#define MAXMAX_COLOR "#4A7"
#define NOMAXNOMAX_COLOR "#000"
#define BORDER_COLOR "#000"
#define MAXNOMAX_COLOR "#999"

#define POINT_LABEL_TEXT_COLOR "#FFF"
#define LABEL_TEXT_COLOR "#777"

#define POINT_BACKGROUND_COLOR "#000"
#define TRIANGLE_COLOR "#e1f7ff"

#define CIRCUMCIRCLE_BORDER_COLOR "#BBB"
#define CIRCUMCIRCLE_BORDER_SCALE 0.006

void write_svg_header(FILE *sp, double x0, double y0, double x1, double y1)
{
	double u;
	double v;
	double l0;
	double l1;
	double w;
	double h;
	double delta;
	
	/* Grosor de "marco". */
	delta = 50;
	
	/* Calcular coordenadas de esquina inferior izquierda de cuadro de visualización. */
	u = x0 - delta;
	v = y0 - delta;
	
	/* Calcular ancho y alto de cuadro de visualización. */
	l0 = fabs(x0 - x1) + 2*delta;
	l1 = fabs(y0 - y1) + 2*delta;
	
	/* Calcular ancho y alto de imagen total. */
	h = SVG_HEIGHT;
	w = h*l1/l0;
	
	fprintf(sp, "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" ");
	fprintf(sp, "viewBox=\"%lf, %lf, %lf, %lf\" stroke-width=\"0\" width=\"%lf\" height=\"%lf\">\n",
					u, v, l0, l1, w, h);
}


void write_svg_point(FILE *sp, double x, double y, char *text)
{
	/* Cuidado con sistema de coordenadas de SVG! */
	y = -y;
	
	fprintf(sp, "	<circle transform=\"translate(%.15f,%.15f)\" ", x, y);
	fprintf(sp, "cx=\"0\" cy=\"0\" r=\"%f\" style=\"fill:%s\"/>\n", POINT_CIRCLE_SCALE*SVG_HEIGHT, POINT_BACKGROUND_COLOR);
	
	if(strcmp(text, "") != 0)
	{
		fprintf(sp, "	<text transform=\"translate(%.15f %.15f)\" ", x, y);
		fprintf(sp, "x=\"0\" y=\"0\" fill=\"%s\" font-size=\"%f\" text-anchor=\"middle\" ",
						POINT_LABEL_TEXT_COLOR, FONT_SCALE*SVG_HEIGHT);
		fprintf(sp, "dominant-baseline=\"middle\" font-family=\"Arial\">%s</text>\n", text);
	}
}


void write_svg_edge(FILE *sp, double x0, double y0, double x1, double y1, int type)
{
	char *color;
	double stroke_width;
	
	/* Cuidado con sistema de coordenadas de SVG! */
	y0 = -y0;
	y1 = -y1;
	
	if(type == BORDER_EDGE)
	{
		color = BORDER_COLOR;
		stroke_width = STRONG_LINE_SCALE*SVG_HEIGHT;
	}
	else if(type == MAXMAX_EDGE)
	{
		color = MAXMAX_COLOR;
		stroke_width = MEDIUM_LINE_SCALE*SVG_HEIGHT;
	}
	else if(type == NOMAXNOMAX_EDGE)
	{
		color = NOMAXNOMAX_COLOR;
		stroke_width = LINE_SCALE*SVG_HEIGHT;
	}
	else if(type == MAXNOMAX_EDGE)
	{
		color = MAXNOMAX_COLOR;
		stroke_width = THIN_LINE_SCALE*SVG_HEIGHT;
	}
	
	fprintf(sp, "	<line x1=\"%.15f\" y1=\"%.15f\" ", x0, y0);
	fprintf(sp, "x2=\"%.15f\" y2=\"%.15f\" style=\"stroke:%s;stroke-width:%f;\" stroke-linecap=\"round\"/>\n",
						x1, y1, color, stroke_width);
}


void write_svg_label(FILE *sp, double x, double y, char *text)
{
	/* Cuidado con sistema de coordenadas de SVG! */
	y = -y;
	
	fprintf(sp, "	<text transform=\"translate(%.15f %.15f)\" x=\"0\" y=\"0\" ", x, y);
	fprintf(sp, "fill=\"%s\" text-anchor=\"middle\" dominant-baseline=\"middle\" ", LABEL_TEXT_COLOR);
	fprintf(sp, "font-size=\"%f\" font-family=\"Arial\">%s</text>\n", FONT_SCALE*SVG_HEIGHT, text);
}


void write_svg_comment(FILE *sp, char *text)
{
	fprintf(sp, "<!-- %s -->\n", text);
}

void write_svg_triangle(FILE *sp, double x0, double y0, double x1, double y1, double x2, double y2)
{
	/* Cuidado con sistema de coordenadas de SVG! */
	y0 = -y0;
	y1 = -y1;
	y2 = -y2;
	
	fprintf(sp, "	<polygon points=\"%lf,%lf %lf,%lf %lf,%lf\" ",
					x0, y0, x1, y1, x2, y2);
	fprintf(sp, "style=\"fill:%s;stroke-width:0\" />\n", TRIANGLE_COLOR);
}


void write_svg_circumcircle(FILE *sp, double x, double y, double r)
{
	/* Cuidado con sistema de coordenadas de SVG! */
	y = -y;
	
	fprintf(sp, "	<circle transform=\"translate(%.15f,%.15f)\" ", x, y);
	fprintf(sp, "cx=\"0\" cy=\"0\" r=\"%f\" style=\"fill:none;stroke:%s;stroke-width:%f;\"/>\n",
					r, CIRCUMCIRCLE_BORDER_COLOR, CIRCUMCIRCLE_BORDER_SCALE*SVG_HEIGHT);
	
}


void write_svg_end(FILE *sp)
{
	fprintf(sp, "</svg>");
}
