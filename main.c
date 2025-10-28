#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h> // Para abs() e qsort()
#include <math.h>   // Para round() ou floor()
#include <stdbool.h> // Para bool, true, false

// Constantes da tela
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

/**
 * @brief Desenha uma linha usando o algoritmo do Ponto Médio (Bresenham otimizado).
 * * @param renderer O renderizador SDL.
 * @param x0 Coordenada x inicial.
 * @param y0 Coordenada y inicial.
 * @param x1 Coordenada x final.
 * @param y1 Coordenada y final.
 */
void draw_line_midpoint(SDL_Renderer* renderer, int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1; // Direção do incremento em x
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1; // Direção do incremento em y
    int err = dx + dy; // Variável de decisão (erro)
    int e2; // Erro multiplicado por 2

    while (1) {
        SDL_RenderDrawPoint(renderer, x0, y0); // Desenha o pixel atual

        // Verifica se alcançou o ponto final
        if (x0 == x1 && y0 == y1) {
            break;
        }

        e2 = 2 * err;

        // Verifica se é hora de incrementar x
        if (e2 >= dy) {
            if (x0 == x1) break; // Evita ultrapassar o limite em retas verticais
            err += dy;
            x0 += sx;
        }
        
        // Verifica se é hora de incrementar y
        if (e2 <= dx) {
            if (y0 == y1) break; // Evita ultrapassar o limite em retas horizontais
            err += dx;
            y0 += sy;
        }
    }
}

// Constantes para os 'outcodes' (códigos de região)
const int INSIDE = 0; // 0000
const int LEFT   = 1; // 0001
const int RIGHT  = 2; // 0010
const int BOTTOM = 4; // 0100
const int TOP    = 8; // 1000

// Janela de recorte (exemplo: 100, 100 a 500, 400)
const int X_MIN = 100;
const int X_MAX = 500;
const int Y_MIN = 100;
const int Y_MAX = 400;

/**
 * @brief Calcula o 'outcode' de 4 bits para um ponto (x, y) 
 * relativo à janela de recorte.
 */
int compute_outcode(double x, double y) {
    int code = INSIDE;

    if (x < X_MIN)
        code |= LEFT;
    else if (x > X_MAX)
        code |= RIGHT;

    if (y < Y_MIN)
        code |= BOTTOM;
    else if (y > Y_MAX)
        code |= TOP;

    return code;
}

/**
 * @brief Recorta e desenha uma linha (x0, y0) - (x1, y1) usando o 
 * algoritmo Cohen-Sutherland.
 * * @param renderer O renderizador SDL.
 * @param x0 Coordenada x inicial.
 * @param y0 Coordenada y inicial.
 * @param x1 Coordenada x final.
 * @param y1 Coordenada y final.
 */
void clip_and_draw_line(SDL_Renderer* renderer, double x0, double y0, double x1, double y1) {
    // Calcula os outcodes dos pontos inicial e final
    int outcode0 = compute_outcode(x0, y0);
    int outcode1 = compute_outcode(x1, y1);
    bool accept = false;

    while (1) {
        if (!(outcode0 | outcode1)) {
            // Aceite trivial: Ambos os pontos estão dentro da janela
            accept = true;
            break;
        } else if (outcode0 & outcode1) {
            // Rejeição trivial: Ambos os pontos estão fora na mesma região
            break;
        } else {
            // Recorte necessário
            double x, y;
            
            // Pega o ponto que está fora
            int outcodeOut = outcode0 ? outcode0 : outcode1;

            // Encontra a interseção
            if (outcodeOut & TOP) {           // Ponto está acima
                if (y1 != y0) {
                    x = x0 + (x1 - x0) * (Y_MAX - y0) / (y1 - y0);
                } else {
                    // Degenerate: horizontal line. Keep x as the outside point's x.
                    x = x0;
                }
                y = Y_MAX;
            } else if (outcodeOut & BOTTOM) { // Ponto está abaixo
                if (y1 != y0) {
                    x = x0 + (x1 - x0) * (Y_MIN - y0) / (y1 - y0);
                } else {
                    x = x0;
                }
                y = Y_MIN;
            } else if (outcodeOut & RIGHT) {  // Ponto está à direita
                if (x1 != x0) {
                    y = y0 + (y1 - y0) * (X_MAX - x0) / (x1 - x0);
                } else {
                    y = y0;
                }
                x = X_MAX;
            } else if (outcodeOut & LEFT) {   // Ponto está à esquerda
                if (x1 != x0) {
                    y = y0 + (y1 - y0) * (X_MIN - x0) / (x1 - x0);
                } else {
                    y = y0;
                }
                x = X_MIN;
            }

            // Atualiza o ponto que estava fora para o ponto de interseção
            if (outcodeOut == outcode0) {
                x0 = x;
                y0 = y;
                outcode0 = compute_outcode(x0, y0);
            } else {
                x1 = x;
                y1 = y;
                outcode1 = compute_outcode(x1, y1);
            }
        }
    }

    if (accept) {
        // Desenha a linha recortada (convertendo para int)
        draw_line_midpoint(renderer, (int)round(x0), (int)round(y0), (int)round(x1), (int)round(y1));
    }
}

/**
 * @brief Desenha o contorno de um polígono.
 * * @param renderer O renderizador SDL.
 * @param vertices Um array de SDL_Point (vértices).
 * @param num_vertices O número de vértices no array.
 */
void draw_polygon(SDL_Renderer* renderer, SDL_Point vertices[], int num_vertices) {
    if (num_vertices < 3) {
        return; // Não é um polígono
    }

    for (int i = 0; i < num_vertices; ++i) {
        // Pega o ponto atual e o próximo ponto (com 'wrap-around')
        SDL_Point p1 = vertices[i];
        SDL_Point p2 = vertices[(i + 1) % num_vertices]; // O % garante a volta ao início

        draw_line_midpoint(renderer, p1.x, p1.y, p2.x, p2.y);
    }
}

// Função de comparação para qsort (necessária para ordenar as interseções)
int compare_doubles(const void* a, const void* b) {
    double da = *(const double*)a;
    double db = *(const double*)b;
    return (da > db) - (da < db);
}

/**
 * @brief Preenche um polígono usando o algoritmo Scanline.
 * * @param renderer O renderizador SDL.
 * @param vertices Um array de SDL_Point (vértices).
 * @param num_vertices O número de vértices no array.
 */
void fill_polygon_scanline(SDL_Renderer* renderer, SDL_Point vertices[], int num_vertices) {
    if (num_vertices < 3) return;

    // 1. Encontra os limites Y (min e max) do polígono
    int y_min = vertices[0].y;
    int y_max = vertices[0].y;
    for (int i = 1; i < num_vertices; ++i) {
        if (vertices[i].y < y_min) y_min = vertices[i].y;
        if (vertices[i].y > y_max) y_max = vertices[i].y;
    }

    // Array para armazenar as interseções X em cada linha de varredura
    double* intersections = (double*)malloc(num_vertices * sizeof(double));
    if (!intersections) {
        printf("Falha ao alocar memoria para scanline.\n");
        return;
    }

    // 2. Itera por cada linha de varredura (scanline) de y_min até y_max
    for (int y = y_min; y <= y_max; ++y) {
        int num_intersections = 0;

        // 3. Encontra todas as interseções X para a linha 'y' atual
        for (int i = 0; i < num_vertices; ++i) {
            SDL_Point p1 = vertices[i];
            SDL_Point p2 = vertices[(i + 1) % num_vertices];

            // Verifica se a aresta (p1, p2) cruza a linha de varredura 'y'
            if (((p1.y <= y && p2.y > y) || (p1.y > y && p2.y <= y))) {
                // Evita divisão por zero (arestas horizontais são ignoradas pela lógica)
                if (p1.y == p2.y) continue; 
                
                // Calcula o X da interseção usando interpolação linear
                double x_intersect = (double)(y - p1.y) * (p2.x - p1.x) / (double)(p2.y - p1.y) + p1.x;
                intersections[num_intersections++] = x_intersect;
            }
        }

        // 4. Ordena as interseções em X
        qsort(intersections, num_intersections, sizeof(double), compare_doubles);

        // 5. Preenche os pixels entre os pares de interseção
        for (int i = 0; i < num_intersections; i += 2) {
            if (i + 1 < num_intersections) {
                int x_start = (int)round(intersections[i]);
                int x_end = (int)round(intersections[i+1]);

                // Desenha a linha horizontal (o scan)
                SDL_RenderDrawLine(renderer, x_start, y, x_end, y);
            }
        }
    }

    free(intersections);
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    // 1. Inicializa o SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL nao pode inicializar! Erro: %s\n", SDL_GetError());
        return 1;
    }

    // 2. Cria a Janela
    window = SDL_CreateWindow(
        "Trabalho 1 - CG",           // Título da janela
        SDL_WINDOWPOS_UNDEFINED,     // Posição x inicial
        SDL_WINDOWPOS_UNDEFINED,     // Posição y inicial
        SCREEN_WIDTH,                // Largura em pixels
        SCREEN_HEIGHT,               // Altura em pixels
        SDL_WINDOW_SHOWN             
    );

    if (window == NULL) {
        printf("Janela nao pode ser criada! Erro: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // 3. Cria o Renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer nao pode ser criado! Erro: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // --- Definição das formas ---
    SDL_Point triangulo[] = {
        {320, 50},  // Topo
        {120, 250}, // Canto inferior esquerdo
        {520, 250}  // Canto inferior direito
    };
    int num_vertices_tri = 3;

    SDL_Point pentagono[] = {
        {100, 300}, {200, 300}, {250, 400}, {150, 450}, {50, 400}
    };
    int num_vertices_pen = 5;


    SDL_SetWindowTitle(window, "Modo 0: Contorno do triângulo");

    // --- Loop Principal ---
    int running = 1; 
    SDL_Event e;     
    // Circular os desenhos com Enter
    int mode = 0; // 0..4
    const int NUM_MODES = 6;

    while (running) {
        // 4. Processa eventos na fila
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = 0; 
            } else if (e.type == SDL_KEYDOWN) {
                // Enter cycles to next mode; Escape quits
                if (e.key.keysym.sym == SDLK_RETURN) {
                    mode = (mode + 1) % NUM_MODES;
                    // Update window title to reflect mode
                    char title[64];
                    switch (mode) {
                        case 0: snprintf(title, sizeof(title), "Modo 0: Contorno do triângulo"); break;
                        case 1: snprintf(title, sizeof(title), "Modo 1: Triângulo preenchido"); break;
                        case 2: snprintf(title, sizeof(title), "Modo 2: Desenhar pentágono"); break;
                        case 3: snprintf(title, sizeof(title), "Modo 3: Mostrar caixa de recorte"); break;
                        case 4: snprintf(title, sizeof(title), "Modo 4: Linha recortada"); break;
                        case 5: snprintf(title, sizeof(title), "Modo 5: Linha rejeitada"); break;
                        default: snprintf(title, sizeof(title), "Modo"); break;
                    }
                    SDL_SetWindowTitle(window, title);
                } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                    running = 0;
                }
            }
        }

        // 5. Limpa a tela (com a cor preta)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // R, G, B, A (Preto)
        SDL_RenderClear(renderer);

        // 6. --- INÍCIO DOS DESENHOS ---
        
        SDL_Rect clip_rect = { X_MIN, Y_MIN, X_MAX - X_MIN, Y_MAX - Y_MIN };

        switch (mode) {
            case 0: //  // Desenhar triangulo
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                draw_polygon(renderer, triangulo, num_vertices_tri);
                break;
            case 1: // Encher Triangulo
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                fill_polygon_scanline(renderer, triangulo, num_vertices_tri);
                break;
            case 2: // Desenhar polígono (Pentagono)
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                draw_polygon(renderer, pentagono, num_vertices_pen);
                break;
            case 3: // Caixa para fazer o clip
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                SDL_RenderDrawRect(renderer, &clip_rect);
                break;
            case 4: // Linha com o clip
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                SDL_RenderDrawRect(renderer, &clip_rect);
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                clip_and_draw_line(renderer, 50.0, 50.0, 600.0, 450.0);
                break;
            case 5: // Linha feita com o clip fora da caixa
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                SDL_RenderDrawRect(renderer, &clip_rect);
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                clip_and_draw_line(renderer, 10.0, 10.0, 50.0, 400.0);
                break;
            default:
                break;
        }


        // --- FIM DOS DESENHOS ---

        // 7. Atualiza a tela
        SDL_RenderPresent(renderer);
    }

    // 8. Limpa e sai
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}