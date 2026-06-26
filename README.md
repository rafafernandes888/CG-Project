# CG

Este repositório contém o projeto da Unidade Curricular de **Computação Gráfica** (CG) no ano letivo 2025/2026, desenvolvido em C++ com OpenGL. O objetivo deste projeto é desenvolver um **motor 3D**.

## 📁 Estrutura do Projeto
```text
CG/
├── src/
│   ├── engine/       # Código do motor de renderização
│   ├── generator/    # Código do gerador de primitivas
│   ├── models/       # Modelos gerados em formato .3d
│   ├── scenes/       # Ficheiros XML das cenas
│   ├── shared/       # Código partilhado entre os módulos
│   ├── textures/
│   └── CMakeLists.txt
├── build/            # Diretório onde os binários são compilados
├── libs/             # Biblioteca RapidXML
├── toolkits/         # Ficheiros adicionais e dependências (GLUT, GLEW, DevIL)
├── CMakeLists.txt    # Configuração principal do CMake
└── README.md
```

## 🚀 Compilação

### 🐧 Linux / Mac
```sh
$ cmake .
$ make
```

### 🪟 Windows
1. Aceda ao terminal do `cmake`
2. Selecione a pasta principal e a respetiva pasta `build`
3. Configure o projeto com as definições adequadas (Usar "Win 32")
4. Selecione a pasta `toolkits` na raiz do projeto
5. Configure e gere o projeto
6. Após o Generate, execute o seguinte comando para compilar:

```sh
cmake --build build --config Release
```
Isto irá gerar os executáveis dentro da pasta `build/`.

## 🖌 Generator
Gera ficheiros de modelo (`.3d`) contendo os vértices das primitivas gráficas, bem como modelos gerados a partir de patches de Bézier.

Para gerar um modelo de primitiva, utilize o **generator**.
Exemplo para gerar um plano:

```sh
# Linux / Mac
$ cd build
$ ./generator plane 2 3 plane_2_3.3d

# Windows
$ cd build\src\generator\Release
$ .\generator.exe plane 2 3 "..\..\..\..\src\models\plane_2_3.3d"
```

**Lista de figuras e inputs**:
1. `generator plane <length> <divisions> <output>`
2. `generator box <length> <divisions> <output>`
3. `generator sphere <radius> <slices> <stacks> <output>`
4. `generator cone <radius> <height> <slices> <stacks> <output>`
5. `generator bezier <patch_file> <tessellation> <output>`

> Nota: Todos os ficheiros gerados pelo **generator** devem ser criados/movidos para a pasta `src/models/` para que o Engine os encontre.

### 🌌 Guia Rápido: Sistema Solar Dinâmico
Para visualizar a cena de demonstração principal (`solar_system4.xml`), é necessário gerar os modelos base que esta utiliza. Estando no diretório raiz do projeto (`CG`), execute no seu terminal (ajuste o caminho se não estiver em Windows):

# 1. Gerar as esferas
$ .\build\src\generator\Release\generator.exe sphere 1 20 20 src\models\sphere_1_20_20.3d
$ .\build\src\generator\Release\generator.exe sphere 1 10 10 src\models\sphere_1_10_10.3d
$ .\build\src\generator\Release\generator.exe sphere 1 8 8 src\models\sphere_1_8_8.3d

# 2. Gerar a caixa e o cone (cintura de asteróides)
$ .\build\src\generator\Release\generator.exe box 1 1 src\models\box_1_1.3d
$ .\build\src\generator\Release\generator.exe cone 1 2 4 3 src\models\cone_1_2_4_3.3d

# 3. Gerar o cometa (teapot de Bézier)
$ .\build\src\generator\Release\generator.exe bezier src\models\teapot.patch 10 src\models\bezier_10.3d

Após gerar estes ficheiros, a cena do Sistema Solar está pronta a ser executada no motor gráfico (ver secção abaixo).

## 🎮 Engine
Carrega ficheiros XML contendo a configuração da cena e renderiza os modelos gerados utilizando **Vertex Buffer Objects (VBOs)**.

Para visualizar uma cena, execute o engine. Por exemplo:

```sh
# Linux / Mac
$ cd build
$ ./engine ../src/scenes/solar_system.xml

# Windows
$ cd build\src\engine\Release
$ .\engine.exe ..\..\..\..\src\scenes\solar_system.xml
```

> Nota: Todas as cenas utilizadas poderão ser encontradas na pasta `src/scenes/`.

## ⌨️ Controlos do Engine

| Tecla | Ação |
|-------|------|
| ← / → | Rodar câmara horizontalmente |
| ↑ / ↓ | Rodar câmara verticalmente |
| `i` | Zoom In |
| `o` | Zoom Out |
| `a` | Ativar/desativar eixos |
| `m` | Alternar modo de renderização (wireframe / pontos / preenchido) |
| `c` | Ativar/desativar a visualização das curvas de Catmull-Rom |
| `r` | Reiniciar posição da câmara |
| `n` | Ativar/desativar visualização das normais |

## ✍️ Desenvolvido por

- [Jorge Rafael Machado Fernandes](https://github.com/rafafernandes888) (A104168)
- [Diogo Teixeira Fernandes](https://github.com/diogo7fernandes) (A104260)
- [Filipe Teixeira Viana](https://github.com/ftviana) (A104361)