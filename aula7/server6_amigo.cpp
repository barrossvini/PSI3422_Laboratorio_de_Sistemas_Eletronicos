//servidor6.cpp

#include "projeto.hpp"
#include <wiringPi.h>
#include <softPwm.h>

int main (void) {
	SERVER server;
	server.waitConnection();
	
	VideoCapture w(0);
	Mat_<COR> a;
	uint32_t client_pronto;
	uint32_t estado;
	uint32_t direcao;
	uint32_t dif_motor;
	uint32_t fecha;
	
	wiringPiSetup();
	if (softPwmCreate(0, 0, 100)) erro("erro");
	if (softPwmCreate(1, 0, 100)) erro("erro");
	if (softPwmCreate(2, 0, 100)) erro("erro");
	if (softPwmCreate(3, 0, 100)) erro("erro");
	
	if (!w.isOpened()) erro("Erro: Abertura de webcam 0.");
	w.set(CV_CAP_PROP_FRAME_WIDTH,320);
    w.set(CV_CAP_PROP_FRAME_HEIGHT,240);
    
    while(true) {
		estado = 0;
		server.receiveUint(client_pronto);
		w.grab();
		w.grab();
		w >> a;
		server.sendImgComp(a);
		server.receiveUint(estado);
		
		if (estado == 0) {
			server.receiveUint(direcao);
			server.receiveUint(dif_motor);
			if (direcao == 0) {
				softPwmWrite(0, 0); softPwmWrite(1, 0); softPwmWrite(2, 0); softPwmWrite(3, 0);
			}
			if (direcao == 1) {
				if (dif_motor > 30) {
					softPwmWrite(0, 6*dif_motor/8); softPwmWrite(1, 0); softPwmWrite(2, 0); softPwmWrite(3, 3*dif_motor/8); 
				}
				else { //vai reto
					softPwmWrite(0, 100); softPwmWrite(1, 0); softPwmWrite(2,80); softPwmWrite(3, 0); 
				}
			}
			if (direcao == 2) {
				if (dif_motor > 30) {
					softPwmWrite(0, 0); softPwmWrite(1, 3*dif_motor/8); softPwmWrite(2, 6*dif_motor/8); softPwmWrite(3, 0); 
				}
				else { //vai reto
					softPwmWrite(0, 100); softPwmWrite(1, 0); softPwmWrite(2,80); softPwmWrite(3, 0); 
				}
			}
		}
		else if (estado == 1) {
			uint32_t parou = 1;
			softPwmWrite(0, 0); softPwmWrite(1, 0); softPwmWrite(2, 0); softPwmWrite(3, 0);
			delay(4000);
			server.sendUint(parou);
		}
		else if (estado == 2) {
			
		}
		else if (estado == 3) {
			uint32_t algarismo;
			server.receiveUint(algarismo);
			if ((algarismo == 4)||(algarismo == 5)||(algarismo == 6)) {
				softPwmWrite(0, 0); softPwmWrite(1, 100); softPwmWrite(2, 100); softPwmWrite(3, 0);
				delay(2800);
			}
			else if ((algarismo == 7)||(algarismo == 8)||(algarismo == 9)) {
				softPwmWrite(0, 100); softPwmWrite(1, 0); softPwmWrite(2, 0); softPwmWrite(3, 100);
				delay(2800);
			}	
			else {
				softPwmWrite(0, 0); softPwmWrite(1, 0); softPwmWrite(2, 0); softPwmWrite(3, 0);
			}
		}
		else if (estado == 4) {
			softPwmWrite(0, 0); softPwmWrite(1, 0); softPwmWrite(2, 0); softPwmWrite(3, 0);
		}
		else if (estado == 5) {
			uint32_t parou = 1;
			softPwmWrite(0, 0); softPwmWrite(1, 0); softPwmWrite(2, 0); softPwmWrite(3, 0);
			server.sendUint(parou);
		}
		server.receiveUint(fecha);
		if (fecha == 1) break;
	}
}
