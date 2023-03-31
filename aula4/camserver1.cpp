// Stephanie Lonzhu Chi - 11257251
// Vinicius de Barros Silva - 10335913
//camserver1.cpp

#include "projeto.hpp"

int main (void) {
	SERVER server;
	server.waitConnection();
	
	VideoCapture w(0);
	Mat_<COR> a;
	uint32_t recebeu;
	
	if (!w.isOpened()) erro("Error: Webcam");
	w.set(CV_CAP_PROP_FRAME_WIDTH,480);
    w.set(CV_CAP_PROP_FRAME_HEIGHT,640);
    
    while(true) {
		w >> a;
		server.sendImg(a);
		server.receiveUint(recebeu);
		cout << "Sending" << endl;
	}

}
