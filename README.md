# CG
Project for Computer Graphics Class using OpenGL / GLUT.

---

## 🐧 Linux / Mac

### Compilar o projeto

```sh
$ cmake -B build
$ cmake --build build
```

### Utilizar o Generator

```sh
$ cd build/src/generator
```

#### Gerar uma figura

```sh
$ ./generator <figura> <dados> <ficheiro de output>
```

> Nota: Todos os ficheiros gerados pelo **generator** serão criados na pasta `models`.

**Lista de figuras e inputs**:
1. `./generator plane <length> <divisions> <output>`
2. `./generator box <length> <divisions> <output>`
3. `./generator sphere <radius> <slices> <stacks> <output>`
4. `./generator cone <radius> <height> <slices> <stacks> <output>`

### Utilizar o Engine

```sh
$ cd build/src/engine
```

#### Abrir uma cena (.xml)

```sh
$ ./engine <path>
```

> Nota: Todas as cenas utilizadas poderão ser encontradas na pasta `scenes`.

---

## 🪟 Windows

### Compilar o projeto

```sh
$ cmake -B build
$ cmake --build build --config Debug
```

### Utilizar o Generator

```sh
$ cd build/src/generator/Debug
```

#### Gerar uma figura

```sh
$ .\generator.exe <figura> <dados> <ficheiro de output>
```

> Nota: Todos os ficheiros gerados pelo **generator** serão criados na pasta `models`.

**Lista de figuras e inputs**:
1. `.\generator.exe plane <length> <divisions> <output>`
2. `.\generator.exe box <length> <divisions> <output>`
3. `.\generator.exe sphere <radius> <slices> <stacks> <output>`
4. `.\generator.exe cone <radius> <height> <slices> <stacks> <output>`

### Utilizar o Engine

```sh
$ cd build/src/engine/Debug
```

#### Abrir uma cena (.xml)

```sh
$ .\engine.exe <path>
```

> Nota: Todas as cenas utilizadas poderão ser encontradas na pasta `scenes`.

---

## 🎮 Controlos do Engine

| Tecla | Ação |
|-------|------|
| ← / → | Rodar câmara horizontalmente |
| ↑ / ↓ | Rodar câmara verticalmente |
| `i` | Zoom In |
| `o` | Zoom Out |
| `a` | Ativar/desativar eixos |
| `m` | Alternar modo de renderização (wireframe / pontos / preenchido) |
| `r` | Reiniciar posição da câmara |