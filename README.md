# CG

Este repositório contém o projeto da Unidade Curricular de **Computação Gráfica** (CG) no ano letivo 2024/2025, desenvolvido em C++ com OpenGL. O objetivo deste projeto é desenvolver um **motor 3D**.

## 📁 Estrutura do Projeto
```
CG/
├── src/
│   ├── engine/       # Código do motor de renderização
│   ├── generator/    # Código do gerador de primitivas
│   ├── models/       # Modelos gerados em formato .3d
│   ├── scenes/       # Ficheiros XML das cenas
│   ├── shared/       # Código partilhado entre os módulos
│   └── CMakeLists.txt
├── build/            # Diretório onde os binários são compilados
├── libs/             # Biblioteca RapidXML
├── toolkits/         # Ficheiros adicionais
├── CMakeLists.txt    # Configuração principal do CMake
└── README.md
```

## 🚀 Compilação

### 🐧 Linux / Mac
```sh
$ cmake .
$ make
```

### 🪟 Windows (feito em Windows)
1. Aceda ao terminal do `cmake`
2. Selecione a pasta principal e a respetiva pasta `build`
3. Configure o projeto com as definições adequadas (Usar "Win 32")
4. Selecione a pasta `toolkits` na raiz do projeto
5. Configure e gere o projeto

Isto irá gerar os executáveis dentro da pasta `build/`.

## 🖌 Generator
Gera ficheiros de modelo (`.3d`) contendo os vértices das primitivas gráficas.

Para gerar um modelo de primitiva, utilize o **generator**.
Exemplo para gerar um plano:

```sh
# Linux
$ cd build
$ ./generator plane 2 3 plane_2_3.3d

# Windows
$ cd build\src\generator\Debug
$ .\generator.exe plane 2 3 "..\..\..\..\src\models\plane_2_3.3d"
```

**Lista de figuras e inputs**:
1. `generator plane <length> <divisions> <output>`
2. `generator box <length> <divisions> <output>`
3. `generator sphere <radius> <slices> <stacks> <output>`
4. `generator cone <radius> <height> <slices> <stacks> <output>`

> Nota: Todos os ficheiros gerados pelo **generator** serão criados na pasta `models`.

## 🎮 Engine
Carrega ficheiros XML contendo a configuração da cena e renderiza os modelos gerados.

Para visualizar uma cena, execute o engine. Por exemplo:

```sh
# Linux
$ cd build
$ ./engine ../src/scenes/test.xml

# Windows
$ cd build\src\engine\Debug
$ .\engine.exe ..\..\..\..\src\scenes\test.xml
```

Isto irá carregar e renderizar a cena descrita no XML.

> Nota: Todas as cenas utilizadas poderão ser encontradas na pasta `scenes`.

## ⌨️ Controlos do Engine

| Tecla | Ação |
|-------|------|
| ← / → | Rodar câmara horizontalmente |
| ↑ / ↓ | Rodar câmara verticalmente |
| `i` | Zoom In |
| `o` | Zoom Out |
| `a` | Ativar/desativar eixos |
| `m` | Alternar modo de renderização (wireframe / pontos / preenchido) |
| `r` | Reiniciar posição da câmara |

## ✍️ Desenvolvido por

- [Jorge Rafael Machado Fernandes](https://github.com/rafafernandes888)

- [Diogo Teixeira Fernandes](https://github.com/diogo7fernandes)

- [Filipe Teixeira Viana](https://github.com/ftviana)