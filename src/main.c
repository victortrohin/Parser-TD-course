#include "parser.h"

void initStruct(){
    for(int i = 0; i<AT_COMMAND_MAX_LINES; i++)
        strcpy(dataCommand.data[i], "");
    dataCommand.line_count = 0;
}

void afisareNrLinii(int linii){
    for(int i = 0; i<linii; i++)
            printf("%s\n", dataCommand.data[i]);
}

void afisare(){
    if(dataCommand.ok == 0){
        if(dataCommand.line_count > AT_COMMAND_MAX_LINES)
            afisareNrLinii(AT_COMMAND_MAX_LINES);
        else
            afisareNrLinii(dataCommand.line_count);
        printf("OK\n");
    }

    if(dataCommand.ok == 1){
        if(dataCommand.line_count > AT_COMMAND_MAX_LINES)
            afisareNrLinii(AT_COMMAND_MAX_LINES);
        else
            afisareNrLinii(dataCommand.line_count);
        printf("ERROR\n");
    }
}

int main(int argc, char *argv[]){


    if(argc < 2){
        printf("No file specified in command line.\n");
        exit(EXIT_FAILURE);
    }

    FILE* file;
    char ch;
    file = fopen(argv[1], "rb");

    if(file == NULL){
        printf("File can not be open.\n");
        exit(EXIT_FAILURE);
    }

    while(1){
        ch = fgetc(file);
        if(ch == EOF)
            break;
        STATE_MACHINE_RETURN_VALUE return_val = at_command_parse(ch, 0);
        if(return_val == STATE_MACHINE_NOT_READY){
            continue;
        }
        if(return_val == STATE_MACHINE_READY_OK){
            afisare();
        }
        if(return_val == STATE_MACHINE_READY_WITH_ERROR){
            break;
        }

        if(ch == EOF){
           break;
        }
        else{
            initStruct();
            continue;
        }
    }

    fclose(file);

    return 0;
}
