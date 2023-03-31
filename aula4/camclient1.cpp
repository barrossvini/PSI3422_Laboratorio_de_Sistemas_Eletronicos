// Stephanie Lonzhu Chi - 11257251
// Vinicius de Barros Silva - 10335913
//camclient1.cpp

#include "projeto.hpp"

int main(int argc, char *argv[]) {
	if (argc!=2) erro("client6 servidorIpAddr\n");
    CLIENT client(argv[1]);   
	uint32_t recebi = 0;
	Mat_<COR> b;
	namedWindow("Window");
	
	while (true)  {
		client.receiveImg(b);
		imshow("Window",b);

		client.sendUint(recebi);
		cout << "Received" << endl;
		int ch=(signed char)(waitKey(30)); // E necessario (signed char)
		if (ch == 27)  break;
  }
}


