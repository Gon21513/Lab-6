/* 
 * File:   newmain.c
 * Author: Luis Pedro Gonzalez
 *
 * Created on 23 de marzo de 2023, 04:57 PM
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <xc.h>
#include <stdint.h>


#define _XTAL_FREQ 8000000





/////////////////interupciones 

void __interrupt() isr(void) {
    if (T0IF){ //   CHEUQEA LA INTERRUPCION DEL TMR0
        PORTC++; //INCREMENTO DEL PORTC
        TMR0 = 216; //VALOR DEL TMR0
        T0IF = 0 ; //LIMPIAR BANDERA
    }
    if (ADIF) { //CHEQUEA INTERUPCION DEL ADC
        if (ADCON0bits.CHS == 0b0000) {// CANAL 0 AN0
            PORTD = ADRESH;//MUEVE EL VALOR AL ADRESH
            ADCON0bits.CHS = 0b0001;// CAMBIA A CANAL 1
        } else if (ADCON0bits.CHS == 0b0001){//CHEQUEA SI EL CANAL ES 1
            PORTB = ADRESH; // MUEVE EL VALOR DEL ADRESH AL PORTB
            ADCON0bits.CHS = 0b0000;
        }
        ADIF = 0;
    }
}

////////////prototipos
void setup (void);
////////////////////principal
void main(void) {
    setup();// llama  a la funcion setup

    while(1) {
        ADCON0bits.GO = 1;/// incicia la conversacion analogia a dig
        //while (ADIF == 0);//espera que termine y revisa la bandera
        //int adc = ADRESH;//(ADRESH <<8) + ADRESL;
       // PORTD = (char)adc; //guarda el valor de 8 bits en resultado de adc en el portd
        __delay_ms(10); // Espera 10ms


    }
    return;
}

/////////////CONFIGURACION 
void setup (void){
    ANSEL = 0; 
    ANSELH = 0;
    TRISB  = 0; ///PORTB COMO SALIDA
    TRISC = 0; //PORTC COMO SALIDA
    TRISD = 0; // PORTD COMO SALIDA
    
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
        //////////////oscilador
    OSCCONbits.IRCF = 0b111 ; ///8Mhz
    OSCCONbits.SCS = 1;
    

    /////////////// tmr0
    OPTION_REGbits.T0CS = 0; //Usar Timer0 con Fosc/4
    OPTION_REGbits.PSA = 0; //Prescaler con el Timer0
    OPTION_REGbits.PS2 = 1; //Prescaler de 256
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;  
    TMR0 = 216; ///VALOR INICIAL DEL TMR0
    
    /////////Banderas e interrupciones
    INTCONbits.T0IF = 0; //interrupcion del tmr0
    INTCONbits.T0IE = 1;///interrupcion del tmr0
    INTCONbits.PEIE = 1; // habilitar interrupciones perifericas
    PIE1bits.ADIE = 1; // habilitar interrupciones de ADC
    PIR1bits.ADIF = 0; // limpiar la bandera de interrupcion del ADC
    INTCONbits.GIE = 1; //globales
    
    ////CONFIGURACION DE ADC
    ANSEL = 0b11; //configura an0 y an1 como analogicas
    ADCON0bits.ADCS = 0b10 ; ///fosc/32 
    ADCON0bits.CHS = 0b0000; ///chs 0000 an0 selecciona canal
    //ADCON0bits.CHS = 0001///chs 0001 an0 selecciona canal
    __delay_ms(1); 
    
    ///////CONFIGURACIN DEL ADC
    ADCON1bits.ADFM = 0; ///justificado a la izquuierda
    ADCON1bits.VCFG0 = 0; //vdd como referenias
    ADCON1bits.VCFG1 = 0; //vss como referencias
    ADCON0bits.ADON = 1; //inicial el adc
    __delay_us(100);
    ADIF = 0 ; //LIMPIAR LA BANDERA
}