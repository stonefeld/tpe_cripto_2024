# Trabajo Práctico de Criptografía y Seguridad: Implementación de Steganografía en BMP

## Integrantes del grupo

- **Stanfield, Theo** - Legajo: 63403
- **Quian Blanco, Francisco** - Legajo: 63006
- **Mutz, Matías** - Legajo: 63590
- **Deyheralde, Ben** - Legajo: 63559

## Descripción del Proyecto

Este trabajo práctico consiste en la implementación de un programa llamado `stegobmp`, que permite realizar operaciones de esteganografía en archivos de imagen **BMP** utilizando los algoritmos **LSB1**, **LSB4**, y **LSBI**. Estos algoritmos permiten ocultar archivos en las imágenes BMP de manera que su presencia pase desapercibida.

El programa también ofrece la opción de cifrar el contenido oculto usando algoritmos de cifrado como **AES (128, 192, 256)** o **3DES**, junto con diferentes modos de operación (**ECB, CFB, OFB, CBC**), agregando una capa adicional de seguridad.

## Objetivos

- Implementar los algoritmos de esteganografía **LSB1**, **LSB4**, y **LSBI**.
- Permitir la opción de cifrar el archivo oculto mediante los algoritmos mencionados.
- Extraer archivos ocultos de imágenes BMP.

## Requisitos

- **`make`** para compilar el proyecto.
- **`gcc`** para compilar el código fuente.

## Compilación

1. Clonar el repositorio localmente.
2. Ejecutar el comando `make clean all` en la raíz del proyecto.
3. El ejecutable `stegobmp` se generará en el directorio `bin`.

## Ejecución

Para ocultar un archivo en una imagen BMP se debe ejecutar el siguiente comando:

```bash
./bin/stegobmp --embed --in <archivo_a_ocultar> -p <imagen_portadora.bmp> --out <imagen_salida.bmp> --steg <LSB1 | LSB4 | LSBI> [-a <aes128 | aes192 | aes256 | 3des> -m <ecb | cfb | ofb | cbc> --pass <contraseña>]
```

Por ejemplo, para ocultar un archivo llamado `mensaje.txt` en una imagen `imagen.bmp` utilizando el algoritmo **LSBI** con cifrado **AES256** en modo **CBC** con la contraseña `seguro123`, se debe ejecutar el siguiente comando:

```bash
./bin/stegobmp --embed --in mensaje.txt -p imagen.bmp --out imagen_estegano.bmp --steg LSBI -a aes256 -m cbc --pass "seguro123"
```

Para extraer un archivo oculto de una imagen BMP se debe ejecutar el siguiente comando:

```bash
./bin/stegobmp --extract -p <imagen_portadora.bmp> --out <archivo_extraido> --steg <LSB1 | LSB4 | LSBI> [-a <aes128 | aes192 | aes256 | 3des> -m <ecb | cfb | ofb | cbc> --pass <contraseña>]
```

Por ejemplo, para extraer un archivo oculto de una imagen `imagen_estegano.bmp` utilizando el algoritmo **LSBI** con cifrado **AES256** en modo **CBC** con la contraseña `seguro123`, se debe ejecutar el siguiente comando:

```bash
./bin/stegobmp --extract -p imagen_estegano.bmp --out mensaje_extraido --steg LSBI -a aes256 -m cbc --pass "seguro123"
```
