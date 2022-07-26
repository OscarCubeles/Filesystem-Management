#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

#include "FatSystem.h"
#include "Ex2System.h"


#define ERROR_CODE_INPUT 0
#define ERROR_CODE_OPERATION 1
#define ERROR_CODE_VOLUME 2
#define NUM_OPERATIONS 3
#define ERROR_INPUT "\nInvalid number of arguments. The valid formats are:\n\n./shooter <operation> <volume_name>\n./shooter <operation> <volume> <file_name>\n"
#define ERROR_OPERATION "\nInvalid Operation. The valid operations are: \n\n/find\n/delete\n/info\n"
#define OPERATIONS "/find", "/info", "/delete"
#define ERROR_FILE "Unable to open volume file"
#define ERROR_CODES  ERROR_INPUT, ERROR_OPERATION, ERROR_FILE

const char* VALID_OPERATIONS[] ={OPERATIONS};
const char* ERROR_STRINGS[] = {ERROR_CODES};


void DISPLAY_displayError(int errorCode);
void DISPLAY_displayOperation(char *operation, char *volume_name);
int isNotValidOperation(char *operation);
int isNotValidInput(int argc, char *argv[]);
void handle_sigsegv()
{
    printf("An error occurred. Invalid number of arguments\n");
}

int main(int argc, char *argv[]){
  char *operation;
  char *volume_name;
  char *file;
  int volume_fd;

  // Terminate the program if the number of arguments is not correct or the operation is invalid
  if (isNotValidInput(argc, argv)){
      return 0;
  }

  // Error handling signal
  signal(SIGSEGV, handle_sigsegv);

  // Assigning the operation, volume and file values
  operation = argv[1];
  volume_name = argv[2];
  file = argv[3];

  // Opening the volume
  volume_fd = open(volume_name, O_RDWR);
  if (volume_fd < 0){
    DISPLAY_displayError(ERROR_CODE_VOLUME);
  }

  // Checking which filesystem is it and performing the operation
  if (FatSystem_isFatSystem(volume_fd)){
    FatSystem_executeOperation(operation, file,volume_fd);
  }else if (Ex2System_isExt (volume_fd)){
    EX2SYSTEM_executeOperation(operation, file, volume_fd);
  }else{
    printf("It is not FAT16 nor EXT2 filesystems\n");
  }
  return 0;
}


void DISPLAY_displayError(int errorCode){
  printf("%s\n", ERROR_STRINGS[errorCode]);
}


void DISPLAY_displayOperation(char *operation, char *volume_name){
  printf("\n- - - - - -  Operation: %s on the volume: %s  - - - - - -\n\n", operation, volume_name);
}


int isNotValidOperation(char *operation){
  for(int i=0; i < NUM_OPERATIONS; i++){
      if (strcmp(operation, VALID_OPERATIONS[i]) == 0){
          return 0;
      }
  }
  return 1;
}


int isNotValidInput(int argc, char *argv[]){
  // The valid operation at least have three arguments
  if (argc <= 2 || argc > 4){
    DISPLAY_displayError(ERROR_CODE_INPUT);
    return 1;
  }
  // Checking whether the oparam is valid
  if (isNotValidOperation(argv[1])){
    DISPLAY_displayError(ERROR_CODE_OPERATION);
    return 1;
  }

  if((strcmp(argv[1], "/find") == 0 || strcmp(argv[1], "/delete") == 0) && argc == 3){
    printf("Invalid number of arguments\n");
    return 1;
  }
  return 0;
}
