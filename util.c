#define STM32L476xx

#include "stm32l476/device/stm32l4xx.h"
// #include "stm32l476/device/stm32l4xx_ll_utils.h"

void USART2_Init(void) {

  RCC->APB1ENR1 |= 0x20000; // enable USART2 clock(bit 17)
  RCC->AHB2ENR |= 1;        // enable port A clock
  GPIOA->AFR[0] = 0x0700;   // enable port A pin number 2 alternate function
                            // USART2(AF7 = 0111)
  GPIOA->MODER = 0x0020;    // Set PA2 to alternate function

  // Initialize UART
  USART2->BRR =
      0x1A1; // 9600 bps @4 Mhz // 9600 bps @8 Mhz = 0x341 //9600 bps @16 Mhz
  USART2->CR1 = 0x0008; // Enable Tx
  // First initialize UART then Enable UART(recommended)
  USART2->CR1 |= 1; // Enable USART2
}

void USART2_write_int(int ch) {
  // wait while TX buffer is empty
  while (!(USART2->ISR & 0x0080)) {
  }
  // unsigned int i = 10;
  // while(i--){
  // ch = ch - 48;
  USART2->TDR = (char)(ch & 0xFF);
  //}
}

void USART2_write(const char *data) {
  unsigned int i = 0;
  while (data[i]) {
    while (!(USART2->ISR & (1 << 7)))
      ;                    // wait for TX buffer(BIT 7) to be empty
    USART2->TDR = data[i]; // Transmit element i of data array.
    i++;                   // Increment variable for array address.
  }
}
