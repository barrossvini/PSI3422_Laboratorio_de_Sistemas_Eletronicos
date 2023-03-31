//cliente4.cpp

#include "projeto.hpp"

int main(int argc, char *argv[]) {
	if (argc != 3) {
		cout << "argumentos invalidos" << endl;
	}
	else {
		CLIENT client(argv[1]);   
		uint32_t pronto = 1;
		uint32_t fecha = 0;
		uint32_t dif_motor = 0;
		uint32_t direcao = 0;
		Mat_<COR> a; Mat_<FLT> b; 
		namedWindow("janela");
		VideoWriter vo(argv[2],CV_FOURCC('X','V','I','D'),30,Size(320,240));
		int melhor_modelo;
		float max_corrCC = 0;
		float max_corrNCC = 0;
		Mat_<FLT> matNCC;
		int linhaCC, colCC, linhaNCC, colNCC;
		string modelo_str;
		string corrCC_str;
		string corrNCC_str;
		std::vector<Mat_<FLT>> matrizesCC(10);
		Mat_<FLT> T; le(T,"quadrado.png");
		std::vector<Mat_<FLT>> quadrado_redim(10); 
		
		for (int n = 0; n < 10; n++) {
			resize(T, quadrado_redim[n], Size(69*pow(0.85,n),69*pow(0.85,n)));
			quadrado_redim[n]=somaAbsDois(dcReject(quadrado_redim[n],1.0));
		}
		
		while (true) {
			//TimePoint t1 = timePoint();
			client.sendUint(pronto);
			client.receiveImgComp(a);
			converte(a,b);
			max_corrCC = 0;
			max_corrNCC = 0;
			dif_motor = 0;
			direcao = 0;
			
			#pragma omp parallel for
			for (int n = 0; n < 10; n++) {
				matrizesCC[n] = matchTemplateSame(b,quadrado_redim[n], CV_TM_CCORR);
			}
			for (int l = 0; l < matrizesCC[0].rows; l++) {
				for (int c=0; c< matrizesCC[0].cols; c++){
					for (int n = 0; n < 10; n++) {
						if (matrizesCC[n](l,c) >= max_corrCC) {
							linhaCC = l;
							colCC = c;
							max_corrCC = matrizesCC[n](l,c);
							melhor_modelo = n;
						} 
					}
				}
			}
			matNCC = matchTemplateSame(b,quadrado_redim[melhor_modelo], CV_TM_CCOEFF_NORMED);
			for (int l = 0; l < matNCC.rows; l++) {
				for (int c=0; c< matNCC.cols; c++){
					if (matNCC(l,c) >= max_corrNCC) {
						linhaNCC = l;
						colNCC = c;
						max_corrNCC = matNCC(l,c);
					}
				}
			}
			
			if ((abs(linhaCC - linhaNCC) <= 30) && abs(colCC - colNCC <= 30) && (max_corrCC > 0.20) && (max_corrNCC > 0.20)) { //caso encontre placa
				reta(a,linhaCC-(69*pow(0.85,melhor_modelo)/2),colCC-(69*pow(0.85,melhor_modelo)/2),linhaCC+(69*pow(0.85,melhor_modelo)/2),colCC-(69*pow(0.85,melhor_modelo)/2),COR(255,0,0));
				reta(a,linhaCC-(69*pow(0.85,melhor_modelo)/2),colCC-(69*pow(0.85,melhor_modelo)/2),linhaCC-(69*pow(0.85,melhor_modelo)/2),colCC+(69*pow(0.85,melhor_modelo)/2),COR(255,0,0));
				reta(a,linhaCC+(69*pow(0.85,melhor_modelo)/2),colCC-(69*pow(0.85,melhor_modelo)/2),linhaCC+(69*pow(0.85,melhor_modelo)/2),colCC+(69*pow(0.85,melhor_modelo)/2),COR(255,0,0));
				reta(a,linhaCC+(69*pow(0.85,melhor_modelo)/2),colCC+(69*pow(0.85,melhor_modelo)/2),linhaCC-(69*pow(0.85,melhor_modelo)/2),colCC+(69*pow(0.85,melhor_modelo)/2),COR(255,0,0));
		
				reta(a,linhaNCC-(69*pow(0.85,melhor_modelo)/2),colNCC-(69*pow(0.85,melhor_modelo)/2),linhaNCC+(69*pow(0.85,melhor_modelo)/2),colNCC-(69*pow(0.85,melhor_modelo)/2),COR(255,255,0));
				reta(a,linhaNCC-(69*pow(0.85,melhor_modelo)/2),colNCC-(69*pow(0.85,melhor_modelo)/2),linhaNCC-(69*pow(0.85,melhor_modelo)/2),colNCC+(69*pow(0.85,melhor_modelo)/2),COR(255,255,0));
				reta(a,linhaNCC+(69*pow(0.85,melhor_modelo)/2),colNCC-(69*pow(0.85,melhor_modelo)/2),linhaNCC+(69*pow(0.85,melhor_modelo)/2),colNCC+(69*pow(0.85,melhor_modelo)/2),COR(255,255,0));
				reta(a,linhaNCC+(69*pow(0.85,melhor_modelo)/2),colNCC+(69*pow(0.85,melhor_modelo)/2),linhaNCC-(69*pow(0.85,melhor_modelo)/2),colNCC+(69*pow(0.85,melhor_modelo)/2),COR(255,255,0));
		
				modelo_str = to_string(melhor_modelo);
				corrCC_str = to_string(int(100*max_corrCC));
				corrCC_str = to_string(int(100*max_corrNCC));
				
				putTxt(a,linhaCC+(69*pow(0.85,melhor_modelo)/2),colCC+(69*pow(0.85,melhor_modelo)/2)+2,modelo_str, COR(255,0,0));
				putTxt(a,linhaCC+(69*pow(0.85,melhor_modelo)/2),colCC+(69*pow(0.85,melhor_modelo)/2)+20,corrCC_str, COR(255,0,0));
				putTxt(a,linhaNCC-(69*pow(0.85,melhor_modelo)/2),colNCC+(69*pow(0.85,melhor_modelo)/2)+2,modelo_str, COR(255,255,0));
				putTxt(a,linhaNCC-(69*pow(0.85,melhor_modelo)/2),colNCC+(69*pow(0.85,melhor_modelo)/2)+20,corrCC_str, COR(255,255,0));	
				
				if (colCC > 160) { //placa esta a direita
					direcao = 1; //direcao direita
					dif_motor = colCC-160;
				}
				if (colCC < 160) { //placa esta a esquerda
					direcao = 2; 
					dif_motor = 160-colCC;
				}
			}
			
			client.sendUint(direcao);
			client.sendUint(dif_motor);
			
			imshow("janela", a);
			vo << a;
			
			
			
			//TimePoint t2 = timePoint();
			//double t = timeSpan(t1,t2);
			//cout << int(1/t) << " quadros/seg" << endl;
			
			int ch=(signed char)(waitKey(30)); // E necessario (signed char)
			if (ch == 27) {
				fecha = 1; client.sendUint(fecha);
				cout << "Conexao fechada" << endl;
				break;
			}
			else {
				fecha = 0; client.sendUint(fecha);
			}
			
		} //fecha while
		
	} //fecha caso correto
	
}	//fecha main

