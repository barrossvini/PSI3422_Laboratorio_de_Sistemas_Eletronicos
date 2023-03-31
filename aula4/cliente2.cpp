// Stephanie Lonzhu Chi - 11257251
// Vinicius de Barros Silva - 10335913
//cliente1.cpp

#include "projeto.hpp"

uint32_t state = 0;
void on_mouse(int event, int c, int l, int flags, void* userdata) {
  if (event == EVENT_LBUTTONDOWN) {
    if      (0 <= l   && l < 80  && 0 <= c   && c < 80)  state = 7;
    else if (0 <= l   && l < 80  && 80 <= c  && c < 160) state = 8;
    else if (0 <= l   && l < 80  && 160 <= c && c < 240) state = 9;
    else if (80 <= l  && l < 160 && 0 <= c   && c < 80)  state = 4;
    else if (80 <= l  && l < 160 && 160 <= c && c < 240) state = 6;
    else if (160 <= l && l < 240 && 0 <= c   && c < 80)  state = 1;
    else if (160 <= l && l < 240 && 80 <= c  && c < 160) state = 2;
    else if (160 <= l && l < 240 && 160 <= c && c < 240) state = 3;
    else state = 0;
  } 
  else if (event == EVENT_LBUTTONUP) state = 0;
}

int main(int argc, char *argv[]) {
	CLIENT client(argv[1]);   
	uint32_t ready = 1;
	uint32_t close = 0;
	COR gray(128, 128, 128);
	COR red(0, 0, 255);
	Mat_<COR> teclado (240, 240, gray);
	Mat_<COR> b;
	Mat_<COR> tela;
	namedWindow("Cliente 2: Window");
	VideoWriter vo;
	if (argc == 3) VideoWriter vo(argv[2],CV_FOURCC('X', 'V', 'I', 'D'), 30, Size(560, 240));
	setMouseCallback("Cliente 2: Window", on_mouse);
	
	reta(teclado, 80, 0, 80, 240, COR(0,0,0));
	reta(teclado, 160, 0, 160, 240, COR(0,0,0));
	reta(teclado, 0, 80, 240, 80, COR(0,0,0));
	reta(teclado, 0, 160, 240, 160, COR(0,0,0));

	reta(teclado, 120, 110, 120, 130, red);
	reta(teclado, 110, 120, 130, 120, red);
	
	flecha(teclado, 60, 120, 20, 120, red); 	// CIMA
	flecha(teclado, 180, 120, 220, 120, red); 	// BAIXO
	flecha(teclado, 120, 60, 120, 20, red); 	// ESQUERDA
	flecha(teclado, 120, 180, 120, 220, red); 	// DIREITA

	flecha(teclado, 60, 60, 20, 20, red); 		// Diagonal CIMA/ESQUERDA
	flecha(teclado, 60, 180, 20, 220, red); 	// diagonal CIMA/DIREITA
	flecha(teclado, 180, 60, 220, 20, red); 	// Diagonal BAIXO/ESQUERDA
	flecha(teclado, 180, 180, 220, 220, red); 	// Diagonal BAIXO/DIREITA
	
	while (true) {
		client.sendUint(ready);
		client.sendUint(state);
		client.receiveImgComp(b);
		tela = grudaH(teclado, b);
		imshow("Cliente 2: Window", tela);
		if (argc == 3) vo << tela;
		
		int ch = (signed char)(waitKey(30));
		if (ch == 27) { // Se apertar ESC
			close = 1; 
			client.sendUint(close);
			cout << "Cliente 2: Closed" << endl;
			break;
		}
		else {
			close = 0; 
			client.sendUint(close);
		}
	}
}