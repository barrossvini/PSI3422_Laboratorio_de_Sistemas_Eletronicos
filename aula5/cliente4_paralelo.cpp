// Aula 5 - Lição 3
// Stephanie Lonzhu Chi - 11257251
// Vinicius de Barros Silva - 10335913

// cliente4.cpp
// compila cliente4.cpp -c -omp
// ./cliente4 [IP rasp] [quadrado.png] [videocapturado.avi]

#include <cekeikon.h>
#include <math.h>
#include "projeto.hpp"
#include <omp.h>

int main(int argc, char* argv[]) {
	if (argc != 4) erro("Numero de entradas errado: precisa de 3");
	else {
		CLIENT client(argv[1]); // pega o IP da raspberry no primeiro parâmetro passado 
		uint32_t pronto = 1;
		uint32_t fecha = 0;
		uint32_t dif_motor = 0;
		uint32_t direcao = 0;

		int ref_max_correNCC; int matriz_ref[20];
		float max_corrCC[20]; float max_corrNCC;
		int linhaCC[20], colCC[20], linhaNCC[20], colNCC[20];
		Mat_<FLT> matNCC; Mat_<COR> a; Mat_<FLT> b; 
		
		int count = 0;
		string modelo_str;
		string corrCC_str;
		string corrNCC_str;
		
		std::vector<Mat_<FLT>> matrizesCC(10); 	// Para quadrados usando CC
		std::vector<Mat_<FLT>> matrizesNCC(10); // Para quadrados usando NCC
		std::vector<Mat_<FLT>> quadrados(10); 	// Vetor de matrizes para quadrados redimensionados
		
		Mat_<FLT> T; 
		le(T, argv[2]); // Lendo imagem "quadrado.png" e colocando em T
		
		VideoWriter vo(argv[3], CV_FOURCC('X','V','I','D'), 30, Size(320, 240)); // Escreve um vídeo de nome passado no parâmetro 3
		
		for (int n = 0; n < 10; n++) { // Construir 10 modelo em PG
			resize(T, quadrados[n], Size(19*pow(1.1541, n), 19*pow(1.1541, n)));// Redimensionar {pow(x,y) = x^y;} {q = 1,1541}
			quadrados[n] = somaAbsDois(dcReject(quadrados[n], 1.0)); // Faz pre processamento com dont'care de cada modelo
		}
		
		while (true) {
			client.sendUint(pronto);
			client.receiveImgComp(a);
			dif_motor = 0;
			direcao = 0;
			converte(a, b); // Converte para <FLT>

			for (int i = 0; i < 20; i++) {
				max_corrCC[i] = 0.0f;
			}
			max_corrNCC = 0.0f;

			// Processamento paralelo
			#pragma omp parallel for

			// Rodando casamento CC e NCC
			for (int n = 0; n < 10; n++) {
				matrizesCC[n] = matchTemplateSame(b, quadrados[n], CV_TM_CCORR); // faz match com todos quadrados
				matrizesNCC[n] = matchTemplateSame(b, quadrados[n], CV_TM_CCOEFF_NORMED); // faz match com todos quadrados
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
					ref_max_correNCC = matriz_ref[m];
				}
			}
			
			// Desenha na tela
			if (max_corrNCC > 0.55) {
				reta(a, linhaCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), linhaCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), COR(255,255,255));
				reta(a, linhaCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), linhaCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), COR(255,255,255));
				reta(a, linhaCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), linhaCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), COR(255,255,255));
				reta(a, linhaCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), linhaCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), COR(255,255,255));
				
				modelo_str = to_string(ref_max_correNCC);
				corrCC_str = to_string(int(100*max_corrCC[ref_max_correNCC]));
				corrNCC_str = to_string(int(100*max_corrNCC));

				putTxt(a, linhaCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2) + 2, modelo_str, COR(255,0,0));
				putTxt(a, linhaCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2) + 20, corrCC_str, COR(255,0,0));
				putTxt(a, linhaCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2) + 2, modelo_str, COR(255,255,0));
				putTxt(a, linhaCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2) + 20, corrNCC_str, COR(255,255,0));
				
				if (colCC[ref_max_correNCC] > 160) { // Placa está à direita
					direcao = 1;
					dif_motor = colCC[ref_max_correNCC] - 160;
				}
				if (colCC[ref_max_correNCC] < 160) { // Placa está à esquerda
					direcao = 2; 
					dif_motor = 160 - colCC[ref_max_correNCC];
				}
			}
			
			client.sendUint(direcao);
			client.sendUint(dif_motor);

			imshow("Aula 5: Licao 3", a);
			vo << a;

			int ch = (signed char)(waitKey(30));
			if (ch == 27) {
				fecha = 1; client.sendUint(fecha);
				cout << "Cliente 4: Conexao fechada" << endl;
				break;
			}
			else {
				fecha = 0; client.sendUint(fecha);
			}
		}
	}
}
