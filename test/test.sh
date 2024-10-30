#!/bin/sh

cd "$(dirname "$0")"

stegs='LSB1 LSB4 LSBI'
algorithms='aes128 aes192 aes256 3des'
blocks='ecb cfb ofb cbc'

rm -rf inputs embeds extracts
[ -d inputs ] || mkdir inputs
[ -d embeds ] || mkdir embeds
[ -d extracts ] || mkdir extracts

for steg in $stegs; do
    input="inputs/test_${steg}.txt"
    embed="embeds/test_${steg}.bmp"
    printf "Hola mundo\nEste es un mensaje oculto en una imagen BMP usando el método $steg\n" > $input
    ../bin/stegobmp --embed --in $input -p portador.bmp --out $embed --steg $steg
done

for steg in $stegs; do
    for algorithm in $algorithms; do
        for block in $blocks; do
            input="inputs/test_${steg}_${algorithm}_${block}.txt"
            embed="embeds/test_${steg}_${algorithm}_${block}.bmp"
            printf "Hola mundo\nEste es un mensaje oculto en una imagen BMP usando el método $steg, el algoritmo $algorithm y el modo $block\n" > $input
            ../bin/stegobmp --embed --in $input -p portador.bmp --out $embed --steg $steg -a $algorithm -m $block --pass margarita
        done
    done
done

for steg in $stegs; do
    input="inputs/test_${steg}.txt"
    embed="embeds/test_${steg}.bmp"
    extract="extracts/test_${steg}"
    ../bin/stegobmp --extract -p $embed --out $extract --steg $steg
    diff $input $extract.*
done

for steg in $stegs; do
    for algorithm in $algorithms; do
        for block in $blocks; do
            input="inputs/test_${steg}_${algorithm}_${block}.txt"
            embed="embeds/test_${steg}_${algorithm}_${block}.bmp"
            extract="extracts/test_${steg}_${algorithm}_${block}"
            ../bin/stegobmp --extract -p $embed --out $extract --steg $steg -a $algorithm -m $block --pass margarita
            diff $input $extract.*
        done
    done
done