# Projeto CG-Trabalho1_REMAKE

Este repositório contém um programa em C que usa SDL2 (`main.c`) com implementações dos seguintes algoritmos gráficos (todos dentro de `main.c`):

- Algoritmo do Ponto Médio (Bresenham otimizado) para desenhar retas
- Algoritmo de recorte Cohen–Sutherland para linhas
- Desenho de contorno de polígonos (usando linhas)
- Preenchimento de polígonos por Scanline

O programa é interativo: pressione Enter dentro da tela para ciclar entre modos e Esc para sair. Os modos (na ordem) são:

- Modo 0: Contorno do triângulo (inicial)
- Modo 1: Triângulo preenchido
- Modo 2: Desenhar pentágono (contorno)
- Modo 3: Mostrar caixa de recorte
- Modo 4: Linha recortada (com a caixa)
- Modo 5: Linha rejeitada (com a caixa)

---

## Como compilar e executar `main.c` (Windows / PowerShell)

Pré-requisitos
- MinGW (gcc) ou outro compilador C para Windows.
- Bibliotecas de desenvolvimento SDL2 (headers, libs e a `SDL2.dll`).
- `mingw32-make` ou `make` (o Makefile já presente usa `gcc`).

Instruções rápidas
1. Abra o PowerShell na pasta do projeto (onde está o `Makefile`).
2. Rode (se estiver usando MinGW no Windows):

```powershell
mingw32-make
```

ou, se preferir e tiver `make` disponível:

```powershell
make
```

3. Execute o binário gerado:

```powershell
.\main.exe
```

Observações
- Se aparecer erro sobre `SDL2.dll` não encontrado, copie `SDL2.dll` para o diretório do `main.exe` ou adicione o diretório da DLL ao `PATH` do Windows.
- O `Makefile` do repositório usa `-Isrc/Include -Lsrc/lib` e vincula `-lmingw32 -lSDL2main -lSDL2`. Garanta que os arquivos `.lib`/`.a` e headers estão corretos para seu ambiente.

---

## Projeto Java (instruções genéricas)

A seguir temos instruções genéricas caso você queira compilar/rodar um projeto Java.

Compilar/rodar um único arquivo Java (JDK instalado):

```powershell

# Executar
java ProgramaDesenho.java
```

## Dicas e ajustes rápidos

- Para mudar a resolução da janela, altere `SCREEN_WIDTH` e `SCREEN_HEIGHT` no topo de `main.c`.
- Para ajustar a janela de recorte, edite `X_MIN`, `X_MAX`, `Y_MIN` e `Y_MAX` (definidos perto das funções de recorte).
- Para ver mensagens de debug, você pode adicionar `printf` em pontos-chave do código e recompilar.

## Limitações

Tive dificuldades para entender o recorte de primitivas (Cohen Sutherland), então tive que usar IA para poder realizar essa atividade, toda aquela parte de output code e como aplicar para a linha.