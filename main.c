/**
 * @file main.c
 *
 * @date 23.06.2019
 * @author twatorowski (tw@mightydevices.com)
 *
 * @brief main application file
 */

/* program main function, must return int so that gcc does not complain in 
 * pedantic mode (-Wmain) */
//#include <stdio.h>

// added by yujie
#define STM32L476xx

#include "parameter_setting.h"
#include "param_fc.h"
#include "param_conv.h"
#include "test_data_id_1_label_2_.h"

// #include <stdint.h>
// #include <stdio.h>
// #include <stdlib.h>  // stdlib.h header file supports all the type casting functions in C language. But it is a non standard function.
// #include <string.h>
#include "stm32l476/device/stm32l4xx.h"
#include "stm32l476/device/system_stm32l4xx.h"
#include "stm32l476/device/system_stm32l4xx.c"
#include "stm32l476/device/stm32l4xx_ll_utils.h"
#include "stm32l476/device/stm32l4xx_ll_utils.c"
// #include "stm32l476/device/stm32l4xx_hal_rcc.h"
// #include "stm32l476/device/stm32l4xx_hal_rcc.c"

//#include "serial.h"

int32_t OUTPUT_MATRIX[R1*500];
int16_t OUTPUT_MATRIX_EXP[R1*C2];
int64_t FINAL_OUT[4];
// int16_t WEIGHT_TEST_F[640];
int64_t OUTPUT64[4000];
//param1 conv1 size 640  dim(8,80) elements 
// const int16_t param1[640];
//param2 bias1 size 8 elements
const int16_t param2[8] = {-375,170,-48,208,83,6,-1205,-696};
//param3 fc_layer size 4*4000 elements
// int16_t param3[16000];
//param4 bias2 size 4 elements
const int16_t param4[] = {433,-526,-96,189};

// added end

// added by yujie


//void USART2_Init(void)
//{
//	//NB: SYSCLK is default at MSI which is 4MHz
//	//PA2 TX
//	//PA3 Rx
//	RCC->APB1ENR1 |= 1<<17;     // enable USART2 clk source BIT 17
//	RCC->AHB2ENR |= (1<<0);     // enable GPIOA clk, port A 
//	
//	GPIOA->AFR[0] |= 0x7700;    // enable usart2 on PA3, and PA2
//	GPIOA->MODER = 0xABFFF7AF;  // PA2 and PA3 in alternate mode, PA5 output
//	
//	//USART2->BRR = 0x1A1;         // 4MHz/9600 = 1A1 hex,  pg 1356/1395 of refernce manual
//	USART2->BRR = 0x1A1;         // 16MHz/9600 = 683 hex,  pg 1356/1395 of refernce manual
//	USART2->CR1 =  (1<<2) |     // enable RX mode of USART RE (BIT 2)
//	               (1<<3);      // enable TX mode of USART TE (BIT 3)
//	USART2->CR1 |= 1<<0;        // enable USART UE BIT 0 (do after all USART config.)
//}

//void USART2_write(char *data)
//{
//	unsigned int i = 0;
//	while(data[i])
//	{
//		while(!(USART2->ISR & (1<<7)));     // wait for TX buffer(BIT 7) to be empty 
//		USART2->TDR  = data[i];             // Transmit element i of data array.
//		i++;                                // Increment variable for array address.
//	}
//}

//int USART2_write_int(int data)
//{
//        while(!(USART2->ISR & (1<<7)));     // wait for TX buffer(BIT 7) to be empty 
//        USART2->TDR  = data & 0XFF;             // Transmit element i of data array.
//        return data;
//}

// void SysClock_Config(void)
// {
// 	RCC->CR = RCC_CR_MSION;  // MSI ON
// 	RCC->CFGR &=~ RCC_CFGR_SW; // select MSI as clk source
// 	while((RCC->CR & RCC_CR_MSIRDY) == 0);  // wait for MSI to be ready
// 	RCC->CR &=~ RCC_CR_MSIRANGE;            // clear the range register
// 	RCC->CR |= RCC_CR_MSIRANGE_8;           // sel. 16MHz range
// 	RCC->CR |= RCC_CR_MSIRGSEL;             // Use the  MSIRANGE  in CR 
// 	while((RCC->CR & RCC_CR_MSIRDY) == 0);  // wait for MSI to be ready
// }


void USART2_Init(void){
	
	RCC->APB1ENR1 |= 0x20000;  // enable USART2 clock(bit 17)
	RCC->AHB2ENR |= 1; //enable port A clock
	GPIOA->AFR[0] = 0x0700; // enable port A pin number 2 alternate function USART2(AF7 = 0111)
	GPIOA->MODER = 0x0020; //Set PA2 to alternate function
	
	//Initialize UART
	USART2->BRR = 0x1A1; //9600 bps @4 Mhz // 9600 bps @8 Mhz = 0x341 //9600 bps @16 Mhz
	USART2->CR1 = 0x0008; //Enable Tx
	//First initialize UART then Enable UART(recommended)
	USART2->CR1 |= 1; //Enable USART2
	
}

void USART2_write_int(int ch){
	// wait while TX buffer is empty
		while(!(USART2->ISR & 0x0080)){}
		unsigned int i = 10;
		//while(i--){
			//ch = ch - 48;
		USART2->TDR = (char) (ch & 0xFF);
		//}
}

void USART2_write(char *data)
{
	unsigned int i = 0;
	while(data[i])
	{
		while(!(USART2->ISR & (1<<7)));     // wait for TX buffer(BIT 7) to be empty 
		USART2->TDR  = data[i];             // Transmit element i of data array.
		i++;                                // Increment variable for array address.
	}
}



void microspeech_conv_layer(){
    
    for (int i=0;i<R1; i++) {
        for (int j=0;j<C2; j++) {
            int16_t temp=0;
            for (int k=0;k<C1; k++){
                temp += WEIGHT_MATRIX[i*C1+k]*INPUT_MATRIX[k*C2+j];
            }
 	        // OUTPUT_MATRIX_[i*C2+j] = temp;
            if(j<500){
                OUTPUT_MATRIX[(i)*500+(j)]=temp;
            }
        }
    }

}

void microspeech_bias_ReLu() {
    for (int i=0; i<4000; i++) {
        int indx = i/500;
            OUTPUT_MATRIX[i] += param2[indx];
    }
}

void quantize_conv_layer() {
    int i, j;
    int sum_weight[8];
    for (int q=0; q<8; q++) {
        sum_weight[q] = 0;
    }
    for(i=0; i<R1; i++) {
        for (j=0; j<C1; j++) {
            sum_weight[i] += WEIGHT_MATRIX[i*C1 + j];
        }
    }

    for (i=0; i<4000; i++) {
        int ind = (int) i/500;
        OUTPUT_MATRIX[i] = OUTPUT_MATRIX[i] + (128*sum_weight[ind]);
    }
}

void quantize_fc_layer() {
    int i, j;    
    int sum_weight[8];
    for (int q=0; q<8; q++) {
        sum_weight[q] = 0;
    }
    for(i=0; i<4000; i++) {
        for (j=0; j<4; j++) {
            sum_weight[j] += DENSE_MATRIX[i*4 + j];
        }
    }

    for (i=0; i<4; i++) {
        FINAL_OUT[i] = FINAL_OUT[i] + (128*sum_weight[i]);
    }
}

void requantize_conv() {

    int64_t factor[8] = {1674350931,1535919972,2026360758,1174747100,1517546873,1302070198,1086796819,1779030479};
    int64_t factor2[8] = {1099511627776,4398046511104,1099511627776,1099511627776,1099511627776,1099511627776,549755813888,1099511627776};
    int64_t factor3[8] = {41,43,41,41,41,41,40,41};

    for (int i=0; i<4000; i++) {
        int ind = i/500;
        OUTPUT64[i] = OUTPUT_MATRIX[i]*factor[ind] + factor2[ind];
    }

    for (int i=0; i<4000; i++) {
        int ind = i/500;
        OUTPUT64[i] = OUTPUT64[i]>>factor3[ind];
        OUTPUT64[i] +=  -128;
        if (OUTPUT64[i] < -128) {
            OUTPUT_MATRIX[i] = -128;
        }
        else if (OUTPUT64[i] > 127) {
            OUTPUT_MATRIX[i] = 127;
        }
        else {
            OUTPUT_MATRIX[i] = OUTPUT64[i];
        }
    }
 
    // int OUTPUT_TEST_F[4000];
    // int count = 0;
    // for (int j=0; j<500; j++) {
    // for (int i=0; i<8; i++) {
    //     OUTPUT_TEST_F[count++] = OUTPUT_MATRIX[i*500 + j];
    //     }
    // }   
}

int requantize_fc() {

    int FINAL[4];
    for(int i=0; i<4; i++) {
        FINAL[i] = FINAL_OUT[i] + param4[i];
    }

    long long int multiplier=1780495384;
    long long int shift=-11;
    int total_right_shift = shift + 31;
    // long long int FINAL64[4];
    int pos_rounding_val = 1 << (total_right_shift);

    // fixed_point_multiply(x, fixed_point_multiplier, right_shift) 
    // x = cast(x,int64) * fixed_point_multiplier 
    // total_right_shift = right_shift + 31 
    // pos_rounding_value = 1 << (total_right_shift -1) 
    // x = x + pos_rounding_value 
    // x = x >> total_right_shift return cast(x, int32)

//USART2_write("Output Label9:\n");

    int FINAL8[4];
    // int FIN[4];
    for (int i=0; i<4; i++) {
        FINAL8[i] = (FINAL[i] * multiplier + pos_rounding_val) >> (total_right_shift + 1);
        FINAL8[i] += pos_rounding_val;       
        FINAL8[i] = FINAL8[i] >> (total_right_shift + 1);
    }

//USART2_write("Output Label10:\n");

    for (int i=0; i<4; i++) {
        FINAL_OUT[i] = FINAL8[i] + 19;
        if (FINAL_OUT[i] < -128) {
            FINAL_OUT[i] = -128;
        }
        else if (FINAL_OUT[i] > 127) {
            FINAL_OUT[i] = 127;
        }
        else {
            FINAL_OUT[i] = FINAL_OUT[i];
        }
    }

    for (int i=0; i<4; i++) {
        FINAL_OUT[i] -= 19;
    }

//USART2_write("Output Label11:\n");

    float FINAL_FLOAT[4];
    float max_val = -13456.0;
    int max_idx = -1;

//USART2_write("Output Label12:\n");

    for (int i=0; i<4; i++) {
//USART2_write("Output Label14--:\n");
        FINAL_FLOAT[i] = FINAL_OUT[i] * 0.0979961;
//int a = 53;
//USART2_write_int(a);
//USART2_write("\n0.0979961\n");
//USART2_write("\n Output Label14:\n");

//USART2_write_int(max_val);
//USART2_write_int(FINAL_FLOAT[i]);

        if (max_val < FINAL_FLOAT[i]) {
//USART2_write("Output Label15:\n");
            max_val = FINAL_FLOAT[i];
            max_idx = i;
        }

//USART2_write("Output Label15++:\n");
    }
    // printf("Obtained label:%d\n", max_idx); 
//USART2_write("Output Label13:\n");

    return max_idx;   
}

int OUTPUT_MATRIX_COPY[4000];

void reshape_conv_output() {

   int i;
   for(i=0; i<4000; i++) {
    OUTPUT_MATRIX_COPY[i] = OUTPUT_MATRIX[i];
   }
   int c1 = 0;
   for(i=0; i<500; i++) {
    for (int j=0; j<8; j++) {
        OUTPUT_MATRIX[c1++] = OUTPUT_MATRIX_COPY[i + j*500];
    }
   }  
}


void microspeech_fc_layer(){
    int i,j,k;

    for (i=0; i<4; i++) {
        FINAL_OUT[i] = 0;
    }

    for (i=0;i<1; i++) {
        for (j=0;j<C3; j++) {
            for (k=0;k<R3; k++){
                FINAL_OUT[i*C3+j] += OUTPUT_MATRIX[i*R3+k]*DENSE_MATRIX[k*C3+j];
            }
        }
    }
}

// added end

void usart2_write_interval(int STCVR1, int STCVR2){

    char time_string[32];
    itoa(STCVR1, time_string, 10);
    USART2_write("STCVR1:");
    USART2_write(time_string);
    USART2_write("\t");

    itoa(STCVR2, time_string, 10);
    USART2_write("STCVR2:");
    USART2_write(time_string);
    USART2_write("\t");
    
    itoa(STCVR1-STCVR2-2, time_string, 10);
    USART2_write("Cycles Consumed:");
    USART2_write(time_string);
    USART2_write("\t");

    itoa((STCVR1-STCVR2-2)/80, time_string, 10);
    USART2_write("Time Consumed:");
    USART2_write(time_string);
    USART2_write("\t");    
}


int Main(void)
{
    LL_SetSystemCoreClock(80000000);
    int obtained_label = 0;
    // char time_string[32];
    // int STCVR1 = 0;
    // int STCVR2 = 0;

	// USART2_Init();
    

    // int *STCSR = (int *)0xE000E010;                    
    // int *STRVR = (int *)0xE000E014;              
    // int *STCVR = (int *)0xE000E018;


    // for(int i=0;i<1;i++){

        // // Configure Systick    
        // *STRVR = 0xFFFFFF;  // max count
        // *STCVR = 0;         // force a re-load of the counter value register
        // *STCSR = 5;         // enable FCLK count without interrupt
        // // STCVR1 = *STCVR;
        // STCVR1 = 16777216;
        microspeech_conv_layer();
        // STCVR2 = *STCVR;
        // usart2_write_interval(STCVR1, STCVR2);

        // // Configure Systick    
        // *STRVR = 0xFFFFFF;  // max count
        // *STCVR = 0;         // force a re-load of the counter value register
        // *STCSR = 5;         // enable FCLK count without interrupt
        // // STCVR1 = *STCVR;        
        quantize_conv_layer();
        // STCVR2 = *STCVR;
        // usart2_write_interval(STCVR1, STCVR2);

        // // Configure Systick    
        // *STRVR = 0xFFFFFF;  // max count
        // *STCVR = 0;         // force a re-load of the counter value register
        // *STCSR = 5;         // enable FCLK count without interrupt
        // // STCVR1 = *STCVR;    
        microspeech_bias_ReLu();
        // STCVR2 = *STCVR;
        // usart2_write_interval(STCVR1, STCVR2);

        // // Configure Systick    
        // *STRVR = 0xFFFFFF;  // max count
        // *STCVR = 0;         // force a re-load of the counter value register
        // *STCSR = 5;         // enable FCLK count without interrupt
        // // STCVR1 = *STCVR;        
        requantize_conv();
        // STCVR2 = *STCVR;
        // usart2_write_interval(STCVR1, STCVR2);

        // // Configure Systick    
        // *STRVR = 0xFFFFFF;  // max count
        // *STCVR = 0;         // force a re-load of the counter value register
        // *STCSR = 5;         // enable FCLK count without interrupt
        // // STCVR1 = *STCVR;        
        reshape_conv_output();
        // STCVR2 = *STCVR;
        // usart2_write_interval(STCVR1, STCVR2);

        // // Configure Systick    
        // *STRVR = 0xFFFFFF;  // max count
        // *STCVR = 0;         // force a re-load of the counter value register
        // *STCSR = 5;         // enable FCLK count without interrupt
        // // STCVR1 = *STCVR;        
        microspeech_fc_layer();
        // STCVR2 = *STCVR;
        // usart2_write_interval(STCVR1, STCVR2);

        // // Configure Systick    
        // *STRVR = 0xFFFFFF;  // max count
        // *STCVR = 0;         // force a re-load of the counter value register
        // *STCSR = 5;         // enable FCLK count without interrupt
        // // STCVR1 = *STCVR;        
        quantize_fc_layer();    
        // STCVR2 = *STCVR;
        // usart2_write_interval(STCVR1, STCVR2);

        // // Configure Systick    
        // *STRVR = 0xFFFFFF;  // max count
        // *STCVR = 0;         // force a re-load of the counter value register
        // *STCSR = 5;         // enable FCLK count without interrupt
        // // STCVR1 = *STCVR;        
        obtained_label = requantize_fc();
        // STCVR2 = *STCVR;
        // usart2_write_interval(STCVR1, STCVR2);

    // }


    // int fclk_frequency=SystemCoreClock;

    // itoa(fclk_frequency, time_string, 10);
    // USART2_write("Time frequency:");
    // USART2_write(time_string);
    // USART2_write("\n");

    // itoa(obtained_label, time_string, 10);
    // USART2_write("Output Label:");
    // USART2_write(time_string);
    // USART2_write("\n");

	return 0;
}
