# The Guardian Journey
Algoritmos y estructura de Datos - Proyecto Unidad 2

### Tabla de contenidos
1. [Introducción](#introducción)
2. [Tecnologías](#tecnologías)
3. [Instalación](#instalación)
4. [Descripción del programa](#descripción-del-programa)
    * [Aldeas](#aldeas)
    * [Guardianes](#guardianes)


### Introducción
The Guardian Journey es un programa que tiene por objetivo reforzar el aprendizaje e implementación de las siguientes estructuras de datos: Listas, Vectores, Árboles y Grafos. Para hacerlo, simula un juego de texto, donde el jugador deberá viajar y enfrentarse a diferentes rivales.

### Tecnologías
El programa fue desarrollado en C++, ocupando las siguientes librerías:
* algorithm
* bits/stdc++.h
* cstdlib
* iostream
* time.h
* vector

### Instalación
Para instalar el programa, por favor siga los pasos descritos en el documento [INSTALL.txt](https://github.com/vis-hoo/The_Guardian_Journey/blob/main/INSTALL.txt).

## Descripción del programa
La historia detrás de The Guardian Journey es la siguiente:
> En un mundo devastado por la guerra y la lucha constante existen los Guardianes. Los Guardianes son héroes con habilidades y destrezas únicas que protegen a las aldeas a lo largo de toda la tierra siendo estas los últimos asentamientos de la humanidad.
> Tesla es la capital de estas tierras y hoy corresponde a un farol en la oscuridad donde abunda la prosperidad, el desarrollo tecnológico y entregan la mayor protección a lo largo del nuevo mundo.
> A los alrededores de Tesla se encuentran las aldeas que corresponden a asentamientos que cumplen distintas funciones dentro de la gran organización de la humanidad y en cada una de estas aldeas existen maestros, quienes enseñan las artes de los guardianes.
> El sueño de cada niño es convertirse en un Guardián y ganar el torneo, pero el viaje de un aprendiz no es fácil y su destino depende de las decisiones que tome a lo largo de este recorrido.
Para este proyecto se exige el uso de estructuras que permitan almacenar los datos de las distintas Aldeas y Guardianes.

#### Aldeas
La implementación de las aldeas se realiza a través de un conjunto de vértices de un grafo. Estos vértices se guardan en un vector de aldeas.
Para guardar la información de los caminos existentes entre las aldeas, se utiliza una matriz de adyacencia que guarda las aristas que hay entre los vértices.

#### Guardianes
La implementación de los guardianes se realiza a través de un conjunto de nodos de un árbol general, para conservar la jerarquía existente entre maestros y aprendices. Además, se utiliza un vector de guardianes para guardarlos de manera no ordenada.