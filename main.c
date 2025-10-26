#include <SDL2/SDL.h>
#include <stdio.h> // Para imprimir erros, se houver

// Constantes da tela
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    // 1. Inicializa o SDL (subsistema de vídeo)
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL nao pode inicializar! Erro: %s\n", SDL_GetError());
        return 1;
    }

    // 2. Cria a Janela
    window = SDL_CreateWindow(
        "Pixel na Tela",             // Título da janela
        SDL_WINDOWPOS_UNDEFINED,     // Posição x inicial
        SDL_WINDOWPOS_UNDEFINED,     // Posição y inicial
        SCREEN_WIDTH,                // Largura em pixels
        SCREEN_HEIGHT,               // Altura em pixels
        SDL_WINDOW_SHOWN             // Flags (apenas mostrar a janela)
    );

    if (window == NULL) {
        printf("Janela nao pode ser criada! Erro: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // 3. Cria o "Renderer" (o que desenha na janela)
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer nao pode ser criado! Erro: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // --- Loop Principal ---
    int running = 1; // Flag para manter o loop rodando
    SDL_Event e;     // Para lidar com eventos (como fechar a janela)

    while (running) {
        // 4. Processa eventos na fila
        while (SDL_PollEvent(&e) != 0) {
            // Se o usuário clicar no 'X' da janela
            if (e.type == SDL_QUIT) {
                running = 0; // Para o loop
            }
        }

        // 5. Limpa a tela (com a cor preta)
        // Define a cor do "pincel" para preto
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // R, G, B, A (Preto)
        // Pinta a tela inteira com essa cor
        SDL_RenderClear(renderer);

        // 6. Desenha o pixel
        // Define a cor do "pincel" para branco
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // R, G, B, A (Branco)
        // Desenha um ponto (pixel) na coordenada (x=100, y=100)
        SDL_RenderDrawPoint(renderer, 100, 100);

        // 7. Atualiza a tela
        // O SDL desenha em um "buffer" escondido. Esta função mostra o que foi desenhado.
        SDL_RenderPresent(renderer);
    }

    // 8. Limpa e sai
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}