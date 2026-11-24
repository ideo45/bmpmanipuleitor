#ifndef FUNCIONES_H_INCLUDED
#define FUNCIONES_H_INCLUDED
#include "estructuras.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define TODO_OK 0
#define ARCHIVO_NO_ENCONTRADO 10
#define NO_SE_PUEDE_CREAR_ARCHIVO 20
typedef struct { unsigned int tamArchivo, tamEncabezado, comienzoImagen, ancho, alto; unsigned short profundidad; } t_nueva_metadata;
void tonalidadRojo(t_pixel **, int, int, int, bool);
t_pixel **recortar(t_pixel ***, unsigned int *, unsigned int *, int, bool);
t_pixel **romper(t_pixel **, int, int, bool);
t_pixel **concatenarHorizontal(t_pixel **, t_nueva_metadata *, t_pixel **, t_nueva_metadata *, t_nueva_metadata *, bool);
t_pixel **concatenarVertical(t_pixel **, t_nueva_metadata *, t_pixel **, t_nueva_metadata *, t_nueva_metadata *, bool);
t_pixel **espejarVertical(t_pixel ***, unsigned int *, unsigned int *, bool);
t_pixel **espejarHorizontal(t_pixel ***, unsigned int *, unsigned int *, bool);
t_pixel **achicarImagen(t_pixel **, t_nueva_metadata *, t_nueva_metadata *, int, bool);
int solucionByte(int, char *[]);
bool procesarOpciones(const char *, t_pixel **, t_nueva_metadata *, t_pixel **, t_nueva_metadata *, char *[], int, bool);
bool escribirImagen(const char *, t_pixel **, int, int, bool);
t_pixel **copiarImagen(t_pixel **, int, int);
t_pixel **cargarImagen(const char *, t_nueva_metadata *, bool);
t_nueva_metadata *leerImagen(const char *, bool);
void mensajeDeFin(const char *);
void comodin(t_pixel **, int, int, bool);
void tonalidadVerde(t_pixel **, int, int, int, bool);
void convertirImagenANegativo(t_pixel **, int, int, bool);
void convertirImagenAGrises(t_pixel **, int, int, bool);
void aumentarContraste(t_pixel **, int, int, int, bool);
void reducirContraste(t_pixel **, int, int, int, bool);
void tonalidadAzul(t_pixel **, int, int, int, bool);
t_pixel **rotarIzquierda(t_pixel ***, unsigned int *, unsigned int *, bool);
t_pixel **rotarDerecha(t_pixel ***, unsigned int *, unsigned int *, bool);
int solucion(int, char *[]);
#endif
