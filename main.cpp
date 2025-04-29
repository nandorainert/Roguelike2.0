// bibliotecas...
#include <iostream>
#include <cctype>
#include <string>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <conio.h>

using namespace std;

// Constantes
#define TAM 40
#define VISIBILIDADE 5
const char PAREDE = (char)219;

// Estruturas
struct Jogador {
    int x, y;
    int vida;
    int pontos;
    int dano;
    bool venceu;
};

struct Inimigo {
    int x, y;
    int vida;
    int dano;
};

// Funções utilitárias
void gotoxy(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void limparTela() {
    gotoxy(0, 0);
    for (int i = 0; i < TAM + 5; i++) {
        cout << string(80, ' ') << endl;
    }
    gotoxy(0, 0);
}

char getInput() {
    while (true) {
        if (_kbhit()) {
            return tolower(_getch());
        }
    }
}

void pausar() {
    cout << "\nPressione Enter para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Funções do jogo
void mostrarMapa(char mat[TAM][TAM], Jogador jogador) {
    gotoxy(0, 0);
    for (int i = 0; i < TAM; i++) {
        for (int j = 0; j < TAM; j++) {
            if (abs(i - jogador.x) <= VISIBILIDADE && abs(j - jogador.y) <= VISIBILIDADE) {
                if (i == jogador.x && j == jogador.y) cout << "@ ";
                else cout << mat[i][j] << ' ';
            } else cout << PAREDE << ' ';
        }
        cout << endl;
    }
    cout << "\nVida: " << jogador.vida << " | Pontos: " << jogador.pontos << " | Dano: " << jogador.dano << endl;
}

void verificarInimigosProximos(Jogador &jogador, Inimigo inimigos[], int numInimigos) {
    for (int i = 0; i < numInimigos; i++) {
        if (inimigos[i].vida > 0 && abs(jogador.x - inimigos[i].x) <= 2 && abs(jogador.y - inimigos[i].y) <= 2) {
            cout << "\nInimigo proximo! Vida: " << inimigos[i].vida << " | Dano: " << inimigos[i].dano << endl;
            limparTela();
        }
    }
}

void fazerInimigoFugir(Jogador jogador, Inimigo &inimigo, char mat[TAM][TAM]) {
    int dirX = 0, dirY = 0;
    if (jogador.x < inimigo.x) dirX = 1;
    else if (jogador.x > inimigo.x) dirX = -1;
    if (jogador.y < inimigo.y) dirY = 1;
    else if (jogador.y > inimigo.y) dirY = -1;

    int novoX = inimigo.x + dirX;
    int novoY = inimigo.y + dirY;
    if (novoX >= 0 && novoX < TAM && novoY >= 0 && novoY < TAM && mat[novoX][novoY] == '.') {
        mat[inimigo.x][inimigo.y] = '.';
        inimigo.x = novoX;
        inimigo.y = novoY;
        mat[inimigo.x][inimigo.y] = 'X';
    }
}

void fazerBossFugir(Jogador jogador, Inimigo &boss, char mat[TAM][TAM]) {
    int dirX = 0, dirY = 0;
    if (jogador.x < boss.x) dirX = 1;
    else if (jogador.x > boss.x) dirX = -1;
    if (jogador.y < boss.y) dirY = 1;
    else if (jogador.y > boss.y) dirY = -1;

    int novoX = boss.x + dirX;
    int novoY = boss.y + dirY;
    if (novoX >= 0 && novoX < TAM && novoY >= 0 && novoY < TAM && mat[novoX][novoY] == '.') {
        mat[boss.x][boss.y] = '.';
        boss.x = novoX;
        boss.y = novoY;
        mat[boss.x][boss.y] = 'B';
    }
}

void mover(char direcao, Jogador &jogador, char mat[TAM][TAM], Inimigo inimigos[], int numInimigos, Inimigo &boss) {
    int novoX = jogador.x, novoY = jogador.y;
    switch (direcao) {
        case 'w': novoX--; break;
        case 's': novoX++; break;
        case 'a': novoY--; break;
        case 'd': novoY++; break;
        default: return;
    }

    if (novoX < 0 || novoX >= TAM || novoY < 0 || novoY >= TAM || mat[novoX][novoY] == PAREDE) {
        return;
    }

    for (int i = 0; i < numInimigos; i++) {
        if (inimigos[i].vida > 0 && inimigos[i].x == novoX && inimigos[i].y == novoY) {
            cout << "\nVoce encontrou um inimigo! Deseja atacar? (s/n): ";
            char escolha;
            cin >> escolha;
            escolha = tolower(escolha);

            if (escolha == 's') {
                inimigos[i].vida -= jogador.dano;
                if (inimigos[i].vida <= 0) {
                    limparTela();
                    cout << "Inimigo derrotado!\n";
                    mat[inimigos[i].x][inimigos[i].y] = '.';
                    jogador.pontos += 20;
                } else {
                    limparTela();
                    cout << "O inimigo revida!\n";
                    jogador.vida -= inimigos[i].dano;
                    fazerInimigoFugir(jogador, inimigos[i], mat);
                }
            }
            pausar();
            return;
        }
    }

    if (mat[novoX][novoY] == 'S') {
        jogador.pontos += 10;
        jogador.dano++;
        cout << "Voce pegou uma espada!\n";
        mat[novoX][novoY] = '.';
        pausar();
    } else if (mat[novoX][novoY] == 'P') {
        jogador.vida++;
        cout << "Voce pegou uma pocao!\n";
        mat[novoX][novoY] = '.';
        pausar();
    } else if (mat[novoX][novoY] == 'B') {
        cout << "\nVoce encontrou o BOSS! Deseja atacar? (s/n): ";
        char escolha;
        cin >> escolha;
        escolha = tolower(escolha);

        if (escolha == 's') {
            if (jogador.pontos >= 100) {
                limparTela();
                cout << "Voce derrotou o BOSS! Vitoria!\n";
                jogador.venceu = true;
                mat[novoX][novoY] = '.';
            } else {
                limparTela();
                jogador.vida -= 2;
                cout << "Poucos pontos! BOSS atacou voce! -2 vida.\n";
                fazerBossFugir(jogador, boss, mat);
            }
            pausar();
        }
        return;
    }

    jogador.x = novoX;
    jogador.y = novoY;
    verificarInimigosProximos(jogador, inimigos, numInimigos);
}

void gerarMapa(char mat[TAM][TAM], Inimigo inimigos[], int &numInimigos, Inimigo &boss) {
    numInimigos = 0;
    boss = {30, 37, 15, 3};

    for (int i = 0; i < TAM; i++)
        for (int j = 0; j < TAM; j++)
            mat[i][j] = PAREDE;

    for (int i = 2; i < 12; i++)
        for (int j = 2; j < 12; j++)
            mat[i][j] = '.';

    for (int i = 14; i < 24; i++)
        for (int j = 6; j < 16; j++)
            mat[i][j] = '.';

    for (int i = 10; i < 20; i++)
        for (int j = 22; j < 32; j++)
            mat[i][j] = '.';

    for (int i = 22; i < 32; i++)
        for (int j = 30; j < 38; j++)
            mat[i][j] = '.';

    for (int i = 12; i <= 14; i++) mat[i][8] = '.';
    for (int j = 16; j <= 22; j++) mat[18][j] = '.';
    for (int i = 20; i <= 22; i++) mat[i][30] = '.';

    inimigos[numInimigos++] = {5, 5, 3, 1}; mat[5][5] = 'X';
    inimigos[numInimigos++] = {16, 8, 4, 1}; mat[16][8] = 'X';
    inimigos[numInimigos++] = {12, 28, 5, 2}; mat[12][28] = 'X';
    inimigos[numInimigos++] = {28, 34, 6, 2}; mat[28][34] = 'X';
    mat[30][37] = 'B';
}

void espalharItens(char mat[TAM][TAM], int quantidade) {
    int colocados = 0;
    while (colocados < quantidade) {
        int i = rand() % TAM;
        int j = rand() % TAM;
        if (mat[i][j] == '.') {
            mat[i][j] = (rand() % 2 == 0) ? 'S' : 'P';
            colocados++;
        }
    }
}

void exibirMenu() {
    system("cls");
    cout << "1 - Jogar\n";
    cout << "2 - Creditos\n";
    cout << "3 - Como Jogar\n";
    cout << "4 - Sair\n";
    cout << "Escolha uma opcao: ";
}

void exibirCreditos() {
    system("cls");
    cout << "\nCreditos:\n";
    cout << "Desenvolvedores:\n Emanuel Rudnick\n Ferdinando Rainert\n Juliana Varela\n";
    cout << "Jogo: RPG em C++\n";
    pausar();
}

void exibirTutorial() {
    system("cls");
    cout << "- Use W A S D para mover o jogador (@);\n";
    cout << "- Inimigos (X) tem vida e causam dano;\n";
    cout << "- Itens: S (espada), P (pocao);\n";
    cout << "- Venca o BOSS com 100+ pontos!\n";
    cout << "- Ao atacar (s), o inimigo tentara fugir!\n";
    pausar();
}

int main() {

    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(out, &cursorInfo);

    SMALL_RECT rect = {0, 0, TAM, TAM + 10};
    SetConsoleWindowInfo(out, TRUE, &rect);

    COORD size = {TAM, TAM + 10};
    SetConsoleScreenBufferSize(out, size);

    srand(time(0));

    char mat[TAM][TAM];
    Inimigo inimigos[10];
    Inimigo boss;
    int numInimigos = 0;

    while (true) {
        exibirMenu();
        char opcao = getInput();

        switch (opcao) {
            case '1': {
                Jogador jogador = {5, 5, 10, 0, 2, false};
                gerarMapa(mat, inimigos, numInimigos, boss);
                espalharItens(mat, 10);

                while (jogador.vida > 0 && !jogador.venceu) {
                    mostrarMapa(mat, jogador);
                    cout << "\nUse WASD para se mover (Q para sair): ";
                    char comando = getInput();
                    if (comando == 'q') {
                        cout << "\nVoce saiu do jogo.\n";
                        break;
                    }
                    mover(comando, jogador, mat, inimigos, numInimigos, boss);
                }

                system("cls");
                if (jogador.venceu) {
                    cout << "Voce venceu o jogo! Parabens!\n";
                } else if (jogador.vida > 0) {
                    cout << "Jogo interrompido. Voce nao perdeu.\n";
                } else {
                    cout << "Game Over! Voce perdeu.\n";
                }
                pausar();
                break;
            }
            case '2':
                exibirCreditos();
                break;
            case '3':
                exibirTutorial();
                break;
            case '4':
                return 0;
            default:
                cout << "Opcao invalida! Tente novamente.\n";
                break;
        }
    }

    return 0;
}