% Cargar la imagen en color (mapa.png)
imagen = imread('monaña.png');

% Invertir los colores de la imagen
imagen_rgb_mejorada = 255 - imagen;

% Mostrar la imagen invertida
figure;
imshow(imagen_rgb_mejorada);
title('Imagen con Colores Invertidos');


% Guardar la imagen procesada
imwrite(imagen_rgb_mejorada, 'montaña_invertida.png');
