#include "Funciones.h"


int solucionByte(int argc, char* argv[]) {
    int i, j;
    bool verborragico = false;
  const char* nombreImagen;
    t_nueva_metadata *metadata1 = NULL, *metadata2 = NULL;
    t_pixel **imagen1 = NULL, **imagen2 = NULL;

    if (argc < 2) {
        puts("Error al proporcionar el nombre del archivo de entrada");
        exit(NO_SE_PUEDE_CREAR_ARCHIVO);
    }

    for (i = 1; i < argc; i++) {
        if (strstr(argv[i], ".bmp") != NULL) {

           for (j = 1; j < argc; j++) {
                if (strcmp(argv[j], "--verborragico") == 0) {
                    verborragico = true;
                }
            }

            if (!metadata1) {
                metadata1 = leerImagen(argv[i], verborragico);
                if (metadata1) {
                    imagen1 = cargarImagen(argv[i], metadata1, verborragico);
                  nombreImagen=argv[i];
                    if (!imagen1) {
                        free(metadata1);
                        exit(NO_SE_PUEDE_CREAR_ARCHIVO);
                    }
                } else {
                    exit(NO_SE_PUEDE_CREAR_ARCHIVO);
                }
            } else if (!metadata2) {
                metadata2 = leerImagen(argv[i], verborragico);
                if (metadata2) {
                    imagen2 = cargarImagen(argv[i], metadata2, verborragico);
                    if (!imagen2) {
                        free(metadata2);
                        free(imagen1);
                        free(metadata1);
                        exit(NO_SE_PUEDE_CREAR_ARCHIVO);
                    }
                } else {
                    free(imagen1);
                    free(metadata1);
                    exit(NO_SE_PUEDE_CREAR_ARCHIVO);
                }
            }
        }
    }

    if (imagen1 && metadata1) {
        if (!procesarOpciones(nombreImagen,imagen1, metadata1, imagen2, metadata2, argv, argc, verborragico)) {
            free(imagen1);
            free(metadata1);
            if (imagen2) free(imagen2);
            if (metadata2) free(metadata2);
            exit(NO_SE_PUEDE_CREAR_ARCHIVO);
        }
    }

    free(imagen1);
    free(metadata1);
    if (imagen2) free(imagen2);
    if (metadata2) free(metadata2);

    return TODO_OK;
}

void mensajeDeFin(const char *nombreArchivo)
{
    fprintf(stderr, "Error al intentar abrir el archivo %s\n", nombreArchivo);

    exit(NO_SE_PUEDE_CREAR_ARCHIVO);
}

t_nueva_metadata *leerImagen(const char *nombreArchivo, bool verborragico)
{
    FILE *archivo = fopen(nombreArchivo, "rb");
    t_nueva_metadata *metadata;

    if (!archivo)
        mensajeDeFin(nombreArchivo);

    metadata = malloc(sizeof(t_nueva_metadata));

    if (!metadata)
    {
        fclose(archivo);
        fprintf(stderr, "Error: No se pudo asignar memoria para metadata.\n");
        return NULL;
    }

    unsigned char buffer[26];
    fread(buffer, sizeof(unsigned char), 26, archivo);

    metadata->tamArchivo = *(unsigned int *)(buffer + 2);
    metadata->comienzoImagen = *(unsigned int *)(buffer + 10);
    metadata->tamEncabezado = *(unsigned int *)(buffer + 14);
    metadata->ancho = *(unsigned int *)(buffer + 18);
    metadata->alto = *(unsigned int *)(buffer + 22);

    // leo el resto del encabezado (26 - 28 bits)
    fread(buffer, sizeof(unsigned char), 2, archivo);
    metadata->profundidad = *(unsigned short *)(buffer + 0);

    fclose(archivo);

    if (verborragico)
        puts("Imagen leida exitosamente");

    return metadata;
}

t_pixel **cargarImagen(const char *nombreArchivo, t_nueva_metadata *metadata, bool verborragico)
{
    FILE *archivo = fopen(nombreArchivo, "rb");
    t_pixel **imagen;
    unsigned int y, j, x;

    if (!archivo)
    {
        free(metadata);
        mensajeDeFin(nombreArchivo);
    }

    imagen = (t_pixel **)malloc(metadata->alto * sizeof(t_pixel *));
    if (!imagen)
    {
        fprintf(stderr, "Error: No se pudo asignar memoria para una fila de la imagen.\n");
        fclose(archivo);
        free(imagen);
        free(metadata);
        return NULL;
    }

    for (y = 0; y < metadata->alto; y++)
    {
        imagen[y] = (t_pixel *)malloc(metadata->ancho * sizeof(t_pixel));
        if (!imagen[y])
        {
            fprintf(stderr, "Error: No se pudo asignar memoria para una columna de la imagen.\n");
            fclose(archivo);
            for (j = 0; j < y; j++)
            {
                free(imagen[j]);
            }
            free(imagen);
            free(metadata);
            return NULL;
        }
    }

    // Leer hasta el comienzo de la imagen
    unsigned char buffer;
    for (unsigned int i = 0; i < metadata->comienzoImagen; i++)
    {
        fread(&buffer, sizeof(unsigned char), 1, archivo);
    }

    // Leer los p?xeles de la imagen
    for (y = 0; y < metadata->alto; y++)
    {
        for (x = 0; x < metadata->ancho; x++)
        {
            fread(&(imagen[y][x].pixel[2]), sizeof(unsigned char), 1, archivo);
            fread(&(imagen[y][x].pixel[1]), sizeof(unsigned char), 1, archivo);
            fread(&(imagen[y][x].pixel[0]), sizeof(unsigned char), 1, archivo);
        }
    }

    fclose(archivo);

    if (verborragico)
        puts("Imagen cargada correctamente");

    return imagen;
}

bool escribirImagen(const char *nombreArchivo, t_pixel **imagen, int ancho, int alto, bool verborragico)
{
    FILE *archivo = fopen(nombreArchivo, "wb");

    int y, x;
    int tamFila = (ancho * 3 + 3) & ~3;
    int tamDatos = tamFila * alto;
    int tamArchivo = tamDatos + 54;
    unsigned char cabecera[54] =
    {
        'B', 'M',
        tamArchivo & 0xFF, (tamArchivo >> 8) & 0xFF, (tamArchivo >> 16) & 0xFF, (tamArchivo >> 24) & 0xFF,
        0, 0, 0, 0,
        54, 0, 0, 0,
        40, 0, 0, 0,
        ancho & 0xFF, (ancho >> 8) & 0xFF, (ancho >> 16) & 0xFF, (ancho >> 24) & 0xFF,
        alto & 0xFF, (alto >> 8) & 0xFF, (alto >> 16) & 0xFF, (alto >> 24) & 0xFF,
        1, 0,
        24, 0,
        0, 0, 0, 0,
        tamDatos & 0xFF, (tamDatos >> 8) & 0xFF, (tamDatos >> 16) & 0xFF, (tamDatos >> 24) & 0xFF,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    };

    if (!archivo)
    {
        fprintf(stderr, "Error al intentar abrir el archivo %s\n", nombreArchivo);

        return false;
    }

    if (verborragico){
        puts("Archivo de salida creado correctamente.");}


    fwrite(cabecera, sizeof(unsigned char), 54, archivo);

    for (y = 0; y < alto; y++)
    {
        for (x = 0; x < ancho; x++)
        {
            fwrite(&(imagen[y][x].pixel[2]), sizeof(unsigned char), 1, archivo);
            fwrite(&(imagen[y][x].pixel[1]), sizeof(unsigned char), 1, archivo);
            fwrite(&(imagen[y][x].pixel[0]), sizeof(unsigned char), 1, archivo);
        }
    }

    fclose(archivo);

    if (verborragico)
        printf("Imagen escrita correctamente en el archivo %s\n", nombreArchivo);

    return true;
}
t_pixel **copiarImagen(t_pixel **imagenOriginal, int ancho, int alto)
{
    int y, j;
    t_pixel **imagenNueva = (t_pixel **)malloc(alto * sizeof(t_pixel *));

    if (!imagenNueva)
    {
        fprintf(stderr, "Error: No se pudo asignar memoria para la copia de la imagen.\n");

        free(imagenNueva);

        return NULL;
    }

    for (y = 0; y < alto; y++)
    {
        imagenNueva[y] = (t_pixel *)malloc(ancho * sizeof(t_pixel));

        if (!imagenNueva[y])
        {
            fprintf(stderr, "Error: No se pudo asignar memoria para la copia de la imagen.\n");

            for (j = 0; j < y; j++)
            {
                free(imagenNueva[j]);
            }

            free(imagenNueva);

            return NULL;
        }

        memcpy(imagenNueva[y], imagenOriginal[y], ancho * sizeof(t_pixel));
    }

    return imagenNueva;
}
bool procesarOpciones(const char* nombre,t_pixel **imagenOriginal, t_nueva_metadata *metadata, t_pixel **imagen2, t_nueva_metadata *metadata2, char *argv[], int argc, bool verborragico) {
    int i;

    unsigned int ancho, alto;

    int aucont=0,rdcont=0,tnv=0,tnr=0,tna=0,rec=0,achi=0,neg=0,gris=0,rotiz=0,rotde=0,com=0,espver=0,esphor=0,conhor=0,conver=0,rp=0;
    char nuevoNombre[256];
    for (i = 1; i < argc; i++) {
        t_pixel **imagenCopia = copiarImagen(imagenOriginal, metadata->ancho, metadata->alto);
        if (!imagenCopia) {
            fprintf(stderr, "Error: No se pudo copiar la imagen.\n");

            return false;
        }


        char *parametroPorcentaje = NULL;
        int porcentaje = 1;

        if ((parametroPorcentaje = strchr(argv[i], '=')) != NULL) {
            porcentaje = atoi(parametroPorcentaje + 1); // atoi convierte char en int


            if (porcentaje < 0 || porcentaje > 100) {
                fprintf(stderr, "Error: El porcentaje debe estar entre 0 y 100.\n");
                free(imagenCopia);

                return false;
            }

              if (strncmp(argv[i], "--aumentar-contraste=", 21) == 0) {
        if (aucont == 0) {

            aumentarContraste(imagenCopia, metadata->ancho, metadata->alto, porcentaje, verborragico);
            sprintf(nuevoNombre, "BYTE_aumentar-contraste_%s", nombre);
            escribirImagen(nuevoNombre, imagenCopia, metadata->ancho, metadata->alto, verborragico);
            aucont = 1;
            } else {printf("\n--aumentar-contraste ya fue ejecutado, no se volver? a ejecutar\n");}

        } else if (strncmp(argv[i], "--reducir-contraste=", 20) == 0) {
                if(rdcont==0){
                reducirContraste(imagenCopia, metadata->ancho, metadata->alto, porcentaje, verborragico);
                sprintf(nuevoNombre, "BYTE_reducir-contraste_%s", nombre);
                escribirImagen(nuevoNombre, imagenCopia, metadata->ancho, metadata->alto, verborragico);
                rdcont=1;
                }else{printf("\n --reducir-contraste ya fue ejecutado, no se volvera a ejecutar");}

            }   else if (strncmp(argv[i], "--tonalidad-verde=", 18) == 0) {
                 if(tnv==0){
                tonalidadVerde(imagenCopia, metadata->ancho, metadata->alto, porcentaje, verborragico);
                sprintf(nuevoNombre, "BYTE_tonalidad-verde_%s", nombre);
                escribirImagen(nuevoNombre, imagenCopia, metadata->ancho, metadata->alto, verborragico);
                tnv=1;
                }else{printf("\n --tonalidad-verde ya fue ejecutado, no se volvera a ejecutar");}

            }
            else if (strncmp(argv[i], "--tonalidad-azul=", 17) == 0) {
                if(tna==0){
                tonalidadAzul(imagenCopia, metadata->ancho, metadata->alto, porcentaje, verborragico);
                 sprintf(nuevoNombre, "BYTE_tonalidad-azul_%s", nombre);
                escribirImagen(nuevoNombre, imagenCopia, metadata->ancho, metadata->alto, verborragico);
                tna=1;
                 }else{printf("\n --tonalidad-azul ya fue ejecutado, no se volvera a ejecutar");}
            } else if (strncmp(argv[i], "--tonalidad-roja=", 17) == 0) {
                if(tnr==0){
                tonalidadRojo(imagenCopia, metadata->ancho, metadata->alto, porcentaje, verborragico);
                  sprintf(nuevoNombre, "BYTE_tonalidad-roja_%s", nombre);
                escribirImagen(nuevoNombre, imagenCopia, metadata->ancho, metadata->alto, verborragico);
                tnr=1;
                }else{printf("\n --tonalidad-roja ya fue ejecutado, no se volvera a ejecutar");}

            } else if (strncmp(argv[i], "--recortar=", 11) == 0) {
                if(rec==0){
                ancho = metadata->ancho;
                alto = metadata->alto;
                imagenCopia = recortar(&imagenCopia, &ancho, &alto, porcentaje, verborragico);
                if (imagenCopia) {
                      sprintf(nuevoNombre, "BYTE_recortar_%s", nombre);
                    escribirImagen(nuevoNombre, imagenCopia, ancho, alto, verborragico);}
                 rec=1;
               } else{printf("\n --?recortar ya fue ejecutado, no se volvera a ejecutar");}


            } else if (strncmp(argv[i], "--achicar=", 10) == 0) {
               if(achi==0){
                t_pixel **imagenAchicar = copiarImagen(imagenCopia, metadata->ancho, metadata->alto);
                t_nueva_metadata metadataReducida;
                if (!imagenAchicar) {
                    fprintf(stderr, "Error: No se pudo asignar memoria para la copia de la imagen.\n");
                    free(imagenCopia);

                    return false;
                }
                imagenAchicar = achicarImagen(imagenAchicar, metadata, &metadataReducida, porcentaje, verborragico);
                ancho = metadataReducida.ancho;
                alto = metadataReducida.alto;
                 sprintf(nuevoNombre, "BYTE_achicar_%s", nombre);
                escribirImagen(nuevoNombre, imagenAchicar, ancho, alto, verborragico);
                free(imagenAchicar);
                achi=1;
                } else{printf("\n --achicar ya fue ejecutado, no se volvera a ejecutar");}
            }
        } else {
            if (strcmp(argv[i], "--negativo") == 0) {
                if(neg==0){
                convertirImagenANegativo(imagenCopia, metadata->ancho, metadata->alto, verborragico);
                 sprintf(nuevoNombre, "BYTE_negativo_%s", nombre);
                if (!escribirImagen(nuevoNombre, imagenCopia, metadata->ancho, metadata->alto, verborragico))
                {   free(imagenCopia);

                    return false;}
                neg=1;
                } else{printf("\n --negativo ya fue ejecutado, no se volvera a ejecutar");}


        }else if (strcmp(argv[i], "--escala-de-grises") == 0){

            if(gris==0){
            convertirImagenAGrises(imagenCopia, metadata->ancho, metadata->alto, verborragico);
             sprintf(nuevoNombre, "BYTE_escala_de_grises_%s", nombre);
            if (!escribirImagen(nuevoNombre, imagenCopia, metadata->ancho, metadata->alto, verborragico))
            {   free(imagenCopia);

                return false;
            }
            gris=1;
            } else{printf("\n --escala-de-grises ya fue ejecutado, no se volvera a ejecutar");}

        } else if (strcmp(argv[i], "--romper") == 0){

            if(rp==0){
              t_pixel **imagenRota = copiarImagen(imagenCopia, metadata->ancho, metadata->alto);
          imagenRota= romper(imagenCopia, metadata->ancho, metadata->alto, verborragico);
             sprintf(nuevoNombre, "BYTE_romper_%s", nombre);
            if (!escribirImagen(nuevoNombre, imagenRota, metadata->ancho, metadata->alto, verborragico))
            {   free(imagenCopia);
                return false;
            }
            rp=1;
            } else{printf("\n --romper ya fue ejecutado, no se volvera a ejecutar");}

        }
        else if (strcmp(argv[i], "--rotar-izquierda") == 0){

            if(rotiz==0){
            t_pixel **imagenCopiaRotada = copiarImagen(imagenCopia, metadata->ancho, metadata->alto);
            ancho = metadata->ancho;
            alto = metadata->alto;
                if (!imagenCopiaRotada)
                {fprintf(stderr, "Error: No se pudo asignar memoria para la copia de la imagen.\n");
                    free(imagenCopiaRotada);
                    free(imagenCopia);

                    return false;}
            imagenCopiaRotada = rotarIzquierda(&imagenCopiaRotada, &ancho, &alto, verborragico);
                if (imagenCopiaRotada)
                {
                     sprintf(nuevoNombre, "BYTE_rotar-izquierda_%s", nombre);

                    if (escribirImagen(nuevoNombre, imagenCopiaRotada, ancho, alto, verborragico)){
                        free(imagenCopiaRotada);}
                        else
                        {free(imagenCopiaRotada);
                         free(imagenCopia);

                        return false;}
                }
                else
                { free(imagenCopiaRotada);
                  free(imagenCopia);

                  return false;}
                rotiz=1;
            } else{printf("\n --rotar-izquierda ya fue ejecutado, no se volvera a ejecutar");}


            }else if (strcmp(argv[i], "--rotar-derecha") == 0)
        {
            if(rotde==0){
            t_pixel **imagenCopiaRotada = copiarImagen(imagenCopia, metadata->ancho, metadata->alto);
            ancho = metadata->ancho;
            alto = metadata->alto;

            if (!imagenCopiaRotada)
            {
                fprintf(stderr, "Error: No se pudo asignar memoria para la copia de la imagen.\n");

                free(imagenCopiaRotada);
                free(imagenCopia);

                return false;
            }

            imagenCopiaRotada = rotarDerecha(&imagenCopiaRotada, &ancho, &alto, verborragico);

            if (imagenCopiaRotada)
            {      sprintf(nuevoNombre, "BYTE_rotar-derecha_%s", nombre);
                if (escribirImagen("BYTE_rotar-derecha.bmp", imagenCopiaRotada, ancho, alto, verborragico))
                {
                    free(imagenCopiaRotada);
                }
                else
                {
                    free(imagenCopiaRotada);
                    free(imagenCopia);


                    return false;
                }
            }
            else
            {
                free(imagenCopiaRotada);
                free(imagenCopia);


                return false;
            }
           rotde=1;
       } else{printf("\n --rotar-derecha ya fue ejecutado, no se volvera a ejecutar");}


        } else if (strcmp(argv[i], "--comodin") == 0)
        {
            if(com==0) {
            comodin(imagenCopia, metadata->ancho, metadata->alto, verborragico);
               sprintf(nuevoNombre, "BYTE_comodin_%s", nombre);

            if (!escribirImagen(nuevoNombre, imagenCopia, metadata->ancho, metadata->alto, verborragico))
            {
                free(imagenCopia);


                return false;
            }
            com=1;
            } else{printf("\n --comodin ya fue ejecutado, no se volvera a ejecutar");}
        } else if (strcmp(argv[i], "--espejar-horizontal") == 0)
        {
            if(esphor==0){
            t_pixel **imagenEspejo = copiarImagen(imagenCopia, metadata->ancho, metadata->alto);
            ancho = metadata->ancho;
            alto = metadata->alto;

            if (!imagenEspejo)
            {
                fprintf(stderr, "Error: No se pudo asignar memoria para la copia de la imagen.\n");

                free(imagenEspejo);
                free(imagenCopia);

                return false;
            }

            imagenEspejo = espejarHorizontal(&imagenEspejo, &ancho, &alto, verborragico);

            if (imagenEspejo)
            {

                   sprintf(nuevoNombre, "BYTE_espejar_horizontal_%s", nombre);
                if (escribirImagen(nuevoNombre, imagenEspejo, ancho, alto, verborragico))
                {
                    free(imagenEspejo);
                }
                else
                {
                    free(imagenEspejo);
                    free(imagenCopia);


                    return false;
                }
            }
            else
            {
                free(imagenEspejo);
                free(imagenCopia);


                return false;
            }
            esphor=1;
              } else{printf("\n --espejar-horizontal ya fue ejecutado, no se volvera a ejecutar");}
        }
            else if (strcmp(argv[i], "--espejar-vertical") == 0)
        {
            if(espver==0){
            t_pixel **imagenEspejo = copiarImagen(imagenCopia, metadata->ancho, metadata->alto);
            ancho = metadata->ancho;
            alto = metadata->alto;

            if (!imagenEspejo)
            {
                fprintf(stderr, "Error: No se pudo asignar memoria para la copia de la imagen.\n");

                free(imagenEspejo);
                free(imagenCopia);


                return false;
            }

            imagenEspejo = espejarVertical(&imagenEspejo, &ancho, &alto, verborragico);

            if (imagenEspejo)
            {

                   sprintf(nuevoNombre, "BYTE_espejar_vertical_%s", nombre);

                if (escribirImagen(nuevoNombre, imagenEspejo, ancho, alto, verborragico))
                {
                    free(imagenEspejo);
                }
                else
                {
                    free(imagenEspejo);
                    free(imagenCopia);

                    return false;
                }
            }
            else
            {
                free(imagenEspejo);
                free(imagenCopia);


                return false;
            }
            espver=1;
            } else{printf("\n --espejar-vertical ya fue ejecutado, no se volvera a ejecutar");}
           }

           else if (strcmp(argv[i], "--concatenar-vertical") == 0) {
           if(conver==0){
            t_nueva_metadata metadataNueva;
            t_pixel **imagenConcatenada = concatenarVertical(imagenCopia, metadata, imagen2, metadata2, &metadataNueva, verborragico);

            if (imagenConcatenada) {
                  sprintf(nuevoNombre, "BYTE_concatenar_vertical_%s", nombre);
                if (escribirImagen(nuevoNombre, imagenConcatenada, metadataNueva.ancho, metadataNueva.alto, verborragico)) {
                    free(imagenConcatenada);
                } else {
                    free(imagenConcatenada);
                    free(imagenCopia);

                    return false;
                }
            } else {
                free(imagenCopia);

                return false;
            }
            conver=1;
        } else{printf("\n --concatenar-vertical ya fue ejecutado, no se volvera a ejecutar");}
        }

            else if (strcmp(argv[i], "--concatenar-horizontal") == 0) {
            if(conhor==0){
            t_nueva_metadata metadataNueva;
            t_pixel **imagenConcatenada = concatenarHorizontal(imagenCopia, metadata, imagen2, metadata2, &metadataNueva,verborragico);

            if(imagenConcatenada){
                    sprintf(nuevoNombre, "BYTE_concatenar_horizontal_%s", nombre);
                if (escribirImagen(nuevoNombre, imagenConcatenada, metadataNueva.ancho, metadataNueva.alto, verborragico)) {
                    free(imagenConcatenada);
                }else {
                    free(imagenConcatenada);
                    free(imagenCopia);

                    return false;
                }
            } else {
                free(imagenCopia);

                return false;
            }
            conhor=1;
             } else{printf("\n --concatenar-horizontal ya fue ejecutado, no se volvera a ejecutar");}
        }


        }

        free(imagenCopia);
    }
    return true;
}


void comodin(t_pixel **imagen, int ancho, int alto, bool verborragico)
{
    int y, x;
    int nAlto=alto/2;
    for (y = 0; y < nAlto; y++)
    {
        for (x = 0; x < ancho; x++)
        {
            imagen[y][x].pixel[0]=MIN(255, imagen[y][x].pixel[0] + 50);
            imagen[y][x].pixel[2]=MIN(255, imagen[y][x].pixel[1] + 50);
            imagen[y+nAlto][x].pixel[1]=MIN(255, imagen[y+nAlto][x].pixel[1] + 50);
            imagen[y+nAlto][x].pixel[2]=MIN(255, imagen[y+nAlto][x].pixel[2] + 50);
        }
    }
    if (verborragico)
        puts("Imagen covertida a comodin");
}


void tonalidadVerde(t_pixel **imagen, int ancho, int alto, int porcentaje, bool verborragico) {
    int y, x;


    for (y = 0; y < alto; y++) {
        for (x = 0; x < ancho; x++) {
            imagen[y][x].pixel[1] = MIN(255, imagen[y][x].pixel[1] + porcentaje);
        }
    }

    if (verborragico)
        printf("Tonalidad roja aumentada en un %d%%.\n", porcentaje);
}




void convertirImagenANegativo(t_pixel **imagen, int ancho, int alto, bool verborragico)
{
    int y, x;

    for (y = 0; y < alto; y++)
    {
        for (x = 0; x < ancho; x++)
        {
            imagen[y][x].pixel[0] = 255 - imagen[y][x].pixel[0];
            imagen[y][x].pixel[1] = 255 - imagen[y][x].pixel[1];
            imagen[y][x].pixel[2] = 255 - imagen[y][x].pixel[2];
        }
    }

    if (verborragico)
        puts("Imagen convertida a negativo.");
}

void convertirImagenAGrises(t_pixel **imagen, int ancho, int alto, bool verborragico)
{
    int y, x, promedio;

    for (y = 0; y < alto; y++)
    {
        for (x = 0; x < ancho; x++)
        {
            promedio = (imagen[y][x].pixel[0] + imagen[y][x].pixel[1] + imagen[y][x].pixel[2]) / 3;

            if (promedio > 255)
                promedio = 255;
            else if (promedio < 0)
                promedio = 0;

            imagen[y][x].pixel[0] = (unsigned char)promedio;
            imagen[y][x].pixel[1] = (unsigned char)promedio;
            imagen[y][x].pixel[2] = (unsigned char)promedio;
        }
    }

    if (verborragico)
        puts("Imagen convertida a escala de grises.");
}

void aumentarContraste(t_pixel **imagen, int ancho, int alto, int porcentaje, bool verborragico) {
    int y, x, i;
    unsigned char promedio;
    float factor = 1 + (porcentaje / 100.0); //calculo el factor de aumento

    for (y = 0; y < alto; y++) {
        for (x = 0; x < ancho; x++) {
            promedio = (imagen[y][x].pixel[0] + imagen[y][x].pixel[1] + imagen[y][x].pixel[2]) / 3;

            for (i = 0; i < 3; i++) {
                if (promedio > 127) {
                    imagen[y][x].pixel[i] = MIN(255, imagen[y][x].pixel[i] * factor);
                } else {
                    imagen[y][x].pixel[i] = MAX(0, imagen[y][x].pixel[i] * (1 - (porcentaje / 100.0)));
                }
            }
        }
    }

    if (verborragico)
        printf("Contraste aumentado en un %d%%.\n", porcentaje);
}

void reducirContraste(t_pixel **imagen, int ancho, int alto, int porcentaje, bool verborragico) {
    int y, x, i;
    unsigned char promedio;
    float factor = 1 - (porcentaje / 100.0); // calculo el factor de reducci?n

    for (y = 0; y < alto; y++) {
        for (x = 0; x < ancho; x++) {
            promedio = (imagen[y][x].pixel[0] + imagen[y][x].pixel[1] + imagen[y][x].pixel[2]) / 3;

            for (i = 0; i < 3; i++) {
                if (promedio > 127) {
                    imagen[y][x].pixel[i] = MAX(0, imagen[y][x].pixel[i] * factor);
                } else {
                    imagen[y][x].pixel[i] = MIN(255, imagen[y][x].pixel[i] * (1 + (porcentaje / 100.0)));
                }
            }
        }
    }

    if (verborragico)
        printf("Contraste reducido en un %d%%.\n", porcentaje);
}

void tonalidadAzul(t_pixel **imagen, int ancho, int alto, int porcentaje, bool verborragico) {
    int y, x;

    for (y = 0; y < alto; y++) {
        for (x = 0; x < ancho; x++) {
            imagen[y][x].pixel[2] = MIN(255, imagen[y][x].pixel[2] + porcentaje);
        }
    }

    if (verborragico)
        printf("Tonalidad azul aumentada en un %d%%.\n", porcentaje);
}

t_pixel **rotarIzquierda(t_pixel ***imagen, unsigned int *ancho, unsigned int *alto, bool verborragico)
{
    unsigned int i, j, k, temp;
    t_pixel **imagenRotada = (t_pixel **)malloc((*ancho) * sizeof(t_pixel *));

    if (!imagenRotada)
    {
        fprintf(stderr, "Error: No se pudo asignar memoria para la imagen rotada.\n");

        free(imagenRotada);

        return NULL;
    }

    for (i = 0; i < *ancho; i++)
    {
        imagenRotada[i] = (t_pixel *)malloc((*alto) * sizeof(t_pixel));

        if (!imagenRotada[i])
        {
            fprintf(stderr, "Error: No se pudo asignar memoria para la imagen rotada.\n");

            for (k = 0; k < i; k++)
            {
                free(imagenRotada);
            }

            free(imagenRotada);

            return NULL;
        }
    }

    for (i = 0; i < *alto; i++)
    {
        for (j = 0; j < *ancho; j++)
        {
            imagenRotada[j][*alto - i - 1] = (*imagen)[i][j];
        }
    }

    temp = *ancho;
    *ancho = *alto;
    *alto = temp;
    *imagen = imagenRotada;

    if (verborragico)
        puts("Imagen rotada a la izquierda");

    return *imagen;
}

t_pixel **rotarDerecha(t_pixel ***imagen, unsigned int *ancho, unsigned int *alto, bool verborragico)
{
    unsigned int i, j, k, temp;
    t_pixel **imagenRotada = (t_pixel **)malloc((*ancho) * sizeof(t_pixel ));

    if (!imagenRotada)
    {
        fprintf(stderr, "Error: No se pudo asignar memoria para la imagen rotada.\n");

        free(imagenRotada);

        return NULL;
    }

    for (i = 0; i < *ancho; i++)
    {
        imagenRotada[i] = (t_pixel *)malloc((*alto) * sizeof(t_pixel));

        if (!imagenRotada[i])
        {
            fprintf(stderr, "Error: No se pudo asignar memoria para la imagen rotada.\n");

            for (k = 0; k < i; k++)
            {
                free(imagenRotada);
            }

            free(imagenRotada);

            return NULL;
        }
    }

    for (i = 0; i < *alto; i++)
    {
        for (j = 0; j < *ancho; j++)
        {
            imagenRotada[*ancho - j - 1][i] = (*imagen)[i][j];
        }
    }

    temp = *ancho;
    *ancho = *alto;
    *alto = temp;
    *imagen = imagenRotada;

    if (verborragico)
        puts("Imagen rotada a la derecha");

    return *imagen;
}




void tonalidadRojo(t_pixel **imagen, int ancho, int alto, int porcentaje, bool verborragico) {
    int y, x;


    for (y = 0; y < alto; y++) {
        for (x = 0; x < ancho; x++) {
            imagen[y][x].pixel[0] = MIN(255, imagen[y][x].pixel[0] + porcentaje);
        }
    }

    if (verborragico)
        printf("Tonalidad roja aumentada en un %d%%.\n", porcentaje);
}
t_pixel **romper(t_pixel **imagen, int ancho, int alto, bool verborragico)
{
    int N =30;  // Tama?o del bloque
    int i, j, x, y;

    // Asignar memoria para la nueva imagen rota
    t_pixel **imagenRota = (t_pixel **)malloc(alto * sizeof(t_pixel *));
    if (!imagenRota)
    {
        fprintf(stderr, "Error: No se pudo asignar memoria para la imagen rota.\n");
        return NULL;
    }
    for (i = 0; i < alto; i++)
    {
        imagenRota[i] = (t_pixel *)malloc(ancho * sizeof(t_pixel));
        if (!imagenRota[i])
        {
            fprintf(stderr, "Error: No se pudo asignar memoria para la imagen rota.\n");
            for (int k = 0; k < i; k++)
            {
                free(imagenRota[k]);
            }
            free(imagenRota);
            return NULL;
        }
    }

    // Rotar la imagen en bloques de NxN
    for (i = 0; i < alto; i += N)
    {
        for (j = 0; j < ancho; j += N)
        {
            // Procesar cada bloque de N x N
            for (x = 0; x < N && (x + i) < alto; x++)
            {
                for (y = 0; y < N && (y + j) < ancho; y++)
                {
                    int bj = N - 1;  // Invertir la coordenada Y dentro del bloque
                     imagenRota[i+(bj-y)][j+x].pixel[0]= imagen[i+x][j+y].pixel[0];
                     imagenRota[i+(bj-y)][j+x].pixel[1]= imagen[i+x][j+y].pixel[1];
                     imagenRota[i+(bj-y)][j+x].pixel[2]= imagen[i+x][j+y].pixel[2];
                }
            }
        }
    }

    if (verborragico)
    {
        printf("Imagen rota.\n");
    }

    return imagenRota;
}


t_pixel **recortar(t_pixel ***imagen, unsigned int *ancho, unsigned int *alto, int porcentaje, bool verborragico)
{
    unsigned int nuevo_ancho = (*ancho * porcentaje) / 100;
    unsigned int nuevo_alto = (*alto * porcentaje) / 100;
    unsigned int i, j;

    t_pixel **imagen_recortada = (t_pixel **)malloc(nuevo_alto * sizeof(t_pixel *));
    if (!imagen_recortada)
    {
        fprintf(stderr, "Error: No se pudo asignar memoria para la imagen recortada.\n");
        return NULL;
    }

    for (i = 0; i < nuevo_alto; i++)
    {
        imagen_recortada[i] = (t_pixel *)malloc(nuevo_ancho * sizeof(t_pixel));
        if (!imagen_recortada[i])
        {
            fprintf(stderr, "Error: No se pudo asignar memoria para la imagen recortada.\n");
            for (j = 0; j < i; j++)
            {
                free(imagen_recortada[j]);
            }
            free(imagen_recortada);
            return NULL;
        }

        memcpy(imagen_recortada[i], (*imagen)[i], nuevo_ancho * sizeof(t_pixel));
    }

    // modifico el ancho y el alto
    *ancho = nuevo_ancho;
    *alto = nuevo_alto;

    // vacio la variable original
    for (i = 0; i < *alto; i++)
    {
        free((*imagen)[i]);
    }
    free(*imagen);

    // almaceno  en la variable original
    *imagen = imagen_recortada;

    if (verborragico)
        puts("Imagen recortada.");

    return *imagen;
}


t_pixel **achicarImagen(t_pixel **imagenOriginal, t_nueva_metadata *metadataOriginal, t_nueva_metadata *metadataReducida, int porcentaje, bool verborragico) {
    unsigned int y, x;
    int factorReduccion = 100 / porcentaje;

    // modicico la nuevas  dimensiones de la imagen reducida
    metadataReducida->ancho = metadataOriginal->ancho / factorReduccion;
    metadataReducida->alto = metadataOriginal->alto / factorReduccion;

    // creo la nueva imagen con las nuevas dimensiones
    t_pixel **imagenReducida = (t_pixel **)malloc(metadataReducida->alto * sizeof(t_pixel *));
    if (!imagenReducida) {
        fprintf(stderr, "Error: No se pudo asignar memoria para la imagen reducida.\n");
        return NULL;
    }

    for (y = 0; y < metadataReducida->alto; y++) {
        imagenReducida[y] = (t_pixel *)malloc(metadataReducida->ancho * sizeof(t_pixel));
        if (!imagenReducida[y]) {
            fprintf(stderr, "Error: No se pudo asignar memoria para una fila de la imagen reducida.\n");
            for (unsigned int j = 0; j < y; j++) {
                free(imagenReducida[j]);
            }
            free(imagenReducida);
            return NULL;
        }
    }

    // copio los pixles
    for (y = 0; y < metadataReducida->alto; y++) {
        for (x = 0; x < metadataReducida->ancho; x++) {
            unsigned int srcY = y * factorReduccion;
            unsigned int srcX = x * factorReduccion;
            imagenReducida[y][x] = imagenOriginal[srcY][srcX];
        }
    }

    if (verborragico)
        printf("Imagen achicada en un %d%%.\n", porcentaje);

    return imagenReducida;
}

t_pixel **espejarHorizontal(t_pixel ***imagen, unsigned int *ancho, unsigned int *alto, bool verborragico)
{
    unsigned int i, j, k;
    t_pixel **imagenEspejo = (t_pixel **)malloc((*alto) * sizeof(t_pixel *));


    if (!imagenEspejo)
    {
        fprintf(stderr, "Error: No se pudo asignar memoria para la imagen rotada.\n");

        free(imagenEspejo);

        return NULL;
    }

    for (i = 0; i < *alto; i++)
    {
        imagenEspejo[i] = (t_pixel *)malloc((*ancho) * sizeof(t_pixel));

        if (!imagenEspejo[i])
        {
            fprintf(stderr, "Error: No se pudo asignar memoria para la imagen rotada.\n");

            for (k = 0; k < i; k++)
            {
                free(imagenEspejo);
            }

            free(imagenEspejo);

            return NULL;
        }
    }

    for (i = 0; i < *alto; i++)
    {
        for (j = 0; j < *ancho; j++)
        {
            imagenEspejo[i][ j] = (*imagen)[i][ *ancho - j];
        }
    }


    *imagen = imagenEspejo;

    if (verborragico)
        puts("Imagen espejada horizontalmente");

    return *imagen;
}

t_pixel **espejarVertical(t_pixel ***imagen, unsigned int *ancho, unsigned int *alto, bool verborragico)
{
     unsigned int i, j, k;
    t_pixel **imagenEspejo = (t_pixel **)malloc((*alto) * sizeof(t_pixel *));

    if (!imagenEspejo)
    {
        fprintf(stderr, "Error: No se pudo asignar memoria para la imagen rotada.\n");

        free(imagenEspejo);

        return NULL;
    }

    for (i = 0; i < *alto; i++)
    {
        imagenEspejo[i] = (t_pixel *)malloc((*ancho) * sizeof(t_pixel));

        if (!imagenEspejo[i])
        {
            fprintf(stderr, "Error: No se pudo asignar memoria para la imagen rotada.\n");

            for (k = 0; k < i; k++)
            {
                free(imagenEspejo);
            }

            free(imagenEspejo);

            return NULL;
        }
    }

    for (i = 0; i < *alto; i++)
    {
        for (j = 0; j < *ancho; j++)
        {
            imagenEspejo[*alto - i -1 ][ *ancho -j -1] = (*imagen)[i][ *ancho - j];
        }
    }


    *imagen = imagenEspejo;

    if (verborragico)
        puts("Imagen espejada horizontalmente");




    return *imagen;
}

t_pixel **concatenarVertical(t_pixel** imagen1, t_nueva_metadata *metadata1, t_pixel** imagen2, t_nueva_metadata *metadata2, t_nueva_metadata* metadataNueva,bool verborragico)
{
    metadataNueva->alto=metadata1->alto + metadata2->alto;

    if(metadata1->ancho > metadata2->ancho)
    {
        metadataNueva->ancho=metadata1->ancho;

    }else{
        metadataNueva->ancho=metadata2->ancho;
    }


     t_pixel **imagenConcatenar = malloc(metadataNueva->alto * sizeof(t_pixel *));
    if (!imagenConcatenar)
    {
        return NULL;
    }


    for (int i = 0; i < metadataNueva->alto; i++)
    {
        imagenConcatenar[i] = malloc(metadataNueva->ancho * sizeof(t_pixel));
        if (!imagenConcatenar[i])
        {

            for (int k = 0; k < i; k++)
            {
                free(imagenConcatenar[k]);
            }
            free(imagenConcatenar);
            return NULL;
        }
    }
    int i;
    for(i=0;i<metadata1->alto;i++){
        for(int j=0;j<metadata1->ancho;j++){
         imagenConcatenar[i][j]=imagen1[i][j];
        }
    }

     for(i=0;i<metadata2->alto;i++){
        for(int j=0;j<metadata2->ancho;j++){
         imagenConcatenar[i + metadata1->alto][j]=imagen2[i][j];
        }
    }
// relleno
    if(metadata1->ancho > metadata2->ancho)
    {
      for(i=0;i<metadata2->alto;i++){
        for(int j=metadata2->ancho ; j<metadata1->ancho;j++){
         imagenConcatenar[i + metadata1->alto][j].pixel[0] =20;
         imagenConcatenar[i + metadata1->alto][j].pixel[1] =100;
         imagenConcatenar[i + metadata1->alto][j].pixel[2] =20;
        }
      }
    }

     if(metadata2->ancho > metadata1->ancho)
    {
      for(i=0; i<metadata1->alto ;i++){
        for(int j=metadata1->ancho ; j<metadata2->ancho;j++){
         imagenConcatenar[i ][j].pixel[0] =20;
         imagenConcatenar[i ][j].pixel[1] =100;
         imagenConcatenar[i ][j].pixel[2] =20;
        }
      }
    }

     if (verborragico)
        puts("Imagen concatenada verticalmente");

    return imagenConcatenar;
}

t_pixel **concatenarHorizontal(t_pixel** imagen1, t_nueva_metadata *metadata1, t_pixel** imagen2, t_nueva_metadata *metadata2, t_nueva_metadata* metadataNueva,bool verborragico)
{
   metadataNueva->ancho= metadata1->ancho + metadata2->ancho;

    if(metadata1->alto > metadata2->alto)
    {
        metadataNueva->alto=metadata1->alto;
    }else{
        metadataNueva->alto=metadata2->alto;
    }

    t_pixel** imagenConcatenada=malloc(metadataNueva->alto * sizeof(t_pixel));
    if(!imagenConcatenada){

        free(imagenConcatenada);
        return NULL;
    }
    int i,j;

     for (int i = 0; i < metadataNueva->alto; i++)
    {
        imagenConcatenada[i] = malloc(metadataNueva->ancho * sizeof(t_pixel));
        if (!imagenConcatenada[i])
        {

            for (int k = 0; k < i; k++)
            {
                free(imagenConcatenada[k]);
            }
            free(imagenConcatenada);
            return NULL;
        }
    }

    for(i=0;i<metadata1->alto;i++){
      for(j=0;j<metadata1->ancho;j++){
         imagenConcatenada[i][j]=imagen1[i][j];
         }
    }

     for(i=0;i<metadata2->alto;i++){
      for(j=0;j<metadata2->ancho;j++){
         imagenConcatenada[i][j + metadata1->ancho]=imagen2[i][j];
         }
    }

    // relleno
   if (metadata1->alto > metadata2->alto)
    {
        for (int i = metadata2->alto; i < metadata1->alto; i++)
        {
            for (int j = 0; j < metadata2->ancho; j++)
            {
                imagenConcatenada[i][j + metadata1->ancho].pixel[0] = 100;
                imagenConcatenada[i][j + metadata1->ancho].pixel[1] = 50;
                imagenConcatenada[i][j + metadata1->ancho].pixel[2] = 50;
            }
        }
    }

    if (metadata2->alto > metadata1->alto)
    {
        for (int i = metadata1->alto; i < metadata2->alto; i++)
        {
            for (int j = 0; j < metadata1->ancho; j++)
            {
                imagenConcatenada[i][j].pixel[0] = 100;
                imagenConcatenada[i][j].pixel[1] = 50;
                imagenConcatenada[i][j].pixel[2] = 50;
            }
        }
    }
    if (verborragico)
        puts("Imagen concatenada horizontalmente");
return imagenConcatenada;
}


