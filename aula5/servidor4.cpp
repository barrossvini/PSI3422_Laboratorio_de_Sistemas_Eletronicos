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
	
	if (!w.isOpened()) erro("Erro: Abertura de webcam");
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    cam.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
    
    while (true) {
		server.receiveUint(client_pronto);
		cam.grab();
 		cam.grab();
		cam >> a;
		server.sendImgComp(a);
		server.receiveUint(direcao);
		server.receiveUint(dif_motor);

		if (direcao == 1) {
			if (dif_motor > 30) {							// Vira pra direita
				cout << "Virando pra DIREITA" << endl; 		// Debug
				softPwmWrite(0, 6*dif_motor/12); 
				softPwmWrite(1, 0); 
				softPwmWrite(2, 0); 
				softPwmWrite(3, 3*dif_motor/12); 
			}
			else { 											// Vai reto
				cout << "Andando RETO" << endl;				// Debug
				softPwmWrite(0, 50); 
				softPwmWrite(1, 0); 
				softPwmWrite(2, 50); 
				softPwmWrite(3, 0); 
			}
		} 	
		if (direcao == 2) { 
			if (dif_motor > 30) {							// Vira pra esquerda
				cout << "Virando pra ESQUERDA" << endl; 	// Debug
				softPwmWrite(0, 0); 
				softPwmWrite(1, 3*dif_motor/12); 
				softPwmWrite(2, 6*dif_motor/12); 
				softPwmWrite(3, 0); 
			}
			else {											// Vai reto
				cout << "Andando RETO" << endl;				// Debug
				softPwmWrite(0, 50); 
				softPwmWrite(1, 0); 
				softPwmWrite(2, 50); 
				softPwmWrite(3, 0); 
			}
		} 
		if (direcao == 0) { 								// Permanece parado
			cout << "PARADO" << endl;					// Debug
			softPwmWrite(0, 0); 
			softPwmWrite(1, 0); 
			softPwmWrite(2, 0); 
			softPwmWrite(3, 0); 
		}

		server.receiveUint(fecha);
		if (fecha == 1) {
			cout << "Servidor 4: Conexao fechada" << endl;
			break;
		}
	} // Fim while()
} // Fim main()
