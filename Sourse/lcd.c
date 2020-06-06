#include "lcd.h"

#include <stdarg.h>
#include <stdio.h>

extern void delay(int);

// #define RS 7
// #define RW 10
// #define EN 11
// #define DAT 12


void delay_ms(uint32_t ms){
	delay((ms*16000));
}
void delay_us(uint32_t us){
	delay((us*16));
}

void lcd_send(uint8_t value, uint8_t mode);
void lcd_write_nibble(uint32_t nibble);

static uint8_t lcd_displayparams;
static char lcd_buffer[LCD_COL_COUNT + 1];

void lcd_command(uint8_t command) {
  lcd_send(command, 0);
}

void lcd_write(uint8_t value) {
  lcd_send(value, 1);
}

void lcd_send(uint8_t value, uint8_t mode) {
	if(mode) LCD_GPIO->BSRRL = _BV(RS); 
	else LCD_GPIO->BSRRH = _BV(RS);
	lcd_write_nibble(value>>4);
	lcd_write_nibble(value);

}

void lcd_write_nibble(uint32_t nibble) {
	LCD_GPIO->BSRRH = _BV(RW);

	if (nibble & 1) LCD_GPIO->BSRRL = _BV(D4); else LCD_GPIO->BSRRH = _BV(D4);
	if (nibble & 2) LCD_GPIO->BSRRL = _BV(D5); else LCD_GPIO->BSRRH = _BV(D5);
	if (nibble & 4) LCD_GPIO->BSRRL = _BV(D6); else LCD_GPIO->BSRRH = _BV(D6);
	if (nibble & 8) LCD_GPIO->BSRRL = _BV(D7); else LCD_GPIO->BSRRH = _BV(D7);

	// nibble &=0xf; // preserve 4 bits
	// nibble<<=12;  // set to be bits 12-15
	// GPIOE->BSRR = 0xf<<28; // clr 4 bits
	// GPIOE->BSRR = nibble;  // write em
	
	LCD_GPIO->BSRRL = _BV(EN); // set EN
	delay_us(100);
	LCD_GPIO->BSRRH = _BV(EN);//clr en
	delay_us(100);
}

void lcd_init(void) {

	delay_ms(40);
	LCD_GPIO->MODER |= _BV(RS*2) | _BV(RW*2) | _BV(EN*2) | _BV(D4*2) | _BV(D5*2) | _BV(D6*2) | _BV(D7*2) ;
	LCD_GPIO->BSRRH=_BV(RW);//RW=W
	LCD_GPIO->BSRRH=_BV(RS);;
	delay_ms(40);
	lcd_write_nibble(0x3);
	delay_ms(15);
	lcd_write_nibble(0x3);
	delay_ms(15);
	lcd_write_nibble(0x3);
	delay_ms(15);
	lcd_write_nibble(0x2);
	delay_ms(15);
	lcd_send(0x28,0);
	delay_ms(15);
	lcd_send(0xc,0);
	delay_ms(15);
	lcd_send(0x6,0);
	delay_ms(15);
	lcd_clear();
	lcd_return_home();

}

void lcd_on(void) {
  lcd_displayparams |= LCD_DISPLAYON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_off(void) {
  lcd_displayparams &= ~LCD_DISPLAYON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_clear(void) {
  lcd_command(LCD_CLEARDISPLAY);
  delay_ms(2);
}

void lcd_return_home(void) {
  lcd_command(LCD_RETURNHOME);
  delay_ms(2);
}

void lcd_enable_blinking(void) {
  lcd_displayparams |= LCD_BLINKON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_disable_blinking(void) {
  lcd_displayparams &= ~LCD_BLINKON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_enable_cursor(void) {
  lcd_displayparams |= LCD_CURSORON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_disable_cursor(void) {
  lcd_displayparams &= ~LCD_CURSORON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_scroll_left(void) {
  lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void lcd_scroll_right(void) {
  lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void lcd_set_left_to_right(void) {
  lcd_displayparams |= LCD_ENTRYLEFT;
  lcd_command(LCD_ENTRYMODESET | lcd_displayparams);
}

void lcd_set_right_to_left(void) {
  lcd_displayparams &= ~LCD_ENTRYLEFT;
  lcd_command(LCD_ENTRYMODESET | lcd_displayparams);
}

void lcd_enable_autoscroll(void) {
  lcd_displayparams |= LCD_ENTRYSHIFTINCREMENT;
  lcd_command(LCD_ENTRYMODESET | lcd_displayparams);
}

void lcd_disable_autoscroll(void) {
  lcd_displayparams &= ~LCD_ENTRYSHIFTINCREMENT;
  lcd_command(LCD_ENTRYMODESET | lcd_displayparams);
}

void lcd_create_char(uint8_t location, uint8_t *charmap) {
  lcd_command(LCD_SETCGRAMADDR | ((location & 0x7) << 3));
  for (int i = 0; i < 8; i++) {
    lcd_write(charmap[i]);
  }
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
  static uint8_t offsets[] = { 0x00, 0x40, 0x14, 0x54 };

  lcd_command(LCD_SETDDRAMADDR | (col + offsets[row]));
}

void lcd_puts(char *string) {
  for (char *it = string; *it; it++) {
    lcd_write(*it);
  }
}

 void lcd_printf(char *format, ...) {
   va_list args;

   va_start(args, format);
   vsnprintf(lcd_buffer, LCD_COL_COUNT + 1, format, args);
   va_end(args);

   lcd_puts(lcd_buffer);
 }
