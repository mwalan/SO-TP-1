#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMANDOS 5
#define MAX_SIZE 40

int main(void) {
  char **input = malloc(sizeof(char*)*MAX_SIZE);
  int termo = 0, fim = 0;

  for(int i = 0; i < MAX_SIZE; i++){
    input[i] = malloc(sizeof(char)*40);
  }

  printf("fsh>");

  while(scanf("%s", input[termo])!= EOF){
    char c;
    termo++;
    fim = scanf("%c", &c);
    if(c == '\n'){
      break;
    }
  }

  char **comando = malloc(sizeof(char*)*MAX_COMANDOS);
  int pos = 0;

  for(int i = 0; i < termo; i++){
    comando[pos] = malloc(sizeof(char)*MAX_SIZE);
    comando[pos][0] = '\0';
    while(1){  
      strcat(comando[pos], input[i]);
      i++;
      if(i==termo){
        break;
      }
      if(strcmp(input[i],"#") != 0){
        strcat(comando[pos], " ");
      }
      else{
        break;
      }
    }
    printf("%s\n", comando[pos]);
    pos++;
  }

  for(int i=0; i<pos; i++){
    free(comando[i]);
  }
  for(int i=0; i<MAX_SIZE; i++){
    free(input[i]);
  }
  free(input);
  free(comando);

  return 0;
}