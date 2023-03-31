// Aula 5 - Lição 3
// Stephanie Lonzhu Chi - 11257251
// Vinicius de Barros Silva - 10335913

// servidor4.cpp
// compila servidor4.cpp -c -w
// ./servidor4

#include "projeto.hpp"
#include <wiringPi.h>
#include <softPwm.h>

int main (void) {
	SERVER server;
	server.waitConnection();
	
	VideoCapture cam(0);
	Mat_<COR> a;

	uint32_t client_pronto;
	uint32_t direcao;
	uint32_t dif_motor;
	uint32_t fecha;

	wiringPiSetup();
	if (softPwmCreate(0, 0, 100)) erro("Error: softPwmCreate 0");
	if (softPwmCreate(1, 0, 100)) erro("Error: softPwmCreate 1");
	if (softPwmCreate(2, 0, 100)) erro("Error: softPwmCreate 2");
	if (softPwmCreate(3, 0, 100)) erro("Error: softPwmCreate 3");
	
	if (!cam.isOpened()) erro("Erro: Abertura de webcam");
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    cam.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

    while (true) {
		cam.grab();
 		cam.grab();
		cam >> a;
		server.sendImgComp(a);
		server.receiveUint(direcao);
		server.receiveUint(dif_motor);

		if (direcao == 0) { // PARADO (0, 1, 2, 3)
			softPwmWrite(0, 0); 
			softPwmWrite(1, 0); 
			softPwmWrite(2, 0); 
			softPwmWrite(3, 0);
		}
		else if (direcao == 1) { // ANDA AJUSTADO PRA DIREITA
			if (dif_motor > 30) {							// Vira pra direita
				cout << "Virando pra DIREITA" << endl; 		// Debug
				softPwmWrite(0, 6*dif_motor/12); 
				softPwmWrite(1, 0); 
				softPwmWrite(2, 0); 
				softPwmWrite(3, 3*dif_motor/12); 
			}
			else { 											// Vai reto
				cout << "Andando RETO" << endl;				// Debug
				softPwmWrite(0, 65); 
				softPwmWrite(1, 0); 
				softPwmWrite(2, 70); 
				softPwmWrite(3, 0); 
			}
		}
		else if (direcao == 2) { // ANDA AJUSTADO PRA ESQUERDA
			if (dif_motor > 30) {							// Vira pra esquerda
				cout << "Virando pra ESQUERDA" << endl; 	// Debug
				softPwmWrite(0, 0); 
				softPwmWrite(1, 3*dif_motor/12); 
				softPwmWrite(2, 6*dif_motor/12); 
				softPwmWrite(3, 0); 
			}
			else {											// Vai reto
				cout << "Andando RETO" << endl;				// Debug
				softPwmWrite(0, 65); 
				softPwmWrite(1, 0); 
				softPwmWrite(2, 70); 
				softPwmWrite(3, 0); 
			}
		}
		else if (direcao == 3) { // GIRA 180 PRA ESQUERDA (4, 5, 6)
			cout << "Girando 180 pra ESQUERDA" << endl;
			softPwmWrite(0, 0); 
			softPwmWrite(1, 100); 
			softPwmWrite(2, 100); 
			softPwmWrite(3, 0);
			delay(2500);
			uint32_t acabou_3 = 1;
			server.sendUint(acabou_3);
		}
		else if (direcao == 4) { // GIRA 180 PRA DIREITA (7, 8, 9)
			cout << "Girando 180 pra DIREITA" << endl;
			softPwmWrite(0, 100); 
			softPwmWrite(1, 0); 
			softPwmWrite(2, 0); 
			softPwmWrite(3, 100);
			delay(2500);
			uint32_t acabou_4 = 1;
			server.sendUint(acabou_4);
		}

		server.receiveUint(fecha);
		if (fecha == 1) {
			cout << "Servidor 6: Conexao fechada" << endl;
			break;
		}
	} // Fim while()
} // Fim main()

