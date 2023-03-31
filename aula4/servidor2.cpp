// Stephanie Lonzhu Chi - 11257251
// Vinicius de Barros Silva - 10335913
//servidor2.cpp

#include "projeto.hpp"
#include <wiringPi.h>
#include <softPwm.h>

int main (void) {
	SERVER server;
	server.waitConnection();
	
	VideoCapture w(0);
	Mat_<COR> a;
	uint32_t client_ready;
	uint32_t state;
	uint32_t close;

	wiringPiSetup();
	if (softPwmCreate(0, 0, 100)) erro("ERROR: Soft PWM Create");
	if (softPwmCreate(1, 0, 100)) erro("ERROR: Soft PWM Create");
	if (softPwmCreate(2, 0, 100)) erro("ERROR: Soft PWM Create");
	if (softPwmCreate(3, 0, 100)) erro("ERROR: Soft PWM Create");
	
	if (!w.isOpened()) erro("ERROR: SERVIDOR 2: Webcam");
	w.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    w.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
    
    while(true) {
		server.receiveUint(client_ready);
		server.receiveUint(state);
		if (state == 0) {
			w >> a;
			putTxt(a, 5, 5, "0", COR(0, 0, 255), 5);
			server.sendImgComp(a);
			softPwmWrite(0, 0); 
			softPwmWrite(1, 0); 
			softPwmWrite(2, 0); 
			softPwmWrite(3, 0); 
		}
		if (state == 1) {
			w >> a;
			putTxt(a, 5, 5, "1", COR(0, 0, 255), 5);
			server.sendImgComp(a);
			softPwmWrite(1, 0); 
			softPwmWrite(3, 100);
		}
		if (state == 2) {
			w >> a;
			putTxt(a, 5, 5, "2", COR(0, 0, 255), 5);
			server.sendImgComp(a);
			softPwmWrite(1, 100); 
			softPwmWrite(3, 80);
		}
		if (state == 3) {
			w >> a;
			putTxt(a, 5, 5, "3", COR(0, 0, 255), 5);
			server.sendImgComp(a);
			softPwmWrite(1, 100); 
			softPwmWrite(3, 0);
		}
		if (state == 4) {
			w >> a;
			putTxt(a, 5, 5, "4", COR(0, 0, 255), 5);
			server.sendImgComp(a);
			softPwmWrite(1, 100); 
			softPwmWrite(2, 100);
		}
		if (state == 6) {
			w >> a;
			putTxt(a, 5, 5, "6", COR(0, 0, 255), 5);
			server.sendImgComp(a);
			softPwmWrite(0, 100); 
			softPwmWrite(3, 100);
		}
		if (state == 7) {
			w >> a;
			putTxt(a, 5, 5, "7", COR(0, 0, 255), 5);
			server.sendImgComp(a);
			softPwmWrite(2, 100); 
			softPwmWrite(3, 0); 
		}
		if (state == 8) {
			w >> a;
			putTxt(a, 5, 5, "8", COR(0, 0, 255), 5);
			server.sendImgComp(a);
			softPwmWrite(0, 100); 
			softPwmWrite(2, 80);
		}
		if (state == 9) {
			w >> a;
			putTxt(a, 5, 5, "9", COR(0, 0, 255), 5);
			server.sendImgComp(a);
			softPwmWrite(0, 100); 
			softPwmWrite(1, 0);
		}
		
		server.receiveUint(close);
		if (close == 1) {
			cout << "SERVIDOR 2: Closing" << endl;
			break;
		}
	}

}
