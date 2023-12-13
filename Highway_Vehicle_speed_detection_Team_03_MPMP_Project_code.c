#include<reg51.h>
#define lcd P0

sbit IR1 = P1^4;
sbit IR2 = P1^5;
sbit led = P1^7;  

sbit rs = P2^7; // Register select              
sbit rw = P2^6; // RW            
sbit en = P2^5; // Enable

void lcd_init();
void cmd(unsigned char);
void dat(unsigned char);
void delay(unsigned int);  // Delay in milliseconds
void lcd_string(char *s);
void conv_and_disp(unsigned int a);

void main() {
    led = 0; // Output config
    lcd_init();
    lcd_string("   WELCOME   ");
    cmd(0x01); // Clear display

    while(1) {
        unsigned int time = 0;
        unsigned int speed = 0;
        int delay_count = 0;
 
        if (IR1 == 1 && IR2 == 0) { // If only IR1 is detected and IR2 is not detected
            while (IR1 == 1 && IR2 == 0) {
                delay(100);
                time++;
            }
            speed = (20 * 36) / time; // Calculating speed in km/hr (assuming distance is 20 meters)
            conv_and_disp(speed);
            if (speed > 72) { // Check for overspeed
                cmd(0x01); // Clear display
                lcd_string("OVER SPEED");
                cmd(0xc0); // Move to the second line
                lcd_string("Speed:");
                conv_and_disp(speed);
                led = 1;
                delay_count = 1000; // Delay for 1000 milliseconds (1 second)
            } else {
                cmd(0x01); // Clear display
                lcd_string("NORMAL SPEED");
                cmd(0xc0); // Move to the second line
                lcd_string("Speed:");
                conv_and_disp(speed);
                led = 0;
                delay_count = 1000; // Delay for 1000 milliseconds (1 second)
            }
        } else {
            cmd(0x01); // Clear display
            lcd_string("   WELCOME   ");
        }

        // Delay to maintain the status for some time
        while (delay_count > 0) {
            delay(1);
            delay_count--;
        }
    }
}

void lcd_init() {
    cmd(0x38);
    cmd(0x0e);
    cmd(0x06);
    cmd(0x01);
    cmd(0x80);
}

void cmd(unsigned char x) {
    lcd = x; 
    rs = 0;  
    rw = 0; 
    en = 1;   
    delay(1);
    en = 0;
}

void dat(unsigned char y) { 
    lcd = y; 
    rs = 1; 
    rw = 0; 
    en = 1; 
    delay(1);
    en = 0;
}

void lcd_string(char *s) {
    while(*s) {
        dat(*s++);
        delay(5); // Additional delay to allow LCD processing time
    }
}

void delay(unsigned int j) {
    unsigned int k;
    for (k = 0; k < j; k++) {
        TMOD = 0X01;
        TH0 = 0XFC;
        TL0 = 0X66;
        TR0 = 1;
        while (!TF0);
        TF0 = 0;
        TR0 = 0;
    }
}

void conv_and_disp(unsigned int a) {
    unsigned char c;
    int p;
    int k = 0;
    int num[16];

    num[0] = 0;
    while (a > 0) {
        num[k] = a % 10;
        a = a / 10;
        k++;
    }
    if (k != 0) {
        k--;
    }
    for (p = k; p >= 0; p--) {
        c = num[p] + 48;
        dat(c);
        delay(5); // Additional delay to allow LCD processing time
    }
    for (p = 0; p < 16 - k; p++) {
        dat(' ');
        delay(5); // Additional delay to allow LCD processing time
    }
}
