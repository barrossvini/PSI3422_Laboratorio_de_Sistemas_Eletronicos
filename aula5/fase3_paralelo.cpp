//fase3.cpp

#include <cekeikon.h>
#include <math.h>
#include <omp.h>

//fase3 capturado.avi quadrado.png seu_localiza.avi

int main(int argc, char* argv[]) {
	if (argc != 4) erro("Numero de entradas errado");
	int ref_max_correNCC;
	float max_corrCC[20] ;
	int matriz_ref[20];
	float max_corrNCC;
	Mat_<FLT> matNCC;
	int linhaCC[20], colCC[20] , linhaNCC[20], colNCC[20];
	Mat_<COR> a; Mat_<FLT> b; 
	
	int count = 0;
	string modelo_str;
	string corrCC_str;
	string corrNCC_str;
	
	std::vector<Mat_<FLT>> matrizesCC(10);// para quadrados usando CC
	std::vector<Mat_<FLT>> matrizesNCC(10); // para quadrados usando NCC
	std::vector<Mat_<FLT>> quadrados(10); // Vetor de matrizes para quadrados redimensionados
	
	VideoCapture vi(argv[1]);//captura video e poe em vi

	Mat_<FLT> T; 
	le(T, argv[2]); // lendo imagem e colocando em T

	
	VideoWriter vo(argv[3], CV_FOURCC('X','V','I','D'), 30, Size(320, 240));
	
	for (int n = 0; n < 10; n++) {//Construir 10 modelo em PG para 
		resize(T, quadrados[n], Size(19*pow(1.1541, n), 19*pow(1.1541, n)));// redimensionar {pow(x,y) = x^y;} {q = 1,1541}
		quadrados[n] = somaAbsDois(dcReject(quadrados[n], 1.0));// faz pre processamento com dont'care de cada modelo
	}
	
	//fazer em loop
	while(true) {
		TimePoint t1 = timePoint();
		
		vi >> a; //poe video em a tipo <COR>
		converte(a, b);// converte para <FLT>
		for (int i = 0; i < 20; i++) {
			max_corrCC[i] = 0.0f;
		}
		
		max_corrNCC = 0.0f;

		//processamento paralelo
		#pragma omp parallel for

		// Rodando casamento CC e NCC

		for (int n = 0; n < 10; n++) {
			matrizesCC[n] = matchTemplateSame(b, quadrados[n], CV_TM_CCORR);// faz match com todos quadrados
			matrizesNCC[n] = matchTemplateSame(b, quadrados[n], CV_TM_CCOEFF_NORMED);// faz match com todos quadrados
		}

		// Analise CC
		
		for (int n = 0; n < 10; n++) {
			for (int l = 0; l < matrizesCC[n].rows; l++) {
				for (int c = 0; c < matrizesCC[n].cols; c++) {
					for(int m =0; m < 20; m++){
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

		//Analise NCC

		for (int m = 0; m < 20; m++) {
			if (matrizesNCC[matriz_ref[m]](linhaCC[m],colCC[m]) >= max_corrNCC){
				max_corrNCC = matrizesNCC[matriz_ref[m]](linhaCC[m],colCC[m]);
				ref_max_correNCC = matriz_ref[m];
			}
		}


		
		if (max_corrNCC > 0.55) {
			reta(a, linhaCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), linhaCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), COR(255,255,255));
			reta(a, linhaCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), linhaCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), COR(255,255,255));
			reta(a, linhaCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), linhaCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), COR(255,255,255));
			reta(a, linhaCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), linhaCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), COR(255,255,255));
			
			/*reta(a, linhaNCC - (19*pow(1.1541, ref_max_correNCC)/2), colNCC - (19*pow(1.1541, ref_max_correNCC)/2), linhaNCC + (19*pow(1.1541, ref_max_correNCC)/2), colNCC - (19*pow(1.1541, ref_max_correNCC)/2), COR(255,255,0));
			reta(a, linhaNCC - (19*pow(1.1541, ref_max_correNCC)/2), colNCC - (19*pow(1.1541, ref_max_correNCC)/2), linhaNCC - (19*pow(1.1541, ref_max_correNCC)/2), colNCC + (19*pow(1.1541, ref_max_correNCC)/2), COR(255,255,0));
			reta(a, linhaNCC + (19*pow(1.1541, ref_max_correNCC)/2), colNCC - (19*pow(1.1541, ref_max_correNCC)/2), linhaNCC + (19*pow(1.1541, ref_max_correNCC)/2), colNCC + (19*pow(1.1541, ref_max_correNCC)/2), COR(255,255,0));
			reta(a, linhaNCC + (19*pow(1.1541, ref_max_correNCC)/2), colNCC + (19*pow(1.1541, ref_max_correNCC)/2), linhaNCC - (19*pow(1.1541, ref_max_correNCC)/2), colNCC + (19*pow(1.1541, ref_max_correNCC)/2), COR(255,255,0));
			*/
			modelo_str = to_string(ref_max_correNCC);
			
			corrCC_str = to_string(int(100*max_corrCC[ref_max_correNCC]));
			corrNCC_str = to_string(int(100*max_corrNCC));
			putTxt(a, linhaCC[ref_max_correNCC] + (19*pow(1.1541,ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541,ref_max_correNCC)/2) + 2, modelo_str, COR(255,0,0));
			putTxt(a, linhaCC[ref_max_correNCC] + (19*pow(1.1541,ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541,ref_max_correNCC)/2) + 20, corrCC_str, COR(255,0,0));
			putTxt(a, linhaCC[ref_max_correNCC] - (19*pow(1.1541,ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541,ref_max_correNCC)/2) + 2, modelo_str, COR(255,255,0));
			putTxt(a, linhaCC[ref_max_correNCC] - (19*pow(1.1541,ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541,ref_max_correNCC)/2) + 20, corrNCC_str, COR(255,255,0));
		}
		
		else {}
		imshow("janela", a);
		vo << a;
		TimePoint t2 = timePoint();
		double t = timeSpan(t1, t2);
		cout << int(1/t) << " quadros/seg" << endl;

		int ch = (signed char)(waitKey(30)); // Se apertar ESC
		if (ch == 27) break;
	}
}
