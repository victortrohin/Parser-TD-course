#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "LPC177x_8x.h"
#include "system_LPC177x_8x.h"
#include <retarget.h>

#include <DRV\drv_sdram.h>
#include <DRV\drv_lcd.h>
#include <DRV\drv_uart.h>
#include <DRV\drv_touchscreen.h>
#include <DRV\drv_led.h>
#include <utils\timer_software.h>
#include <utils\timer_software_init.h>
#include "parser.h"

const char at_command_simple[]="AT\r\n";				//Autobaud
const char at_command_csq[] = "AT+CSQ\r\n";				//GSM Modem signal
const char at_command_cops[] = "AT+COPS?\r\n";			//Name of network operator
const char at_command_gsn[] = "AT+GSN\r\n"; 			//Modem IMEI
const char at_command_creg[] = "AT+CREG?\r\n";  			//State of registration
const char at_command_gmi[] = "AT+GMI\r\n";				//Modem Manufacturer
const char at_command_gmr[] = "AT+GMR\r\n";				//Modem Software Version
const char at_command_ext_creg = "AT+CREG?\r\n"; 		//Cell ID and Location ID (extended AT+CREG) 

timer_software_handler_t my_timer_handler;


void timer_callback_1(timer_software_handler_t h)
{	
	
}

void testLCD()
{
	uint32_t i,j;
	LCD_PIXEL foreground = {0, 255, 0, 0};
	LCD_PIXEL background = {0, 0, 0, 0};
	
	
	for (i = 0; i < LCD_HEIGHT; i++)
	{
		for (j = 0; j < LCD_WIDTH / 3; j++)
		{
			DRV_LCD_PutPixel(i, j, 255, 0, 0);
		}
		for (j = LCD_WIDTH / 3; j < 2 * (LCD_WIDTH / 3); j++)
		{
			DRV_LCD_PutPixel(i, j, 230, 220, 0);
		}
		for (j = 2 * (LCD_WIDTH / 3); j < LCD_WIDTH; j++)
		{
			DRV_LCD_PutPixel(i, j, 0, 0, 255);
		}
	}

	DRV_LCD_Puts("Helloweeen", 20, 30, foreground, background, TRUE);
	DRV_LCD_Puts("Booo!", 20, 60, foreground, background, FALSE);	
}

void TouchScreenCallBack(TouchResult* touchData)
{
	printf("touched X=%3d Y=%3d\n", touchData->X, touchData->Y);		
	
}

void BoardInit()
{
	timer_software_handler_t handler;
	
	TIMER_SOFTWARE_init_system();
	
	
	DRV_SDRAM_Init();
	
	initRetargetDebugSystem();
	DRV_LCD_Init();
	DRV_LCD_ClrScr();
	DRV_LCD_PowerOn();	
	
	DRV_TOUCHSCREEN_Init();
	DRV_TOUCHSCREEN_SetTouchCallback(TouchScreenCallBack);
	DRV_LED_Init();
	printf ("Helloweeeeen\n");	
	
	handler = TIMER_SOFTWARE_request_timer();
	TIMER_SOFTWARE_configure_timer(handler, MODE_1, 1000, 1);
	TIMER_SOFTWARE_set_callback(handler, timer_callback_1);
	TIMER_SOFTWARE_start_timer(handler);
}

void SendCommand(const char *command)
{
 DRV_UART_FlushRX(UART_3);
 DRV_UART_FlushTX(UART_3);
 DRV_UART_Write(UART_3, (uint8_t *)command, strlen(command));
}

BOOLEAN GetCommandResponse()
{
 uint8_t ch;
 BOOLEAN ready = FALSE;
 while (ready == FALSE)
 {
		 while (DRV_UART_BytesAvailable(UART_3) > 0)
		 {
				 DRV_UART_ReadByte(UART_3, &ch);
				 if (at_command_parse(ch) != STATE_MACHINE_NOT_READY)
				 {
						ready = TRUE;
				 }
		 }
 }
 return ready;
}

void ExecuteCommand(const char *command)
{
 SendCommand(command);
}

uint32_t ExtractData(){
	char ret[3];
	ret[0] = dataCommand.data[0][5];
	if(dataCommand.data[0][6] != ','){
		ret[1] = dataCommand.data[0][6];
		ret[2] = '\0';
		return atoi(ret);
	}	
	ret[1] = '\0';
	return atoi(ret);
}

uint32_t ConvertAsuToDbmw(uint32_t rssi_value_asu){
	return 2*rssi_value_asu - 113;
}

void handle_creg(){
	int i;
	int response;
	for(i = 0; i < strlen(dataCommand.data[0]); i++){
		if(dataCommand.data[0][i] == ',')
			break;
	}
	response = dataCommand.data[0][i+1] - '0';
	switch (response)
	{
	case 0:
		printf("State of registration: not registered & not searching");
		break;
	case 1:
		printf("State of registration: Home newtwork");
		break;
	case 2:
		printf("State of registration: not registered & searching");
		break;	
	case 3:
		printf("State of registration: Denied registration");
		break;	
	case 4:
		printf("State of registration: Unknown");
		break;	
	case 5:
		printf("State of registration: Roaming");
		break;	
	default:
		printf("State of registration: Error")
		break;
	}

}

void handle_cops(){
	int i;
	char operator[] = "";
	for(i = 0; i < strlen(dataCommand.data[0]); i++){
		if(dataCommand.data[0][i] == '"')
		{
			i++;
			while(dataCommand.data[0][i] != '"')
				strncat(operator, &dataCommand.data[0][i], 1);
		}
	}
	printf("Operator name: %s\n", operator);
}


int main(void)
{
	uint32_t rssi_value_asu;
	uint32_t rssi_value_dbmw; 
	int i;
	my_timer_handler = TIMER_SOFTWARE_request_timer();
	BoardInit();
	testLCD();
	

	
	DRV_UART_Configure(UART_3, UART_CHARACTER_LENGTH_8, 115200, UART_PARITY_NO_PARITY, 1, TRUE, 3);
	DRV_UART_Configure(UART_2, UART_CHARACTER_LENGTH_8, 115200, UART_PARITY_NO_PARITY, 1, TRUE, 3);

	my_timer_handler = TIMER_SOFTWARE_request_timer();
	TIMER_SOFTWARE_configure_timer(my_timer_handler , MODE_1, 1000, 1);
	TIMER_SOFTWARE_start_timer(my_timer_handler);
	
	
	for(i = 0; i < 4; i++){
		DRV_UART_Write(UART_3, (uint8_t*)at_command_simple, strlen(at_command_simple));
		TIMER_SOFTWARE_Wait(1000);
	}
	
	while (1)
	 {
		 if(TIMER_SOFTWARE_interrupt_pending(my_timer_handler)){
			SendCommand(at_command_csq);
			 if (GetCommandResponse() == TRUE)
			 {
					rssi_value_asu = ExtractData();
					rssi_value_dbmw = ConvertAsuToDbmw(rssi_value_asu);
					printf("GSM Modem signal %d ASU -> %d dBmW\n",rssi_value_asu,rssi_value_dbmw);
			 }

			//State of registration
			SendCommand(at_command_creg);
			if (GetCommandResponse() == TRUE)
				handle_creg();

			//Operator Name
			SendCommand(at_command_cops);
			if (GetCommandResponse() == TRUE)
				handle_cops();

			//Modem IMEI
			SendCommand(at_command_gsn);
			if (GetCommandResponse() == TRUE)
				printf("Modem IMEI: %s\n", dataCommand.data[0]);

			//Modem Manufacturer
			SendCommand(at_command_gmi);
			if (GetCommandResponse() == TRUE)
				printf("Modem IMEI: %s\n", dataCommand.data[0]);

			//Modem Software Version
			//trebuie de scos versiunea
			SendCommand(at_command_gmr);
			if (GetCommandResponse() == TRUE)
				printf("Modem Software Version: %s\n", dataCommand.data[0]);


			TIMER_SOFTWARE_clear_interrupt(my_timer_handler);
		 }
	 }
	
	return 0; 
}
