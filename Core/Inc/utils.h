#include <stdarg.h>

#define DEBUG_UART_RX_BUFF_SIZE 40

#define FW_REV   "1.0"	// Firmware Revision


#define PROMPT "\n\rCmd > "

//#define COMMAND_INVALID 		255

//#define ARG_OFF					0
//#define ARG_ON					1
//#define ARG_VOLTAGE             2

// add more args here

// #define ARG_DEFAULT				255

#define MAX_ARGS               5
#define MAX_PARAM              (MAX_ARGS - 1)

#define MAX_ARG_SIZE           8

#define Q24_STR_SIZE 16


typedef struct {
unsigned char val;
const char *command;
const char *usage;
} CmdData;


typedef struct {
const char *arg;
const char val;
} ArgData;


typedef struct {
	uint8_t ArgCount;
	char Args[MAX_ARGS][MAX_ARG_SIZE];
} CmdArgs;




void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void debug_uart_receive_IT(void);



void handle_debug_uart_rx(UART_HandleTypeDef *huart);
void PrintMenu(void);
void print_prompt(void);
void clear_debug_UART_Rx_buff(void);
void PrintBanner(void);
char *q24_to_str(int32_t q24);

CmdArgs Handle_UART_Rx(void);
unsigned char ArgtoParam(char *arg, ArgData const * ArgTbl);

void HandleCLI(CmdArgs *c_args);
