# BMP Manipuleitor

Programa de consola escrito en C para aplicar transformaciones a imágenes BMP.

## Funcionalidades

- Convertir una imagen a negativo o escala de grises.
- Ajustar las tonalidades roja, verde y azul.
- Aumentar o reducir el contraste.
- Rotar o espejar la imagen.
- Aplicar un efecto de bloques (`--romper`) o el efecto comodín.
- Concatenar dos imágenes horizontal o verticalmente.

## Breve explicación

Para realizar este proyecto investigué cómo se organiza una imagen BMP y qué información guarda su cabecera. A partir de esa investigación y de varias pruebas, implementé la lectura de datos como el ancho, el alto y la posición donde comienzan los píxeles. Luego representé los píxeles en memoria para aplicarles transformaciones y generar una imagen de salida.

El programa procesa los píxeles como componentes de color de tres bytes. Esta experiencia me permitió aprender cómo interpretar los datos de una imagen y cómo trabajar con ellos desde C para aplicar distintos efectos.

Como limitación conocida, la versión actual no maneja correctamente el padding que algunos archivos BMP agregan al final de cada fila. Una mejora pendiente sería contemplar ese relleno para procesar correctamente imágenes con distintos anchos.

## Requisitos

- GCC.
- Una imagen BMP compatible con las características que procesa el programa.

## Ejecución

Primero, compila el proyecto desde el entorno de desarrollo que prefieras. Al compilarlo se generan las carpetas de salida del proyecto (por ejemplo, `bin/Debug`) y allí queda el ejecutable. Estas carpetas no se suben a GitHub porque se generan nuevamente al compilar y están excluidas mediante `.gitignore`.

Abre una consola y accede a la carpeta de salida generada por la compilación (normalmente `bin/Debug`). Una vez allí, ejecuta el programa indicando la imagen BMP y las opciones. La imagen debe estar en esa carpeta o puedes indicar su ruta.

```sh
./bmpmanipuleitor unlam.bmp --negativo
./bmpmanipuleitor unlam.bmp --escala-de-grises --verborragico
```

Opciones disponibles:

```text
--negativo
--escala-de-grises
--romper
--rotar-izquierda
--rotar-derecha
--comodin
--espejar-horizontal
--espejar-vertical
--concatenar-vertical
--concatenar-horizontal
--verborragico
```

Para concatenar imágenes, indica dos archivos BMP:

```sh
./bmpmanipuleitor entrada.bmp segunda.bmp --concatenar-horizontal
```

Los archivos resultantes se crean en la carpeta desde la que se ejecuta el programa.
