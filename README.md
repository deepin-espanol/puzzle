# Puzzle 15

El rompecabezas deslizante: ordena las fichas del 1 al último. Desarrollado con **Qt6** y **Deepin Tool Kit (DTK6)** para Deepin Linux.

## Características

- Tamaños 3×3, 4×4 y 5×5
- Clic en cualquier ficha de la fila o columna del hueco para deslizar la línea
- Cronómetro y récord de movimientos por tamaño
- Siempre tiene solución
- Traducido a 17 idiomas

## Controles

| Tecla | Acción |
|-------|--------|
| Clic | Deslizar fichas |
| Flechas / WASD | Mover la ficha junto al hueco |
| Ctrl+N | Nuevo juego |
| Ctrl+Q | Salir |

## Instalación

Descarga el paquete de la [última release](https://github.com/deepin-espanol/puzzle/releases/latest):

- `.deb` (amd64, arm64, riscv64, loong64):
  ```bash
  sudo apt install ./puzzle_*.deb
  ```
- `.uab` de linyaps (amd64, arm64): ejecutable directamente.
  ```bash
  chmod +x org.deepinenespanol.*.uab && ./org.deepinenespanol.*.uab
  ```

## Compilar

```bash
sudo apt build-dep ./          # dependencias declaradas en debian/control
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/puzzle
```

O generar el paquete `.deb`:

```bash
dpkg-buildpackage -us -uc -b
```

## Publicar una versión

Añade una entrada nueva en `debian/changelog` y haz push: el workflow
*Build and publish .deb* crea la release con los `.deb` y los `.uab`.

## Licencia

GPL-3.0 o posterior. © Deepin en Español — https://deepinenespanol.org
