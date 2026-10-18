#include "main.h"
#include "utils.h"


#include <string.h>
#include <stdio.h>
#include <stdlib.h>





//extern ArgData const G_ArgTable[];

CmdArgs CmdStruct;



enum Arg_1   {
	ARG_OFF,						\
	ARG_ON,							\
	ARG_VOLTAGE,					\
	ARG_F_NORM,						\
	ARG_K,							\
	ARG_SIN,						\
	ARG_SQR,						\
	ARG_DEFAULT
};


ArgData const G_ArgTable[] = {
	{"off",		ARG_OFF},
	{"on",		ARG_ON},
	{"v",		ARG_VOLTAGE},
	{"f",       ARG_F_NORM},
	{"k",       ARG_K},
	{"sin",     ARG_SIN},
	{"sqr",     ARG_SQR},
	{"null",	ARG_DEFAULT}

};





enum UartCmd_1   {

	UART_CMD_MENU, 						\
	UART_GUI_CONNECT,					\
	UART_CMD_SWVER, 					\
    UART_CMD_BUILD_DATE_AND_TIME,       \
	UART_CMD_SET,						\
	UART_CMD_STIM,						\
	UART_CMD_CONFIG,					\
	UART_CMD_FILT,						\
	COMMAND_INVALID						\
};


CmdData const G_CommandTable[] = {
	{UART_CMD_MENU, 				"h", 				"h          - help"},
	{UART_GUI_CONNECT,				"connect",			"connect    - gui connect (not implemented) (1 - connect, 0 - disconnect)"},
	{UART_CMD_SWVER, 				"ver",				"ver        - print sw version"},
	{UART_CMD_BUILD_DATE_AND_TIME, 	"build", 			"build      - print build date and time"},
	{UART_CMD_SET, 					"set",				"set        - set parameter [k,f]"},
	{UART_CMD_STIM, 				"stim",				"stim       - stimulus [sin,sqr]"},
	{UART_CMD_CONFIG,				"config",			"config     - print configuration"},
	{UART_CMD_FILT,					"filt",				"filt       - print filter parameters"},
	{COMMAND_INVALID,				"null",				"null       - null command"}
};


unsigned char ArgtoParam(char *arg, ArgData const * ArgTbl)
{
	unsigned char value = ARG_DEFAULT;		// set default return value

	do
	{
		if ((strcmp(arg, ArgTbl->arg) == 0))
		{
			value = ArgTbl->val;
			break;
		}
		ArgTbl++;
	} while (ArgTbl->val != ARG_DEFAULT);

	return(value);
}



void HandleCLI(CmdArgs *c_args)
{
//	uint8_t j;
	uint8_t arg;
	float f_p0;
//	uint32_t i_p0;

	uint8_t temp1;

	uint8_t Command = COMMAND_INVALID;
	CmdData const * CmbTbl = G_CommandTable;



	if (c_args->ArgCount > 0)
	{
		do
		{
			if ((strcmp(&c_args->Args[0][0], CmbTbl->command) == 0))
			{
				Command = CmbTbl->val;
				break;
			}
			CmbTbl++;
		} while(CmbTbl->val != COMMAND_INVALID);


		switch(Command)
		{
			case UART_CMD_MENU:
				PrintMenu();
			 	break;

		  	case UART_GUI_CONNECT:
				arg = atoi(&c_args->Args[1][0]);
//			  gui_connect(arg);
            // not implemented
				break;


		  	case UART_CMD_SWVER:
				printf("\n\r%s\n\r", FW_REV);
				break;

			case UART_CMD_BUILD_DATE_AND_TIME:
				printf("\n\r%s %s\n\r", __DATE__, __TIME__);
				break;

			case UART_CMD_SET:
				if (c_args->ArgCount < 3)
				{
					printf("\n\rNot enough arguments for 'set' command");
					break;
				}

				arg = ArgtoParam(&c_args->Args[1][0], G_ArgTable);
				f_p0 = atof(&c_args->Args[2][0]);

				switch(arg)
				{
					case ARG_K:
						set_k(q24_from_float(f_p0));
						break;

					default:
						printf("\n\rUnrecognised argument '%s'", &c_args->Args[1][0]);
						break;
				}
				break;

			case UART_CMD_STIM:
				if (c_args->ArgCount < 3)
				{
					printf("\n\rNot enough arguments for 'stim' command");
					break;
				}
				arg = ArgtoParam(&c_args->Args[1][0], G_ArgTable);
				f_p0 = atof(&c_args->Args[2][0]);
				printf("\n\rStimulus command received: %d with value %f", arg, f_p0);

				switch(arg)
				{
					case ARG_SIN:
						set_stim(SIN, q24_from_float(f_p0));
						break;

					case ARG_SQR:
						set_stim(SQR, q24_from_float(f_p0));
						break;

					default:
						printf("\n\rUnrecognised argument '%s'", &c_args->Args[1][0]);
						break;
				}
				break;
			
			case UART_CMD_CONFIG:
				print_config();
				break;

			case UART_CMD_FILT:
				print_filter_params();
				break;

			case COMMAND_INVALID:

			default:
		    	printf("\n\rUnrecognised command '%s'", &c_args->Args[0][0]);
				break;


		}
//		printf("Hello from CLI");	
		print_prompt();


	}


}

void PrintMenu(void)
{

	CmdData const * CmbTbl = G_CommandTable;

	printf("\n\r\n\rKP NG COMMAND MENU  - case sensitive\n\r\n\r");
	printf("Numerical values are decimal\n\r\n\r");

	do
	{
		printf("%s\n\r", CmbTbl->usage);
		CmbTbl++;
	} while(CmbTbl->val != COMMAND_INVALID);

}
