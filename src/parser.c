#include "parser.h"


int countcurrent_characterar = 0;
AT_COMMAND_DATA dataCommand;
static uint16_t state = 0;

void printError(uint8_t current_character){
    printf("Error at line %d, in state %d. Received 0x%xH.", dataCommand.line_count, state, current_character);
}

STATE_MACHINE_RETURN_VALUE at_command_parse(uint8_t current_character, uint8_t check){
    switch (state)
    {
        case 0:{
            if (current_character == 0x0D){
                dataCommand.line_count = 0;
                state = 1;
                return STATE_MACHINE_NOT_READY;
            }
            printError(current_character);
            return STATE_MACHINE_READY_WITH_ERROR;
        }
        case 1:{
            if (current_character == 0x0A){
                state = 2;
                return STATE_MACHINE_NOT_READY;
            }
            printError(current_character);
            return STATE_MACHINE_READY_WITH_ERROR;
        }
        case 2:{
            if (current_character == '+' && check == 1){
                state = 3;
                return STATE_MACHINE_NOT_READY;
            }
            if(current_character == 'E' && check == 1){
                state = 9;
                return STATE_MACHINE_NOT_READY;
            }
            if(current_character == 'O' && check == 1){
                state = 16;
                return STATE_MACHINE_NOT_READY;
            }
            if(check == 0){
                state = 3;
                return STATE_MACHINE_NOT_READY;
            }

            printError(current_character);
            return STATE_MACHINE_READY_WITH_ERROR;
        }
        case 3:{
            if(current_character >= 0x20 && current_character <= 0x7e){
                if(dataCommand.line_count < AT_COMMAND_MAX_LINES - 1)
                    dataCommand.data[dataCommand.line_count][countcurrent_characterar] = current_character;
                countcurrent_characterar++;
                state = 4;
                return STATE_MACHINE_NOT_READY;
            }
            printError(current_character);
            return STATE_MACHINE_READY_WITH_ERROR;
        }
        case 4:{
            if(current_character >= 0x20 && current_character <= 0x7e){
                if(dataCommand.line_count < AT_COMMAND_MAX_LINES - 1)
                    dataCommand.data[dataCommand.line_count][countcurrent_characterar] = current_character;
                countcurrent_characterar++;
                state = 4;
                return STATE_MACHINE_NOT_READY;
            }
            if(current_character == 0x0D){
                if(dataCommand.line_count < AT_COMMAND_MAX_LINES - 1)
                    dataCommand.data[dataCommand.line_count][countcurrent_characterar] = '\0';
                dataCommand.line_count++;
                countcurrent_characterar = 0;
                state = 5;
                return STATE_MACHINE_NOT_READY;
            }
            printError(current_character);
            return STATE_MACHINE_READY_WITH_ERROR;
        }
        case 5:{
            if (current_character == 0x0A) {
                state = 6;
                return STATE_MACHINE_NOT_READY;
            }
            printError(current_character);
            return STATE_MACHINE_READY_WITH_ERROR;
        }
        case 6:{
            if(current_character == 0x0D){
                state = 7;
                return STATE_MACHINE_NOT_READY;
            }
            if(current_character == '+'){
                state = 3;
                return STATE_MACHINE_NOT_READY;
            }
            printError(current_character);
            return STATE_MACHINE_READY_WITH_ERROR;
        }
        case 7:{
            if(current_character == 0x0A){
                state = 8;
                return STATE_MACHINE_NOT_READY;
            }
            printError(current_character);
            return STATE_MACHINE_READY_WITH_ERROR;
        }
        case 8:{
            if(current_character == 'O'){
                state = 16;
                return STATE_MACHINE_NOT_READY;
            }
            if(current_character == 'E'){
                state = 9;
                return STATE_MACHINE_NOT_READY;
            }
            printError(current_character);
            return STATE_MACHINE_READY_WITH_ERROR;
        }
        case 9:{
            if(current_character == 'R'){
                state = 10;
                return STATE_MACHINE_NOT_READY;
            }
            printError(current_character);
            return STATE_MACHINE_READY_WITH_ERROR;
        }
        case 10:{
            if(current_character == 'R'){
                state = 11;
                return STATE_MACHINE_NOT_READY;
            }
            printError(current_character);
            return STATE_MACHINE_READY_WITH_ERROR;
        }
        case 11:{
            if(current_character == 'O'){
                state = 12;
                return STATE_MACHINE_NOT_READY;
            }
            printError(current_character);
            return STATE_MACHINE_READY_WITH_ERROR;
        }
        case 12:{
            if(current_character == 'R'){
                state = 13;
                return STATE_MACHINE_NOT_READY;
            }
            printError(current_character);
            return STATE_MACHINE_READY_WITH_ERROR;
        }
        case 13:{
            if(current_character == 0x0D){
                state = 14;
                return STATE_MACHINE_NOT_READY;
            }
            printError(current_character);
            return STATE_MACHINE_READY_WITH_ERROR;
        }
        case 14:{
            dataCommand.ok = 1;
            state = 0;
            return STATE_MACHINE_READY_OK;
        }
        case 16:{
            if(current_character == 'K'){
                state = 17;
                return STATE_MACHINE_NOT_READY;
            }
            printError(current_character);
            return STATE_MACHINE_READY_WITH_ERROR;
        }
        case 17:{
            if(current_character == 0x0D){
                state = 18;
                return STATE_MACHINE_NOT_READY;
            }
            printError(current_character);
            return STATE_MACHINE_READY_WITH_ERROR;
        }
        case 18:{
                dataCommand.ok = 0;
                state = 0;
                return STATE_MACHINE_READY_OK;
        }
        default:
            return STATE_MACHINE_READY_WITH_ERROR;
    }
}
