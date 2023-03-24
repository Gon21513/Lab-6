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



//void read_analog_channel(void);


/////////////////interupciones 

void __interrupt() isr(void) {
    if (T0IF){
        PORTC++; //INCREMENTO DEL PORTD
        TMR0 = 216; //VALOR DEL TMR0
        T0IF = 0 ;
    }
}

////////////prototipos
void setup (void);
////////////////////principal
void main(void) {
    setup();// llama  a la funcion setup

    while(1) {
        ADCON0bits.GO = 1;/// incicia la conversacion analogia a dig
        while (ADIF == 0);//espera que termine y revisa la bandera
        int adc = ADRESH;//(ADRESH <<8) + ADRESL;
        PORTD = (char)adc; //guarda el valor de 8 bits en resultado de adc en el portd
        __delay_ms(10); // Espera 10ms


    }
    return;
}

/////////////CONFIGURACION 
void setup (void){
    ANSEL = 0; 
    ANSELH = 0;
    TRISC = 0;
    TRISD = 0;
    TRISB = 0;
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
    INTCONbits.GIE = 1; //globales
    
    ////CONFIGURACION DE ADC
    ANSEL = 0b01;
    TRISA = 0b01;
    ADCON0bits.ADCS = 0b10 ; ///fosc/32 
    ADCON0bits.CHS = 0; ///chs 0000 an0 selecciona canal
    __delay_ms(1); 
    
    ///////CONFIGURACIN DEL ADC
    ADCON1bits.ADFM = 0; ///justificado a la izquuierda
    ADCON1bits.VCFG0 = 0; //vdd como referenias
    ADCON1bits.VCFG1 = 0; //vss como referencias
    ADCON0bits.ADON = 1; //inicial el adc
    ADIF = 0 ; //LIMPIAR LA BANDERA
}