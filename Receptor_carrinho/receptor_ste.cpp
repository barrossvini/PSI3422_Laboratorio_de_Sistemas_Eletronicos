// CARRINHO

#include "mbed.h"
#include "HCSR04.h"
#include "nRF24L01P.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;


Serial pc(USBTX, USBRX);

// INFOS
// Circunferência da roda: 22cm
// OBS.: NÃO USAR OS PINOS PTA1 E PTA2 (SERVEM PARA A CONEXÃO SERIAL)

float lista[4] = {0.06f, 0.06f, 0.06f, 0.06f}; // 60ms para o período e 10ms para 

DigitalOut myled1(PTD1);  // LED Azul
DigitalOut myled2(PTB18); // LED Vermelho
DigitalOut myled3(PTB19); // LED Verde

// NRF
nRF24L01P my_nrf24l01p(PTD2, PTD3, PTC5, PTD0, PTD5, PTA13); // (mosi, miso, sck, csn, ce, irq)

// PONTE H
PwmOut motor_IN1(PTE20); // MOTOR DIREITO (A) P/ TRAS
PwmOut motor_IN2(PTE21); // MOTOR DIREITO (A) P/ FRENTE
PwmOut motor_IN3(PTE29); // MOTOR ESQUERDO (B) P/ TRAS
PwmOut motor_IN4(PTE31); // MOTOR ESQUERDO (B) P/ FRENTE

// Ultrassom HCSR04
DigitalOut echo(PTA4);
DigitalOut trig(PTA5);
HCSR04 sonar(PTA5, PTA4); // trigger, echo
int dist;

// ENCODER
//distancia/pulso = 2*pi*r/20, para diametro 6,8cm = 1,06814150222052 cm/ pulso
//furos na roda = 20
InterruptIn encodMotorA(PTA12); // MOTOR A
InterruptIn encodMotorB(PTD4);  // MOTOR B
volatile int pulsosDireita = 0;
volatile int pulsosEsquerda = 0;

void setLed(bool azul, bool vermelho, bool verde) {
    myled1 = azul; 
    myled2 = vermelho; 
    myled3 = verde;   
}

void contadorA() {
    pulsosDireita++;
    //dist = sonar.distance(CM);
}
void contadorB() {
    pulsosEsquerda++;
    //dist = sonar.distance(CM);
}

void resetPulsos() {
    pulsosDireita = 0;
    pulsosEsquerda = 0;
}

void setMotor(float IN1, float IN2, float IN3, float IN4) {
    motor_IN1 = IN1;   // MOTOR ESQUERDO (A) P/ FRENTE
    motor_IN2 = IN2;   // MOTOR ESQUERDO (A) P/ TRAS
    motor_IN3 = IN3;   // MOTOR DIREITO (B) P/ FRENTE
    motor_IN4 = IN4;   // MOTOR DIREITO (B) P/ TRAS
}

void setMotorPeriodAndWidth(float lista[]) {
    motor_IN1.period(lista[0]);
    motor_IN2.period(lista[1]);
    motor_IN3.period(lista[2]);
    motor_IN4.period(lista[3]);
}

// ------------------------------------------------------------------------------ //
// ----------------------------- FUNÇÕES DE DIREÇÃO ----------------------------- //
// ------------------------------------------------------------------------------ //

// (angulo/360)*2*pi*r --> (angulo/360)*1*pi*21.206
// ARCO DE COMPRIMENTO 21.206 CM
// 1cm = 1 pulsos arredondando
void viraGrausDireita(float angulo) {
    float dist_roda = 21.206;
    float arco = (angulo/360) * 2 * 3.14 * dist_roda;
    arco = floor(arco*1.65); // FATOR DE CORREÇÃO PARA RODA PRESA 
    resetPulsos();
    while (pulsosEsquerda <= arco) {
            motor_IN1 = 0.25;
            motor_IN2 = 0;
            motor_IN3 = 0;
            motor_IN4 = 0.25;
            motor_IN1.period_ms(0.25);
            motor_IN2.period_ms(0.25);
            motor_IN3.period_ms(0.25);
            motor_IN4.period_ms(0.25);
    }
    setMotor(0, 0, 0, 0);
    resetPulsos();
}

void viraGrausEsquerda(float angulo) {
    float dist_roda = 21.206;
    float arco = (angulo/360) * 2 * 3.14 * dist_roda;
    arco = floor(arco * 1.70);
    resetPulsos();
    while (pulsosDireita <= arco) {
        motor_IN1 = 0;
        motor_IN2 = 0.25;
        motor_IN3 = 0.25;
        motor_IN4 = 0;
        motor_IN1.period_ms(0.25);
        motor_IN2.period_ms(0.25);
        motor_IN3.period_ms(0.25);
        motor_IN4.period_ms(0.25);
    }
    setMotor(0, 0, 0, 0);
    resetPulsos();
}

void andaFrente(int pulsos) {
    resetPulsos();
    while (pulsosDireita <= pulsos && pulsosEsquerda <= pulsos ) { 
        motor_IN1 = 0.3;
        motor_IN2 = 0;
        motor_IN3 = 0.25;
        motor_IN4 = 0;
        motor_IN1.period_ms(0.25);
        motor_IN2.period_ms(0.25);
        motor_IN3.period_ms(0.25);
        motor_IN4.period_ms(0.25);
        setLed(false, true, true); // azul  
    }
    //setMotor(0, 0, 0, 0);
    resetPulsos();
}

void andaTras(int pulsos) {
    resetPulsos();
    setMotorPeriodAndWidth(lista);
    while (pulsosDireita <= pulsos & pulsosEsquerda <= pulsos ) {
        setMotor(0, 1, 0, 1);
    }
    setMotor(0, 0, 0, 0);
    resetPulsos();
}

void destino(int X, int Y, bool sinal) {
    int Y_per = Y * 0.63; // Y percorrido e fator de correção
    int X_andou = 0;
    X = X * 1.18; // Fator de correcão

    // O CARRINHO SEMPRE IRÁ ANDAR EM Y PRIMEIRO
    while (Y_per >= 0) {
        dist = sonar.distance(CM);
        if (dist > 18 || dist <= 0) {
            andaFrente(1); // Decrementa de 1 em 1
            Y_per--;
        }
        else {
            // Desvia de obstáculo
            setMotor(0, 0, 0, 0);
            wait_ms(1000);
            if (sinal == true) {
                viraGrausDireita(90);
                wait_ms(1000);
                andaFrente(23);
                setMotor(0, 0, 0, 0);
                wait_ms(1000);
                viraGrausEsquerda(90);
                X_andou += 23;
            }
            else if (sinal == false) {
                viraGrausEsquerda(90);
                wait_ms(1000);
                andaFrente(23);
                setMotor(0, 0, 0, 0);
                wait_ms(1000);
                viraGrausDireita(90);
                X_andou += 23;
            }
        }
    }

    setMotor(0, 0, 0, 0);
    wait_ms(1000); // pausa para iniciar x

    // SE TINHA QUE VIRAR PRA DIREITA
    if (sinal == true) {
        // Obs.: Se não tiver feito nenhum desvio, então X_andou = 0 (entra no else if)
        if (X_andou > X) {          // Se, depois do desvio feito, o carrinho andou mais do que deveria
            viraGrausEsquerda(90);  // Volta
            wait_ms(1000);
            andaFrente(X_andou - X);
            setMotor(0, 0, 0, 0);
        }
        else if (X_andou < X) {     // Se, depois do desvio feito, o carrinho ainda não chegou no destino em X
            viraGrausDireita(90);   // Anda mais
            wait_ms(1000);
            andaFrente(X - X_andou);
            setMotor(0, 0, 0, 0);
        }
    }

    // SE TINHA QUE VIRAR PRA ESQUERDA
    else if (sinal == false) {
        if (X_andou > X) {
            viraGrausDireita(90);
            wait_ms(1000);
            andaFrente(X_andou-X);
            setMotor(0, 0, 0, 0);
        }
        else if (X_andou < X) {
            viraGrausEsquerda(90);
            wait_ms(1000);
            andaFrente(X - X_andou);
            setMotor(0, 0, 0, 0);
        }
    }
}
 
int main() {
    resetPulsos(); // inicializa os pulsos com 0
    
    //ACIONA INTERRUPT COM BORDA DE SUBIDA PARA CONTADOR DE PULSO DAS RODAS
    encodMotorA.rise(&contadorA);
    encodMotorB.rise(&contadorB);
    
    //CODIGO PARA NRF RECEPTOR
    #define TRANSFER_SIZE   5 // 5 caracteres: 2 para coordenada X e 2 para Y (Ex.: 90p80) -> "p" = coordenada X positiva / "n" = coordenada X negativa
    char txData[TRANSFER_SIZE], rxData[TRANSFER_SIZE];
    int txDataCnt = 0;
    int rxDataCnt = 0;
 
    my_nrf24l01p.powerUp();
    pc.printf( "nRF24L01+ Frequency    : %d MHz\r\n",  my_nrf24l01p.getRfFrequency() );
    pc.printf( "nRF24L01+ Output power : %d dBm\r\n",  my_nrf24l01p.getRfOutputPower() );
    pc.printf( "nRF24L01+ Data Rate    : %d kbps\r\n", my_nrf24l01p.getAirDataRate() );
    pc.printf( "nRF24L01+ TX Address   : 0x%010llX\r\n", my_nrf24l01p.getTxAddress() );
    pc.printf( "nRF24L01+ RX Address   : 0x%010llX\r\n", my_nrf24l01p.getRxAddress() );
    pc.printf( "Type keys to test transfers:\r\n  (transfers are grouped into %d characters)\r\n", TRANSFER_SIZE );
    my_nrf24l01p.setTransferSize( TRANSFER_SIZE );
    my_nrf24l01p.setReceiveMode();
    my_nrf24l01p.enable();
    
    setLed(true, true, true); // Inicia com todos os LED apagados
    
    while (1) {
        if ( pc.readable() ) {
            txData[txDataCnt++] = pc.getc();
            if ( txDataCnt >= sizeof( txData ) ) {
                my_nrf24l01p.write( NRF24L01P_PIPE_P0, txData, txDataCnt );
                txDataCnt = 0;
            }
        }

        if ( my_nrf24l01p.readable() ) {
            rxDataCnt = my_nrf24l01p.read( NRF24L01P_PIPE_P0, rxData, sizeof( rxData ) );
            resetPulsos();
            int coordX = 0;
            int coordY = 0;
            bool sinal = true;
            if (rxData[0] > 47 && rxData[0] < 58 && rxData[1] > 47 && rxData[1] < 58) {
                coordX = (rxData[0]-48)*10 + (rxData[1]-48);
            }
            if (rxData[3] > 47 && rxData[3] < 58 && rxData[4] > 47 && rxData[4] < 58) {
                coordY = (rxData[3]-48)*10 + (rxData[4]-48);
            }
            if (rxData[2] == 'n')
                sinal = false;
            else if (rxData[2] == 'p')
                sinal = true;
            
            // Display the receive buffer contents via the host serial link
            for ( int i = 0; rxDataCnt > 0; rxDataCnt--, i++ ) {
                //pc.putc( rxData[i] );
            }
            // Toggle LED2 (to help debug nRF24L01+ -> Host communication)
            myled3 = !myled3;
            destino(coordX, coordY, sinal);
        }
    }
}
