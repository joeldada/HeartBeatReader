#define switch PORTC.F3
#define led PORTC.F2
sbit LCD_RS at RB2_bit;
sbit LCD_EN at RB3_bit;
sbit LCD_D4 at RB4_bit;
sbit LCD_D5 at RB5_bit;
sbit LCD_D6 at RB6_bit;
sbit LCD_D7 at RB7_bit;

sbit LCD_RS_Direction at TRISB2_bit;
sbit LCD_EN_Direction at TRISB3_bit;
sbit LCD_D4_Direction at TRISB4_bit;
sbit LCD_D5_Direction at TRISB5_bit;
sbit LCD_D6_Direction at TRISB6_bit;
sbit LCD_D7_Direction at TRISB7_bit;
  unsigned char lcd[] = "000";
unsigned short pulserate, pulsecount;
  unsigned long Vin, mV,Vdec,Vfrac;
unsigned char op[12];
unsigned char m,i,j,lcd1[] = "00000" ,ch1 = 0,ch2 = 0;
char uart_rd;
unsigned char sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout);



void power_on(){

    unsigned char answer=0;

    // checks if the module is started
    answer = sendATcommand("AT\r", "OK", 2000);
    if (answer == 0)
    {
    //Lcd_Chr(1, 1, '0');
        // power on pulse
        PORTC.F5 = 1;
        delay_ms(3000);
        PORTC.F5 = 0;
        delay_ms(2000);
        // waits for an answer from the module
        while(answer == 0){     // Send AT every two seconds and wait for the answer
      //      Lcd_Chr(1, 3, 'L');
            answer = sendATcommand("AT\r", "OK", 2000);
        //    Lcd_Chr(1, 3, ' ');
        }
    }
    
   // Lcd_Chr(1, 1, '1');

}

void countpulse(){
//PORTC.F1 = 1;
 led = 1;
 delay_ms(500);
 TMR0=0;
 Delay_ms(15000);  // Delay 15 Sec
 led = 0;
 pulsecount = TMR0;
 pulserate = pulsecount*4;
}


void display(){
  lcd[2] = (pulserate%10) + 48;

  lcd[1] = ((pulserate/10)%10) + 48;

  lcd[0] = (pulserate/100) + 48;
  Lcd_Cmd(_LCD_CLEAR);               // Clear display
  Lcd_Out(1,1,"BPM = ");
  Lcd_Out(1,7,lcd);
  Lcd_Out(1,10," bpm");
}


void main() {

TRISA = 255; // RA4/T0CKI input, RA5 is I/P only
 OPTION_REG = 0b00101000; // Prescaler (1:1), TOCS =1 for counter mode

 TRISC.F5 = 0;
 PORTC.F5 = 0;
   INTCON.GIE = 1;
     INTCON.PEIE = 1;
     PIE1.RCIE=1;    //enable receive interrupt

   UART1_Init(9600);               // Initialize UART module at 9600 bps
  Delay_ms(100);

    Lcd_Init();                        // Initialize LCD

  Lcd_Cmd(_LCD_CLEAR);               // Clear display
  Lcd_Cmd(_LCD_CURSOR_OFF);          // Cursor off
  Lcd_Out(1,2,"DESIGNED BY");                 // Write text in first row

  Lcd_Out(2,2,"ALLI GBOLAHAN");                 // Write text in second row
  Delay_ms(4000);
  Lcd_Cmd(_LCD_CLEAR);               // Clear display

 // Lcd_Out(1,2,"SUPERVISED BY");                 // Write text in first row

  Lcd_Out(2,2,"DR. FAKOLUJO");                 // Write text in second row
  Delay_ms(4000);
  Lcd_Cmd(_LCD_CLEAR);
  pulserate = 0;               // Clear display
   display();
 delay_ms(3000);
// Lcd_Cmd(_LCD_CLEAR);

//power_on();

//PORTC.F5 = 1;
//delay_ms(3000);
//PORTC.F5 = 0;
 //  Lcd_Cmd(_LCD_CLEAR);
   power_on();
   
   while(1) {
   sendATcommand("ATD08053201102;\r", "OK", 2000);
   delay_ms(10000);
   }
//delay_ms(7000);

//sendATcommand("AT\r", "OK", 2000);

//power_on();
 while(1);
     /*
    while( UART1_Data_Ready()) UART1_Read() ;   // Clean the input buffer
    Lcd_Out(2,1,"R  ");
    delay_ms(1000);
    UART1_Write_Text("AT\r");
    
    while (1) {                     // Endless loop
    if (UART1_Data_Ready()) {     // If data is received,
      uart_rd = UART1_Read();     // read the received data,
  Lcd_Chr_Cp(uart_rd);    
  Lcd_Chr_Cp('8');

  }
  }
   */


while(1) {
 if (switch == 0) {
  Lcd_Out(1,7,"   ");
  Lcd_Out(1,10," bpm");

  for (m = 6; m>0; m--) {
  Lcd_Out(1,1,"heart ");
  Lcd_Chr_Cp(48+m-1);
  delay_ms(1000);

  }
  countpulse();
  display();

 }

}
}



unsigned char sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){

    unsigned char x=0,  answer=0;
    char response[20];
    unsigned long previous = 0;
   // Lcd_Out(2,14,"  ");
    memset(response, '\0', 20);    // Initialize the string

    delay_ms(100);

    while( UART1_Data_Ready()) UART1_Read() ;   // Clean the input buffer
  // Send the AT command
    UART1_Write_Text(ATcommand);


    x = 0;
          Lcd_Out(2,1,"R =");
    delay_ms(1);
    //previous++;

    // this loop waits for the answer
    do{
    delay_ms(1);
    previous++;

        if(UART1_Data_Ready()){
            // if there are data in the UART input buffer, reads it and checks for the asnwer
            response[x] = UART1_Read();

  Lcd_Chr_Cp(response[x]);
            x++;
            // check if the desired answer  is in the response of the module
            if (strstr(response, expected_answer) != 0)
            {
            Lcd_Chr_Cp('#');
                answer = 1;
            }
        }
         // Waits for the asnwer with time out
    }while((answer == 0) && ((previous) < timeout));
    Lcd_Out(2,14,"Rt");
    return answer;
}
