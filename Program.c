//Usaremos un PIC 16F876.
#include <16F876.h>
#device adc=10
#fuses XT, NOWDT
#use delay(clock=4000000)
#include "ds1624.c"
#include "lcd_b.c"




int desbordes = 0;
float voltaje;
float temperatura;
int refrescar=0;


#INT_TIMER1
rutina_interrupcion(){
   refrescar=1;
   set_timer1(3036);
   if (desbordes==2 | desbordes==4 | desbordes==6){
      if (desbordes==6){
         init();
         init_temp();
         temperatura=read_temp();
         temperatura=((temperatura-32)*5)/9;
         //Hacemos la lectura del puerto analogicos.         
         voltaje=read_adc();  
         voltaje=5*(float)voltaje/1024;
         
         if(voltaje<=0.5){
            //Sacamos por el diodo rojo. RA2.
            output_high(PIN_A2);
            output_low(PIN_A4);
         }
         
         if(voltaje>=4){
            //Sacamos por el diodo verde. RA4.
            output_high(PIN_A4);
            output_low(PIN_A2);
         }
         
         desbordes=0;
      }   
      else{
         //Hacemos la lectura del puerto analogico y lo transformamos en voltios.
         voltaje=read_adc();  
         voltaje=5*(float)voltaje/1024;
         
         desbordes=desbordes+1;
         
         if(voltaje<=0.5){
            //Sacamos por el diodo rojo. RA2.
            output_high(PIN_A2);
            output_low(PIN_A4);
         }
         if(voltaje>=4){
            //Sacamos por el diodo verde. RA4.
            output_high(PIN_A4);
            output_low(PIN_A2);
         }
         
      }
   }
   
   else{
      desbordes=desbordes+1;
   }
   
}

void main(){


   //Variable para contabilizar el numero de desbordamientos.
   //Configuramos el timer1 para que se desborde cada MEDIO segundo.
   //Habilitamos las interrupciones por timer1.
   //Habilitamos las interrupciones de un modo global.
   setup_adc_ports(AN0);
   setup_adc(adc_clock_div_8);
   set_adc_channel(0);
   delay_us(10);
   
   //Puerto RA0 configurado como entrada.
   set_tris_a(101011);
   
   //Puertos B, configurados como salida.
   set_tris_b(0x00);
   
   
   
   setup_timer_1(T1_INTERNAL| T1_DIV_BY_8);
   set_timer1(3036);
   enable_interrupts(GLOBAL);
   enable_interrupts(INT_TIMER1);
   
   
   
   WHILE(TRUE){
      //Mostramos por el display los valores de temperatura y de tension.
      while(refrescar!=1);
      lcd_init();
      lcd_gotoxy(1,1);
      printf(lcd_putc,"Luminosidad: %f",voltaje);
      lcd_gotoxy(1,2);
      printf(lcd_putc,"Temperatura: %f",temperatura);
      refrescar=0;
   }
}
