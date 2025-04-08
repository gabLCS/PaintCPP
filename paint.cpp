/*
 * Computacao Grafica
 * Codigo Exemplo: Rasterizacao de Segmentos de Reta com GLUT/OpenGL
 * Autor: Prof. Laurindo de Sousa Britto Neto
 */

// Bibliotecas utilizadas pelo OpenGL
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <windows.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <forward_list>
#include <limits>
#include <queue>
#include "glut_text.h"

#define M_PI 3.14159265358979323846

using namespace std;

// Variaveis Globais
#define ESC 27
int raio_circunferencia = 0;

//Enumeracao com os tipos de formas geometricas
enum tipo_forma { LIN = 1, TRI, RET, POL, CIR, QUAD, TRANSLACAO, ESCALA, CISALHAMENTO, REFLEXAO, ROTACAO, FF }; // Linha, Triangulo, Retangulo Poligono, Circulo, Quadrilatero

//Verifica se foi realizado o primeiro clique do mouse
bool click1 = false;
bool click2 = false;

//Coordenadas da posicao atual do mouse
int m_x, m_y;

//Coordenadas do primeiro clique e do segundo clique do mouse
int x_1, y_1, x_2, y_2, x_3, y_3;

//Indica o tipo de forma geometrica ativa para desenhar
int modo = LIN;

//Largura e altura da janela
int width = 512, altura = 512;

// Definicao de vertice
struct vertice
{
	int x;
	int y;
};

// Definicao das formas geometricas
struct forma
{
	int tipo;
	forward_list<vertice> v; //lista encadeada de vertices
};

// Lista encadeada de formas geometricas
forward_list<forma> formas;
forward_list<vertice> vertices_temp;

// Funcao para armazenar uma forma geometrica na lista de formas
// Armazena sempre no inicio da lista
void pushForma(int tipo)
{
	forma f;
	f.tipo = tipo;
	formas.push_front(f);
}

// Funcao para armazenar um vertice na forma do inicio da lista de formas geometricas
// Armazena sempre no inicio da lista
void pushVertice(int x, int y)
{
	vertice v;
	v.x = x;
	v.y = y;
	formas.front().v.push_front(v);
}

//Fucao para armazenar uma Linha na lista de formas geometricas
void pushLinha(int x1, int y1, int x2, int y2)
{
	pushForma(LIN);
	pushVertice(x1, y1);
	pushVertice(x2, y2);
}
// Funcao para armazenar um circulo na lista de formas geometricas
void pushCirc(int x, int y, int x2, int y2)
{
	pushForma(CIR);
	// Calcula o raio do círculo
	int distancia_x = x2 - x;
	int distancia_y = y2 - y;
	int raio = sqrt(distancia_x * distancia_x + distancia_y * distancia_y);

	// Armazena o centro do círculo como um vértice
	pushVertice(x, y);
	// Armazena o raio do círculo como um vértice adicional
	pushVertice(x2, y2);
}
// Funcao para armazenar um Quadrilatero na lista de formas geometricas
void pushQuadrilatero(int x1, int y1, int x2, int y2)
{
	pushForma(QUAD);
	pushVertice(x1, y1); // Top left
	pushVertice(x2, y1); // Top right
	pushVertice(x2, y2); // Bottom right
	pushVertice(x1, y2); // Bottom left
}
void pushTriangulo(int x1, int y1, int x2, int y2, int x3, int y3)
{
	pushForma(TRI);
	pushVertice(x1, y1);
	pushVertice(x2, y2);
	pushVertice(x3, y3);
}
float distancia(int x1, int y1, int x2, int y2)
{
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}
vertice encontrarVerticeMaisProximo(const vertice& ultimo, const forward_list<vertice>& vertices)
{
	vertice maisProximo = vertices.front();
	float menorDistancia = distancia(ultimo.x, ultimo.y, maisProximo.x, maisProximo.y);

for (const auto & v : vertices)
	{
		float dist = distancia(ultimo.x, ultimo.y, v.x, v.y);
		if (dist < menorDistancia && !(v.x == ultimo.x && v.y == ultimo.y))
		{
			maisProximo = v;
			menorDistancia = dist;
		}
	}

	return maisProximo;
}


// Funções de transformação

void translacaoGlobal(int dx, int dy)
{
for (auto & f : formas)
	{
for (auto & v : f.v)
		{
			v.x += dx;
			v.y += dy;
		}
	}
}

// Função para realizar a escala de todas as formas geométricas
void escalaGlobal(float sx, float sy)
{
for (auto & f : formas)
	{
for (auto & v : f.v)
		{
			printf("%d, %d\n", v.x, v.y);
			v.x *= sx;
			v.y *= sy;
		}
	}
}

// Função para realizar o cisalhamento de todas as formas geométricas
void cisalhamentoGlobal(float shx, float shy)
{
for (auto & f : formas)
	{
for (auto & v : f.v)
		{
			float x_original = v.x;
			v.x += shx * v.y;
			v.y += shy * x_original;
		}
	}
}

// Função para realizar a reflexão de todas as formas geométricas
void reflexaoGlobal(bool refletirX, bool refletirY)
{
	// Guardar as coordenadas originais
	int transX = 0, transY = 0;

	// Iterar sobre todas as formas geométricas
for (auto & f : formas)
	{
		// Iterar sobre todos os vértices da forma atual
for (auto & v : f.v)
		{
			// Salvar as coordenadas originais do primeiro vértice
			if (transX == 0 && transY == 0)
			{
				transX = v.x;
				transY = v.y;
			}
			// Verificar se é para refletir em relação ao eixo x
			if (refletirX)
			{
				v.x = -v.x;
			}
			// Verificar se é para refletir em relação ao eixo y
			if (refletirY)
			{
				v.y = -v.y;
			}
		}
	}
	if (refletirX)
	{
		translacaoGlobal(transX * 2, 0);
	}
	if (refletirY)
	{
		translacaoGlobal(0, transY * 2);
	}

}

// Função para realizar a rotação de todas as formas geométricas
void rotacaoGlobal(float angulo)
{
	float radianos = angulo * M_PI / 180.0;
	float cos_theta = cos(radianos);
	float sen_theta = sin(radianos);

for (auto & f : formas)
	{
for (auto & v : f.v)
		{
			float x_original = v.x;
			float y_original = v.y;
			v.x = x_original * cos_theta - y_original * sen_theta;
			v.y = x_original * sen_theta + y_original * cos_theta;
		}
	}
}

/*
 * Declaracoes antecipadas (forward) das funcoes (assinaturas das funcoes)
 */
void init(void);
void reshape(int w, int h);
void display(void);
void menu_popup(int value);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void mousePassiveMotion(int x, int y);
void drawPixel(int x, int y);
// Funcao que percorre a lista de formas geometricas, desenhando-as na tela
void drawFormas();
// Funcao que implementa o Algoritmo Imediato para rasterizacao de segmentos de retas
void retaBresenham(double x1, double y1, double x2, double y2);
void circunferenciaBresenham(int x_centro, int y_centro, int raio);
void floodFill4(int x, int y, unsigned char fillColor[3], unsigned char targetColor[3]);

/*
 * Funcao principal
 */
int main(int argc, char** argv)
{
	glutInit(&argc, argv); // Passagens de parametro C para o glut
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); //Selecao do Modo do Display e do Sistema de cor
	glutInitWindowSize(width, altura);  // Tamanho da janela do OpenGL
	glutInitWindowPosition(100, 100); //Posicao inicial da janela do OpenGL
	glutCreateWindow("Computacao Grafica: Paint"); // Da nome para uma janela OpenGL
	init(); // Chama funcao init();
	glutReshapeFunc(reshape); //funcao callback para redesenhar a tela
	glutKeyboardFunc(keyboard); //funcao callback do teclado
	glutMouseFunc(mouse); //funcao callback do mouse
	glutPassiveMotionFunc(mousePassiveMotion); //fucao callback do movimento passivo do mouse
	glutDisplayFunc(display); //funcao callback de desenho

	// Define o menu pop-up
	glutCreateMenu(menu_popup);
	glutAddMenuEntry("Linha", LIN);
	glutAddMenuEntry("Quadrilatero", QUAD);
	glutAddMenuEntry("Triangulo", TRI);
	glutAddMenuEntry("Poligono", POL);
	glutAddMenuEntry("Circulo", CIR);
	glutAddMenuEntry("Translacao", TRANSLACAO);
	glutAddMenuEntry("Escala", ESCALA);
	glutAddMenuEntry("Cisalhamento", CISALHAMENTO);
	glutAddMenuEntry("Reflexao", REFLEXAO);
	glutAddMenuEntry("Rotacao", ROTACAO);
	glutAddMenuEntry("FloodFill", FF);
	glutAddMenuEntry("Sair", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop(); // executa o loop do OpenGL
	return EXIT_SUCCESS; // retorna 0 para o tipo inteiro da funcao main();
}

/*
 * Inicializa alguns parametros do GLUT
 */
void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0); //Limpa a tela com a cor branca;
}

/*
 * Ajusta a projecao para o redesenho da janela
 */
void reshape(int w, int h)
{
	// Muda para o modo de projecao e reinicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Definindo o Viewport para o tamanho da janela
	glViewport(0, 0, w, h);

	width = w;
	altura = h;
	glOrtho(0, w, 0, h, -1, 1);

	// muda para o modo de desenho
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/*
 * Controla os desenhos na tela
 */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT); //Limpa o buffer de cores e reinicia a matriz
	glColor3f(0.0, 0.0, 0.0); // Seleciona a cor default como preto
	drawFormas(); // Desenha as formas geometricas da lista
	//Desenha texto com as coordenadas da posicao do mouse
	draw_text_stroke(0, 0, "(" + to_string(m_x) + "," + to_string(m_y) + ")", 0.2);
	glutSwapBuffers(); // manda o OpenGl renderizar as primitivas
}

/*
 * Controla o menu pop-up
 */
void menu_popup(int value)
{
	if (value == 0) exit(EXIT_SUCCESS);
	modo = value;
}

/*
 * Controle das teclas comuns do teclado
 */
void keyboard(unsigned char key, int x, int y)
{
	switch (key)   // key - variavel que possui valor ASCII da tecla precionada
	{
	case ESC:
		exit(EXIT_SUCCESS);
		break;
	}
}

/*
 * Controle dos botoes do mouse
 */
void mouse(int button, int state, int x, int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		switch (modo)
		{
		case LIN:
			if (state == GLUT_DOWN)
			{
				if (click1)
				{
					x_2 = x;
					y_2 = altura - y - 1;
					printf("Clique 2(%d, %d)\n", x_2, y_2);
					pushLinha(x_1, y_1, x_2, y_2);
					click1 = false;
					glutPostRedisplay();
				}
				else
				{
					click1 = true;
					x_1 = x;
					y_1 = altura - y - 1;
					printf("Clique 1(%d, %d)\n", x_1, y_1);
				}
			}
			break;
		case QUAD:
			if (state == GLUT_DOWN)
			{
				if (click1)
				{
					x_2 = x;
					y_2 = altura - y - 1;
					printf("Clique 2(%d, %d)\n", x_2, y_2);
					pushLinha(x_1, y_1, x_2, y_1);
					pushLinha(x_2, y_1, x_2, y_2);
					pushLinha(x_2, y_2, x_1, y_2);
					pushLinha(x_1, y_2, x_1, y_1);
					click1 = false;
					glutPostRedisplay();
				}
				else
				{
					click1 = true;
					x_1 = x;
					y_1 = altura - y - 1;
					printf("Clique 1(%d, %d)\n", x_1, y_1);
				}
			}
			break;
		case TRI:
			if (state == GLUT_DOWN)
			{
				if (!click1)
				{
					click1 = true;
					x_1 = x;
					y_1 = altura - y - 1;
					printf("Clique 1(%d, %d)\n", x_1, y_1);
				}
				else if (!click2)
				{
					click2 = true;
					x_2 = x;
					y_2 = altura - y - 1;
					printf("Clique 2(%d, %d)\n", x_2, y_2);
				}
				else
				{
					click1 = click2 = false;
					x_3 = x;
					y_3 = altura - y - 1;
					printf("Clique 3(%d, %d)\n", x_3, y_3);
					pushTriangulo(x_1, y_1, x_2, y_2, x_3, y_3);
					glutPostRedisplay();
				}
			}
			break;
		case POL:
			if (state == GLUT_DOWN)
			{
				if (!click1)
				{
					click1 = true;
					pushForma(POL);
				}
				int x_click = x;
				int y_click = altura - y - 1;
				printf("Clique (%d, %d)\n", x_click, y_click);
				pushVertice(x_click, y_click);
				glutPostRedisplay();
			}
			break;
		case CIR:
			if (state == GLUT_DOWN)
			{
				if (click1)
				{
					x_2 = x;
					y_2 = altura - y - 1;
					printf("Clique 2(%d, %d)\n", x_2, y_2);
					int distancia_x = x_2 - x_1;
					int distancia_y = y_2 - y_1;
					raio_circunferencia = sqrt(distancia_x * distancia_x + distancia_y * distancia_y);
					pushCirc(x_1, y_1, x_2, y_2);
					glutPostRedisplay();
					click1 = false;
				}
				else
				{
					click1 = true;
					x_1 = x;
					y_1 = altura - y - 1;
					printf("Clique 1(%d, %d)\n", x_1, y_1);
				}
			}
			break;

		case TRANSLACAO:
			if (click1)
			{
				int dx, dy;
				dx = x - x_1;
				dy = (altura - y - 1) - y_1;
				translacaoGlobal(dx, dy);
				click1 = false;
				glutPostRedisplay();
			}
			else
			{
				click1 = true;
				x_1 = x;
				y_1 = altura - y - 1;
			}
			break;
		case ESCALA:
			if (click1)
			{
				float sx, sy;
				printf("Escala - sx: \n");
				scanf("%f", &sx);
				printf("Escala - sy: \n");
				scanf("%f", &sy);
				escalaGlobal(sx, sy);
				click1 = false;
				glutPostRedisplay();
			}
			else
			{
				click1 = true;
				x_1 = x;
				y_1 = altura - y - 1;
			}
			break;
		case CISALHAMENTO:
			if (click1)
			{
				float shx, shy;
				shx = (float)(x - x_1) / width;
				shy = (float)((altura - y - 1) - y_1) / altura;
				click1 = false;
				cisalhamentoGlobal(shx, shy);
				glutPostRedisplay();
			}
			else
			{
				click1 = true;
				x_1 = x;
				y_1 = altura - y - 1;
			}
			break;
		case REFLEXAO:
			if (click1)
			{
				printf("Escolha a direcao da reflexao:\n");
				printf("1. Refletir apenas no eixo x\n");
				printf("2. Refletir apenas no eixo y\n");
				printf("3. Refletir em ambos os eixos\n");
				int choice;
				scanf("%d", &choice);
				switch (choice)
				{
				case 1:
					reflexaoGlobal(true, false);
					break;
				case 2:
					reflexaoGlobal(false, true);
					break;
				case 3:
					reflexaoGlobal(true, true);
					break;
				default:
					printf("Opcao invalida.\n");
				}
			}
			else
			{
				click1 = true;
				x_1 = x;
				y_1 = altura - y - 1;
			}
			break;
		case ROTACAO:
			if (click1)
			{
				float rot;
				printf("Escolha o angulo de rotacao\n");
				scanf("%f", &rot);
				rotacaoGlobal(rot);
				click1 = false;
				glutPostRedisplay();
			}
			else
			{
				click1 = true;
				x_1 = x;
				y_1 = altura - y - 1;
			}
			break;
		case FF:
			if (state == GLUT_DOWN)
			{
				unsigned char fillColor[3] = {255, 0, 0}; // Cor de preenchimento (vermelho)
				unsigned char targetColor[3]; // Cor do pixel de origem
				glReadPixels(x, altura - y - 1, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, targetColor);
				floodFill4(x, altura - y - 1, fillColor, targetColor);
			}
			break;


		}
		break;
	}
}

/*
 * Controle da posicao do cursor do mouse
 */
void mousePassiveMotion(int x, int y)
{
	m_x = x;
	m_y = altura - y - 1;
	glutPostRedisplay();
}

/*
 * Funcao para desenhar apenas um pixel na tela
 */
void drawPixel(int x, int y)
{
	glBegin(GL_POINTS); // Seleciona a primitiva GL_POINTS para desenhar
	glVertex2i(x, y);
	glEnd();  // indica o fim do ponto
}

/*
 *Funcao que desenha a lista de formas geometricas
 */
void drawFormas()
{
	// Apos o primeiro clique, desenha a reta com a posicao atual do mouse
	if (click1 && modo == LIN)
	{
		retaBresenham(x_1, y_1, m_x, m_y);
	}
	else if (click1 && modo == TRI && !click2)
	{
		retaBresenham(x_1, y_1, m_x, m_y);
	}
	else if (click1 && modo == TRI && click2)
	{
		retaBresenham(x_2, y_2, m_x, m_y);
	}
	else if (click1 && modo == POL && !vertices_temp.empty())
	{
		for (auto it = vertices_temp.begin(), prev = it++; it != vertices_temp.end(); prev = it++)
		{
			retaBresenham(prev->x, prev->y, it->x, it->y);
		}
		retaBresenham(vertices_temp.front().x, vertices_temp.front().y, m_x, m_y);
	}


	// Percorre a lista de formas geometricas para desenhar
	for (forward_list<forma>::iterator f = formas.begin(); f != formas.end(); f++)
	{
		switch (f->tipo)
		{
		case LIN:
		{
			int i = 0, x[2], y[2];
			// Percorre a lista de vertices da forma linha para desenhar
			for (forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++)
			{
				x[i] = v->x;
				y[i] = v->y;
			}
			// Desenha o segmento de reta apos dois cliques
			retaBresenham(x[0], y[0], x[1], y[1]);
			break;
		}
		case RET:
		{
			int i = 0, x[4], y[4];
			// Percorre a lista de vertices da forma quadrado para desenhar
			for (forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++)
			{
				x[i] = v->x;
				y[i] = v->y;
			}
			// Desenha os quatro lados do quadrado
			retaBresenham(x[0], y[0], x[1], y[1]);
			retaBresenham(x[1], y[1], x[2], y[2]);
			retaBresenham(x[2], y[2], x[3], y[3]);
			retaBresenham(x[3], y[3], x[0], y[0]);
			break;
		}
		case TRI:
		{
			int i = 0, x[3], y[3];
			// Percorre a lista de vertices da forma triangulo para desenhar
			for (forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++)
			{
				x[i] = v->x;
				y[i] = v->y;
			}
			// Desenha os três lados do triângulo
			retaBresenham(x[0], y[0], x[1], y[1]);
			retaBresenham(x[1], y[1], x[2], y[2]);
			retaBresenham(x[2], y[2], x[0], y[0]);
			break;
		}
		case POL:
		{
			int prev_x = -1, prev_y = -1, first_x = -1, first_y = -1;
			// Percorre a lista de vertices do poligono para desenhar
			for (forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++)
			{
				int x = v->x;
				int y = v->y;
				if (prev_x != -1 && prev_y != -1)
				{
					retaBresenham(prev_x, prev_y, x, y);
				}
				else
				{
					first_x = x;
					first_y = y;
				}
				prev_x = x;
				prev_y = y;
			}
			// Desenha o último segmento do polígono, ligando ao vértice mais próximo
			if (first_x != -1 && first_y != -1)
			{
				vertice ultimo = {prev_x, prev_y};
				vertice maisProximo = encontrarVerticeMaisProximo(ultimo, f->v);
				retaBresenham(prev_x, prev_y, maisProximo.x, maisProximo.y);
			}
			break;
		}
		case CIR:
		{
			int i = 0, x[4], y[4], x_centro, y_centro, raio;
			// Desenha a circunferência utilizando o algoritmo de Bresenham
			for (forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++)
			{
				x[i] = v->x;
				y[i] = v->y;
			}
			x_centro = x[1];
			y_centro = y[1];
			raio = distancia(x[0], y[0], x[1], y[1]);
			circunferenciaBresenham(x_centro, y_centro, raio);
			break;
		}

		}
	}
}

/*
 * Fucao que implementa o Algoritmo de Rasterizacao da Reta Imediata
 */
void retaBresenham(double x1, double y1, double x2, double y2)
{
	// Coordenadas
	int xIni = (int) x1, yIni = (int) y1;
	int xFim = (int) x2, yFim = (int) y2;
	int deltaX = abs(xFim - xIni), deltaY = abs(yFim - yIni);

	int s1 = xIni < xFim ? 1 : -1;
	int s2 = yIni < yFim ? 1 : -1;

	int var = 0;
	if (deltaY > deltaX)
	{
		int temp = deltaX;
		deltaX = deltaY;
		deltaY = temp;
		var = 1;
	}

	int e = 2 * deltaY - deltaX;
	int x = xIni, y = yIni;

	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();

	for (int i = 0; i < deltaX; i++)
	{
		while (e >= 0)
		{
			if (var == 1)
			{
				x = x + s1;
			}
			else
			{
				y = y + s2;
			}
			e = e - 2 * deltaX;
		}
		if (var == 1)
		{
			y = y + s2;
		}
		else
		{
			x = x + s1;
		}
		e = e + 2 * deltaY;

		glBegin(GL_POINTS);
		glVertex2i(x, y);
		glEnd();
	}
}
void circunferenciaBresenham(int x_centro, int y_centro, int raio)
{
	int x = 0;
	int y = raio;
	int d = 3 - 2 * raio; // valor inicial de decisão

	while (y >= x)
	{
		// Desenha os oito pontos simétricos
		drawPixel(x + x_centro, y + y_centro);
		drawPixel(-x + x_centro, y + y_centro);
		drawPixel(x + x_centro, -y + y_centro);
		drawPixel(-x + x_centro, -y + y_centro);
		drawPixel(y + x_centro, x + y_centro);
		drawPixel(-y + x_centro, x + y_centro);
		drawPixel(y + x_centro, -x + y_centro);
		drawPixel(-y + x_centro, -x + y_centro);

		// Testa se o ponto central do círculo foi atingido
		if (d <= 0)
		{
			d += 4 * x + 6;
		}
		else
		{
			d += 4 * (x - y) + 10;
			y--;
		}
		x++;
	}
}

void floodFill4(int x, int y, unsigned char fillColor[3], unsigned char targetColor[3])
{
	printf("pixel atual = (%d, %d)\n", x, y );
	// Verifica se a cor do pixel na posição (x, y) é a mesma cor que a do alvo
	unsigned char currentColor[3];
	glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, currentColor);
	if (currentColor[0] != targetColor[0] || currentColor[1] != targetColor[1] || currentColor[2] != targetColor[2])
	{
		return;
	}

	// Cria uma fila para armazenar os pixels a serem preenchidos
	queue<pair<int, int>> pixels;

	// Adiciona o pixel inicial à fila
	pixels.push( {x, y});

	// Enquanto houver pixels na fila
	while (!pixels.empty())
	{
		// Obtém o pixel atual da fila
		pair<int, int> currentPixel = pixels.front();
		pixels.pop();
		int currentX = currentPixel.first;
		int currentY = currentPixel.second;


		// Preenche o pixel atual com a cor de preenchimento
		glColor3ub(fillColor[0], fillColor[1], fillColor[2]);
		glBegin(GL_POINTS);
		glVertex2i(currentX, currentY);
		glEnd();
		glFlush();

		// Verifica os vizinhos do pixel atual (vizinhança 4)
		int dx[] = {0, 1, 0, -1};
		int dy[] = {1, 0, -1, 0};
		for (int i = 0; i < 4; i++)
		{
			int newX = currentX + dx[i];
			int newY = currentY + dy[i];

			// Verifica se o vizinho está dentro dos limites da janela
			if (newX >= 0 && newX < width && newY >= 0 && newY < altura)
			{
				// Verifica se a cor do vizinho é a mesma cor que a do alvo
				glReadPixels(newX, newY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, currentColor);
				if (currentColor[0] == targetColor[0] && currentColor[1] == targetColor[1] && currentColor[2] == targetColor[2])
				{
					// Adiciona o vizinho à fila
					pixels.push( {newX, newY});
				}
			}
		}
	}
}
