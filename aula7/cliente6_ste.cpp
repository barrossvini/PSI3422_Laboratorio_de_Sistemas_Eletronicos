//cliente6.cpp
#include <cekeikon.h>
#include <math.h>
#include "projeto.hpp"
#include <time.h>

/* 
receive a
direcao
dif_motor
fecha
*/
void toServer(CLIENT cliente, int direcao, int dif_motor, int fecha) {
	cliente.sendUint(direcao);
	cliente.sendUint(dif_motor);
	cliente.sendUint(fecha);
}


int main(int argc, char *argv[]) {
	if (argc != 4) erro("Numero de entradas errado: coloque 3");
	CLIENT client(argv[1]);
	uint32_t direcao = 0;
	uint32_t dif_motor = 0;
	uint32_t fecha = 0;

	// ---------------------------- DETECTAR QUADRADO ---------------------------- //
	float max_corrCC[20], max_corrNCC;
	int matriz_ref[20], quadrado;
	int linhaCC[20], colCC[20], linhaNCC[20], colNCC[20];
	Mat_<FLT> matNCC; Mat_<COR> a; Mat_<FLT> b;
	string modelo_str, corrCC_str, corrNCC_str;

	std::vector<Mat_<FLT>> matrizesCC(10); // para quadrados usando CC
	std::vector<Mat_<FLT>> matrizesNCC(10); // para quadrados usando NCC
	std::vector<Mat_<FLT>> quadrados(10); // Vetor de matrizes para quadrados redimensionados

	Mat_<FLT> T; 
	le(T, argv[2]); // lendo imagem e colocando em T

	VideoWriter vo(argv[3], CV_FOURCC('X','V','I','D'), 30, Size(320, 240));
	
	for (int n = 0; n < 10; n++) { // Construir 10 modelo em PG para detectar 
		resize(T, quadrados[n], Size(19*pow(1.1541, n), 19*pow(1.1541, n))); // Redimensionar {pow(x,y) = x^y;} {q = 1,1541}
		quadrados[n] = somaAbsDois(dcReject(quadrados[n], 1.0)); // Faz pre processamento com dont'care de cada modelo
	}
	
	// ----------------------------- RECONHECIMENTO DE DÍGITO ----------------------------- //
	// Leitura do banco de dados
	MNIST mnist(14, true, true); 
	mnist.le("/home/psi3422/programas");
	
	// Treinamento
	flann::Index ind(mnist.ax, flann::KDTreeIndexParams(4));
	vector<int> indices(1); 
	vector<float> dists(1);

	// Variáveis para printar na tela
	string digito_str;
	Mat_<COR> digito;
	Mat_<FLT> digito_flt;
	Mat_<FLT> red_digito;

	// -------------------------------------------------------------- //
	// ---------------------------- MAIN ---------------------------- //
	// -------------------------------------------------------------- //
	
	imshow("janela", a);
	int t = 0;
	dif_motor = 0;
	direcao = 0;
	t = 0;
	goto estado0;

	// ---------------------------- ESTADO 0 ----------------------------- //
	estado0: // SEGUIR A PLACA
		cout << "ESTADO 0" << endl;
		client.receiveImgComp(a); // captura um frame
		t = 0;
		converte(a, b); // imagem para imagem_float
		// inicialização das variáveis
		for (int i = 0; i < 20; i++) {
			max_corrCC[i] = 0.0f;
		}
		max_corrNCC = 0.0f;

		// Rodando casamento CC e NCC
		#pragma omp parallel for
		for (int n = 0; n < 10; n++) {
			matrizesCC[n] = matchTemplateSame(b, quadrados[n], CV_TM_CCORR);
			matrizesNCC[n] = matchTemplateSame(b, quadrados[n], CV_TM_CCOEFF_NORMED);
		}
		// Analise CC
		for (int n = 0; n < 10; n++) {
			for (int l = 0; l < matrizesCC[n].rows; l++) {
				for (int c = 0; c < matrizesCC[n].cols; c++) {
					for (int m = 0; m < 20; m++) {
						if (matrizesCC[n](l, c) >= max_corrCC[m]) {//m = 20 maximas correlações
							linhaCC[m] = l;
							colCC[m] = c;
							max_corrCC[m] = matrizesCC[n](l, c);
							matriz_ref[m] = n;
						} 
					}
				}
			}
		}
		// Analise NCC
		for (int m = 0; m < 20; m++){
			if (matrizesNCC[matriz_ref[m]](linhaCC[m], colCC[m]) >= max_corrNCC) {
				max_corrNCC = matrizesNCC[matriz_ref[m]](linhaCC[m], colCC[m]);
				quadrado = matriz_ref[m];
			}
		}

		// NÃO DETECTOU PLACA: PARAR MOTORES
		if (max_corrNCC < 0.55) {
			direcao = 0; // desligar motores - direção = 0 (parado)
			toServer(client, direcao, dif_motor, 0);
			goto estado0;
		}

		// DETECTOU PLACA
		if (max_corrNCC >= 0.55) {
			reta(a, linhaCC[quadrado] - (19*pow(1.1541, quadrado)/2), colCC[quadrado] - (19*pow(1.1541, quadrado)/2), linhaCC[quadrado] + (19*pow(1.1541, quadrado)/2), colCC[quadrado] - (19*pow(1.1541, quadrado)/2), COR(255,255,255));
			reta(a, linhaCC[quadrado] - (19*pow(1.1541, quadrado)/2), colCC[quadrado] - (19*pow(1.1541, quadrado)/2), linhaCC[quadrado] - (19*pow(1.1541, quadrado)/2), colCC[quadrado] + (19*pow(1.1541, quadrado)/2), COR(255,255,255));
			reta(a, linhaCC[quadrado] + (19*pow(1.1541, quadrado)/2), colCC[quadrado] - (19*pow(1.1541, quadrado)/2), linhaCC[quadrado] + (19*pow(1.1541, quadrado)/2), colCC[quadrado] + (19*pow(1.1541, quadrado)/2), COR(255,255,255));
			reta(a, linhaCC[quadrado] + (19*pow(1.1541, quadrado)/2), colCC[quadrado] + (19*pow(1.1541, quadrado)/2), linhaCC[quadrado] - (19*pow(1.1541, quadrado)/2), colCC[quadrado] + (19*pow(1.1541, quadrado)/2), COR(255,255,255));
			
			modelo_str = to_string(quadrado);
			corrCC_str = to_string(int(100*max_corrCC[quadrado]));
			corrNCC_str = to_string(int(100*max_corrNCC));
			
			putTxt(a, linhaCC[quadrado] + (19*pow(1.1541, quadrado)/2), colCC[quadrado] + (19*pow(1.1541, quadrado)/2) + 2, modelo_str, COR(255,0,0));
			putTxt(a, linhaCC[quadrado] + (19*pow(1.1541, quadrado)/2), colCC[quadrado] + (19*pow(1.1541, quadrado)/2) + 20, corrCC_str, COR(255,0,0));
			putTxt(a, linhaCC[quadrado] - (19*pow(1.1541, quadrado)/2), colCC[quadrado] + (19*pow(1.1541, quadrado)/2) + 2, modelo_str, COR(255,255,0));
			putTxt(a, linhaCC[quadrado] - (19*pow(1.1541, quadrado)/2), colCC[quadrado] + (19*pow(1.1541, quadrado)/2) + 20, corrNCC_str, COR(255,255,0));
			
			vo << a;
			
			// PLACA PEQUENA: ANDAR EM DIREÇÃO À ELA
			if (quadrado < 8) {
				if (colCC[quadrado] > 160) { // Placa está à direita
					direcao = 1; // Anda para a direita de forma ajustada
					dif_motor = colCC[quadrado] - 160;
				}
				else if (colCC[quadrado] < 160) { // Placa está à esquerda
					direcao = 2; // Anda para a esquerda de forma ajustada
					dif_motor = 160 - colCC[quadrado];
				}
				toServer(client, direcao, dif_motor, 0);
				goto estado0;
			}

			// PLACA SUFICIENTEMENTE GRANDE
			else {
				toServer(client, direcao, dif_motor, 0);
				goto estado1;
			}
		}
		

	estado1: // PARAR: Fica nesse estado uma fração de segundo até o carrinho parar
		cout << "ESTADO 1" << endl;	
		direcao = 0;
		dif_motor = 0;
		client.receiveImgComp(a);
		vo << a;
		toServer(client, direcao, dif_motor, 0);
		sleep(100);
		goto estado2; // Carrinho parou
		
	estado2:
		cout << "ESTADO 2" << endl;
		client.receiveImgComp(a);
		Range rows(linhaCC[quadrado] - 10, linhaCC[quadrado] + 10);
		Range cols(colCC[quadrado] - 10, colCC[quadrado] + 10);
		digito = a(rows, cols);
		converte(digito, digito_flt);
		
		for (int l = 0; l < digito_flt.rows; l++) {
			for (int c = 0; c < digito_flt.cols; c++) {
				if (digito_flt(l, c) <= 0.5) {
					digito_flt(l, c) = 0;
				}
				else {
					digito_flt(l, c) = 1.0;
				}
			}
		}
		
		red_digito = mnist.bbox(digito_flt);
		Mat_<FLT> alg(1, red_digito.total());
		
		for (int i = 0; i < alg.cols; i++) alg(0, i) = red_digito(i);
	
		ind.knnSearch(alg, indices, dists, 1);
	
		digito_str = to_string(int(mnist.ay(indices[0])));
		putTxt(a, linhaCC[quadrado], colCC[quadrado], digito_str, COR(0, 0, 255), 2);

		vo << a;
		
		// DÍGITO LIDO MANDA PARAR
		if (digito_str == "0" || digito_str == "1" || digito_str == "2" || digito_str == "3") {
			direcao = 0;
			dif_motor = 0;
			toServer(client, direcao, dif_motor, 0);
			goto estado4;
		}
		else {
			direcao = 0;
			dif_motor = 0;
			toServer(client, direcao, dif_motor, 0);
			goto estado3;
		}

	estado3:
		cout << "ESTADO 3" << endl;
		client.receiveImgComp(a);
		vo << a;
		if (digito_str == "4" || digito_str == "5" || digito_str == "6") {
			direcao = 3; // VIRA 180 PRA ESQUERDA
			dif_motor = 0;
		}
		else if (digito_str == "7" || digito_str == "8" || digito_str == "9") {
			direcao = 4; // VIRA 180 PRA DIREITA
			dif_motor = 0;
		}
		toServer(client, direcao, dif_motor, 0);

		while (t < 5) {
			t++;
			goto estado3;
		}
		goto estado5;
			
	estado4:
		cout << "ESTADO 4" << endl;
		client.receiveImgComp(a);
		vo << a;
		direcao = 0;
		dif_motor = 0;
		toServer(client, direcao, dif_motor, 1);
		return 0;

	estado5:
		cout << "ESTADO 5" << endl;
		client.receiveImgComp(a);
		vo << a;
		direcao = 0;
		dif_motor = 0;
		toServer(client, direcao, dif_motor, 0);
		sleep(100);
		goto estado0; 
}