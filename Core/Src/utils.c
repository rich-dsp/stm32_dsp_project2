#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "q24.h"
#include "main.h"
#include "utils.h"

extern UART_HandleTypeDef huart2;
extern CmdData const G_CommandTable[];

uint8_t DEBUG_UART_IT_RX[1];


uint8_t uart_rx_write_index = 0;
uint8_t uart_rx_read_index = 0;
uint8_t CLI_Rx_buff_index = 0;

uint8_t debug_UART_RX_buff[DEBUG_UART_RX_BUFF_SIZE];
char CLI_Rx_buff[DEBUG_UART_RX_BUFF_SIZE];




uint8_t const bs_sequence[] = {0x08, 0x20, 0x08};


void print_prompt(void)
{
	printf(PROMPT);
}

char *q24_to_str(int32_t q24)
{
    uint32_t abs_q24;
    char sign = '\0';
	static char buf[Q24_STR_SIZE];


    if (q24 < 0) {
        sign = '-';
        abs_q24 = (uint32_t)(-q24);
    } else {
        abs_q24 = (uint32_t)q24;
    }

    uint32_t integer = abs_q24 >> 24;
    uint32_t fraction = abs_q24 & 0xFFFFFF;

    // 6 decimal places, rounded
    uint32_t frac_dec =
        (((uint64_t)fraction * 1000000) + (1 << 23)) >> 24;

    if (sign)
        snprintf(buf, Q24_STR_SIZE, "%c%u.%06u", sign, integer, frac_dec);
    else
        snprintf(buf, Q24_STR_SIZE, "%u.%06u", integer, frac_dec);

    return buf;
}



void clear_debug_UART_Rx_buff(void)
{
	uint8_t i;

	for (i =0; i<DEBUG_UART_RX_BUFF_SIZE; i++)
	{
		debug_UART_RX_buff[i] = 0;
	}
}



void PrintBanner(void)
{

	printf("\n\r#####################################################################\n\n\r");
	printf("STM32 DSP Learning Project\n\n\r");
	printf("Compiled on ");
	printf(__DATE__);
	printf(" at ");
	printf(__TIME__);
	printf("\n\n\rSoftware Version %s\n\n\r", FW_REV);
	printf("#####################################################################\n\n\n\r");

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart2)
	{
		handle_debug_uart_rx(huart);
	}
	// add case for ups_db uart here ..
    // else if ((huart == &huartX))...
}




void debug_uart_receive_IT(void)
{
  HAL_UART_Receive_IT(&huart2, (uint8_t *)DEBUG_UART_IT_RX, 1);
}






void handle_debug_uart_rx(UART_HandleTypeDef *huart)
{
//	HAL_GPIO_TogglePin(DEBUG_2_GPIO_Port, DEBUG_2_Pin);

	uint8_t a = 0;
//	HAL_UART_Transmit(huart, (uint8_t *)DEBUG_UART_IT_RX, 1, 0);					// echo the Rx char

	a = HAL_UART_Receive_IT(huart, (uint8_t *)DEBUG_UART_IT_RX, 1);
	if (a != 0)
	{
		printf("UART Rx ERROR!\n\r");
	}
	debug_UART_RX_buff[uart_rx_write_index++] = DEBUG_UART_IT_RX[0];
	if (uart_rx_write_index >= DEBUG_UART_RX_BUFF_SIZE)
	{
		uart_rx_write_index = 0;
	}
}


CmdArgs Handle_UART_Rx(void)
{
	uint8_t temp1 = 0;
	uint8_t rx_char;
	uint8_t CR = 0;
	CmdArgs Args;

	Args.ArgCount = 0;
	uint8_t arg_index = 0;

	int8_t num_rx_chars;

	num_rx_chars = uart_rx_write_index - uart_rx_read_index;

	if (num_rx_chars < 0)
	{
		num_rx_chars += DEBUG_UART_RX_BUFF_SIZE;
	}

	while ((num_rx_chars-- > 0) && ( CR == 0))
	{
		rx_char = debug_UART_RX_buff[uart_rx_read_index++];


		if (uart_rx_read_index >= DEBUG_UART_RX_BUFF_SIZE)
		{
			uart_rx_read_index = 0;
		}

		switch(rx_char)
		{
		    case 0x0D:
                CR = 1;			// CARRIAGE RETURN
                arg_index = 0;
				
				HAL_UART_Transmit(&huart2, &rx_char, 1, 0);					// echo the Rx char

				char *p = strtok(CLI_Rx_buff, " ");
				while ((p !=0 ) && (arg_index < MAX_ARGS))
				{
					strcpy(Args.Args[arg_index], p);
					p = strtok(0, " ");
					arg_index++;
					Args.ArgCount++;
				}

				for (temp1 = 0; temp1 < DEBUG_UART_RX_BUFF_SIZE; temp1++)				// erase RxBuff
				{
					CLI_Rx_buff[temp1] = 0;
				}
				CLI_Rx_buff_index = 0;

				if (Args.ArgCount == 0)
				{
					print_prompt();
				}

				break;
			
			case 0x08:											// Backspace
				if (CLI_Rx_buff_index > 0)
				{
					CLI_Rx_buff_index--;
					CLI_Rx_buff[CLI_Rx_buff_index] = 0;
					HAL_UART_Transmit(&huart2, &bs_sequence[0], sizeof(bs_sequence), 0xffffff);					// echo the Backspace char
				}
				break;
				
	
			case 0x1B:											// ESC

		    	for (temp1 = 0; temp1 < DEBUG_UART_RX_BUFF_SIZE; temp1++)				// erase RxBuff
		    	{
                    CLI_Rx_buff[temp1] = 0;
				}
				CLI_Rx_buff_index = 0;
				print_prompt();
				break;





				default:

				if ( CLI_Rx_buff_index < (DEBUG_UART_RX_BUFF_SIZE - 2))										// Copy char to CLI buff if not full
				{
					CLI_Rx_buff[CLI_Rx_buff_index++] = rx_char;
					HAL_UART_Transmit(&huart2, &rx_char, 1, 0);					// echo the Rx char
				}

				break;
        }
	}

  return Args;
  }


