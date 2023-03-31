//fase5.cpp

#include <cekeikon.h>
#include <math.h>
#include <omp.h>

int main(int argc, char* argv[]) {
	if (argc != 4) erro("Numero de entradas errado: coloque 3");
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

	VideoCapture vi(argv[1]);

	Mat_<FLT> T; 
	le(T, argv[2]); // lendo imagem e colocando em T

	VideoWriter vo(argv[3], CV_FOURCC('X','V','I','D'), 30, Size(320, 240));
	
	for (int n = 0; n < 10; n++) { //Construir 10 modelo em PG para 
		resize(T, quadrados[n], Size(19*pow(1.1541, n), 19*pow(1.1541, n)));// redimensionar {pow(x,y) = x^y;} {q = 1,1541}
		quadrados[n] = somaAbsDois(dcReject(quadrados[n], 1.0));// faz pre processamento com dont'care de cada modelo
	}
	
	// Reconhecimento de digito
	MNIST mnist(14, true, true); 
	mnist.le("/home/psi3422/programas");
	string digito_str;
	Mat_<COR> digito;
	Mat_<FLT> digito_flt;
	Mat_<FLT> red_digito;
	
	flann::Index ind(mnist.ax,flann::KDTreeIndexParams(5));
	vector<int> indices(1); 
	vector<float> dists(1);

	
	// Fazer em loop
	while(true) {
		vi >> a;
		converte(a, b);
		max_corrNCC = 0;
		digito_str = ""; // RECONHECIMENTO
		
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
				ref_max_correNCC = matriz_ref[m];
			}
		}

		// Análise CC
		if (max_corrNCC > 0.55) {
			reta(a, linhaCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), linhaCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), COR(255,255,255));
			reta(a, linhaCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), linhaCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), COR(255,255,255));
			reta(a, linhaCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), linhaCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), COR(255,255,255));
			reta(a, linhaCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), linhaCC[ref_max_correNCC] - (19*pow(1.1541, ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541, ref_max_correNCC)/2), COR(255,255,255));
			
			modelo_str = to_string(ref_max_correNCC);
			corrCC_str = to_string(int(100*max_corrCC[ref_max_correNCC]));
			corrNCC_str = to_string(int(100*max_corrNCC));
			
			putTxt(a, linhaCC[ref_max_correNCC] + (19*pow(1.1541,ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541,ref_max_correNCC)/2) + 2, modelo_str, COR(255,0,0));
			putTxt(a, linhaCC[ref_max_correNCC] + (19*pow(1.1541,ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541,ref_max_correNCC)/2) + 20, corrCC_str, COR(255,0,0));
			putTxt(a, linhaCC[ref_max_correNCC] - (19*pow(1.1541,ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541,ref_max_correNCC)/2) + 2, modelo_str, COR(255,255,0));
			putTxt(a, linhaCC[ref_max_correNCC] - (19*pow(1.1541,ref_max_correNCC)/2), colCC[ref_max_correNCC] + (19*pow(1.1541,ref_max_correNCC)/2) + 20, corrNCC_str, COR(255,255,0));

			if (ref_max_correNCC < 10) {
				Range rows(linhaCC[ref_max_correNCC] - 10, linhaCC[ref_max_correNCC] + 10);
				Range cols(colCC[ref_max_correNCC] - 10, colCC[ref_max_correNCC] + 10);
				digito = a(rows, cols);
				converte(digito, digito_flt);
				
				for (int l = 0; l < digito_flt.rows; l++) {
					for (int c = 0; c < digito_flt.cols; c++) {
						if (digito_flt(l, c) < 0.6) {
							digito_flt(l, c) = 0;
						}
						else {
							digito_flt(l, c) = 1.0;
						}
					}
				}
				
				red_digito = mnist.bbox(digito_flt);
				Mat_<FLT> alg(1, red_digito.total());
				
				for (int i = 0; i < alg.cols; i++) {
					alg(0, i) = red_digito(i);
				}
			
				ind.knnSearch(alg, indices, dists, 1);
			
				digito_str = to_string(int(mnist.ay(indices[0])));
				putTxt(a, linhaCC[ref_max_correNCC], colCC[ref_max_correNCC], digito_str, COR(0, 0, 255), 2);
				cout << digito_str << endl;
			}
		}
		else {}

		imshow("Aula 6", a);
		vo << a;

		int ch = (signed char)(waitKey(30));
		if (ch == 27) {
			cout << "fase5: Conexao fechada" << endl;
			break;
		}
		else {}
		
	} //fecha while
} //fecha main
