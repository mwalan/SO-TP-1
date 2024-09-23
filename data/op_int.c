#include "op_int.h"

void waitall(){
  while(1){
    if(((waitpid(-1, NULL, WNOHANG)) == -1)){
      if(errno == EINTR){
        continue;
      }
      else{
        break;
      }
    }
  }
}

void die(Proc *p){
  pid_t *pid = get_pids(p);
  pid_t **pgid = get_pgids(p);
  int *size = get_size_pgid(p);
  int num_terminais = get_num_terminal(p);

  //Mata-se os processos de foreground
  for(int i=0; i<num_terminais; i++){
    kill(pid[i], SIGKILL);
  }
  
  //Mata-se os processos de background
  for(int i=0; i<num_terminais; i++){
    for(int j=0; j<size[i]; j++){
      kill(-(pgid[i][j]), SIGKILL);
    }
  }
}