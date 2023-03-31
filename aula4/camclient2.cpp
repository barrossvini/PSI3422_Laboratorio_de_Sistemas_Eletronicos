// Stephanie Lonzhu Chi - 11257251
// Vinicius de Barros Silva - 10335913
//camclient2.cpp

#include "projeto.hpp"

int main(int argc, char *argv[]) {
	if (argc!=2) erro("client6 servidorIpAddr\n");
    CLIENT client(argv[1]);   
	uint32_t received = 0;
	Mat_<COR> b;
	namedWindow("Window");
	
	while (true)  {
		client.receiveImgComp(b);
		imshow("Window",b);

		int ch = (signed char)(waitKey(30));
		if (ch == 27)  {
			cout << "Closed" << endl;
			recebi = 1;
			client.sendUint(received);
			break;
		}
		cout << "Received" << endl;
		client.sendUint(received);
  }
}


