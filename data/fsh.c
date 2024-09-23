#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include "proc.h"
#include "op_int.h"

#define MAX_COMANDOS 5
#define MAX_SIZE 100

static Proc *control;

void sigchld_handler(int signum){
  int status;
  pid_t pid;

  while((pid = waitpid(-1, &status, WNOHANG | WUNTRACED))>0){//Se mantém no while enquanto tiver processos filhos que encerraram ou foram suspensos
    if(WIFSIGNALED(status)){//Verifica se o processo foi encerrado
      int signal = WTERMSIG(status);//Descobre qual sinal encerrou o processo
      send_signal_to_group(control, pid, signal);
    }
    else if(WIFSTOPPED(status)){//Verifica se o processo foi suspenso
      int signal = WSTOPSIG(status);//Descobre qual sinal suspendeu o processo
      send_signal_to_group(control, pid, signal);
    }
  }
}

void sigint_handler(int signum){
  
  if((waitpid(-1, NULL, WNOHANG))==-1 && errno == ECHILD){//Verifica se há filhos vivos
    exit(0);
  }
  else{//Se tiverem filhos vivos
    char response[MAX_SIZE];

    printf("\nTem certeza que deseja sair? (s/n): ");
    fflush(stdout); //Garante que a mensagem seja exibida

    //Lê a resposta do usuário
    fgets(response, sizeof(response), stdin);

    //Se o usuário digitar 's' (case-insensitive), encerra a shell
    if (response[0] == 's' || response[0] == 'S') {
      printf("Saindo...\n");
      exit(0);
    }
    else{
      return;//Retorna a shell
    }
  }
}

void sigtstp_handler(int signum){
  pid_t *pids = get_pids(control);
  pid_t **pgid = get_pgids(control);
  int *size_pgid = get_size_pgid(control);
  int num_terminais = get_num_terminal(control);

  //Suspende os processos de background
  for(int i=0; i<num_terminais; i++){
    for(int j=0; j<size_pgid[i]; j++){
      kill(-(pgid[i][j]), SIGSTOP);
    }
  }
  //Suspende os processos de foreground
  for(int i=0; i<num_terminais; i++){
    kill(pids[i], SIGSTOP);
  }

  return;
}

int main(){
  int num_terminal = 0;
  struct sigaction sa_sigint, sa_sigtstp, sa_sigchld;
  sigset_t mask_int, mask_tstp, mask_chld;
  sigfillset(&mask_int);//Preenche a máscara de sinais, objetivando ignorar todos sinais durante o tratamento desse sinal
  sigemptyset(&mask_chld);
  sigemptyset(&mask_tstp);
  //Definindo os handlers
  sa_sigint.sa_handler = sigint_handler;
  sa_sigtstp.sa_handler = sigtstp_handler;
  sa_sigchld.sa_handler = sigchld_handler;
  //Definindo as mascáras de sinais
  sa_sigint.sa_mask = mask_int;
  sa_sigtstp.sa_mask = mask_tstp;
  sa_sigchld.sa_mask = mask_chld;
  //Definindo as flags
  sa_sigint.sa_flags = 0;
  sa_sigtstp.sa_flags = 0;
  sa_sigchld.sa_flags = SA_RESTART;
  //Define como devem ser tratados os sinais especificados na fsh
  sigaction(SIGINT, &sa_sigint, NULL);
  sigaction(SIGTSTP, &sa_sigtstp, NULL); 
  sigaction(SIGCHLD, &sa_sigchld, NULL);
  

  Proc *p = proc_initialize();
  control = p;

  while(1){
    char **input = malloc(sizeof(char*)*MAX_SIZE);
    int termo = 0;

    for(int i = 0; i < MAX_SIZE; i++){
      input[i] = malloc(sizeof(char)*MAX_SIZE);
    }

    printf("fsh> ");

    while(scanf("%s", input[termo])!= EOF){
      char c;
      termo++;
      scanf("%c", &c);
      if(c == '\n'){
        break;
      }
    }

    char ***comando = malloc(sizeof(char**)*MAX_COMANDOS);
    int pos = 0;
    int num_string = 0;

    for(int i = 0; i < termo; i++){
      if(pos==5){
        pos++;
        break;
      }
      comando[pos] = malloc(sizeof(char*)*MAX_SIZE);
      num_string = 0;
      while(1){  
        comando[pos][num_string]= input[i];
        i++;
        if(i==termo || strcmp(input[i], "#") == 0){
          break;
        }
        
        num_string++;
      }
      pos++;
    }
    if(pos<=5){//Se o número de comandos for menor ou igual a 5
      set_input(p, comando);
      for(int i=pos-1; i>=0; i--){
      if(strcmp(comando[i][0], "waitall") == 0){//Verifica se o input é "waitall"
        waitall();
      }
      else if(strcmp(comando[i][0], "die") == 0){//Verifica se o input é "die"
        die(p);
        for(int i=0; i<pos; i++){
          free(comando[i]);
        }
        for(int i=0; i<MAX_SIZE; i++){
          free(input[i]);
        }
        free(input);
        free(comando);
        free_proc(p);
        exit(EXIT_SUCCESS);
      }
      else{//Caso o comando não seja alguma operação interna o executa
        exec_proc(p, i, num_terminal);
      }
    }
  }
  else{//Se ultrapassou o limite de 5 comandos
    printf("\nVocê excedeu o limite de comandos!!! Tente novamente com até 5 comandos\n");
  } 
  if(pos==6){
    pos = 5;//Caso ultrapasse o limite de 5 comando pos fica setado como 6
  }
    for(int i=0; i<pos; i++){
      free(comando[i]);
    }
    for(int i=0; i<MAX_SIZE; i++){
      free(input[i]);
    }
    free(input);
    free(comando);
    printf("\n");
    num_terminal++;
  }
  free_proc(p);

  return 0;
}
