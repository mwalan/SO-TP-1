#include "proc.h"
#define MAX_PROC 1024
#define MAX_COMANDOS 5

struct proc{
  pid_t *pid_foreground;//Armazena os pids dos processos criados em foreground
  pid_t **pgid;//Armazena os pgids dos grupos de processos criados em background
  int *size_pgid;//Armazena quantos grupos de background foram criados em cada terminal
  char ***command;//Input a ser executado (o que foi digitado no terminal)
  int num_terminais;//Quantidade de terminais executados
};

Proc *proc_initialize(){
  Proc *proc = malloc(sizeof(Proc));
  proc->pid_foreground = malloc(sizeof(pid_t)*MAX_PROC);
  proc->pgid = malloc(sizeof(pid_t*)*MAX_PROC);
  proc->size_pgid = malloc(sizeof(int)*MAX_PROC);
  for(int i=0; i<MAX_PROC; i++){
    proc->pgid[i] = malloc(sizeof(pid_t)*MAX_COMANDOS);
  }
  for(int i=0; i<MAX_PROC; i++){
    proc->size_pgid[i] = 0;
  }
  proc->num_terminais = 0;
  
  return proc;
}

void set_input(Proc *p, char ***input){
  p->command = input;
}

int *get_size_pgid(Proc *p){
  return p->size_pgid;
}

pid_t *get_pids(Proc *p){
  return p->pid_foreground;
}

pid_t **get_pgids(Proc *p){
  return p->pgid;
}

int get_num_terminal(Proc *p){
  return p->num_terminais;
}

void exec_proc(Proc *p, int num, int num_terminal){
  pid_t PID = fork(), PID_2, pgid;
  if(PID==-1){
    perror("Erro no fork do filho primário");//Verifica erro no fork do filho primário
    exit(EXIT_FAILURE);
  }
  struct sigaction sa_sigint;
  sigset_t mask_int;
  sigemptyset(&mask_int);
  sa_sigint.sa_handler = SIG_IGN;
  sa_sigint.sa_mask = mask_int;
  sa_sigint.sa_flags = 0;

  if(PID==0){
    if(num>=1){//Se o processo for de background
      setsid();//Muda-se sua sessão
      pgid = getpgrp();//Obtém-se a qual grupo pertence
      PID_2 = fork();//Realiza-se a criação do processo secundário
      if(PID_2==-1){
        perror("Erro no fork do filho secundário");//Verifica erro no fork do filho secundário
        exit(EXIT_FAILURE);
      }
      if(PID_2 == 0){//Se for o processo secundário
        setpgid(getpid(), pgid);//Adiciona-se ele no grupo do processo filho primário
      }
    }
    //signal(SIGINT, SIG_IGN);//Descendentes da shell devem ignorar o SIGINT
    sigaction(SIGINT, &sa_sigint, NULL);
    execvp(p->command[num][0], p->command[num]);//Executa-se o comando
    perror("Erro ao executar o comando");//Verifica se houve erro no exec
    exit(EXIT_FAILURE);
  }
  else{
    if(num==0){
      p->pid_foreground[p->num_terminais++] = PID;//Armazena o PID do processo de foreground
      waitpid(PID, NULL, WUNTRACED);
    }
    if(num >= 1){
      p->pgid[num_terminal][p->size_pgid[num_terminal]++] = PID;//Armazena-se o pgid dos processos de background(PID do processo filho primário)
    }
  }
}

void send_signal_to_group(Proc *p, pid_t pid, int signal){
  int num_terminal_proc=-1;

  //Usa-se o for para ver a qual linha de comando o processo de pid "pid" pertence.
  for(int i=0; i<p->num_terminais; i++){
    if(p->pid_foreground[i]==pid){
      num_terminal_proc = i;
      break;
    }
    for(int j=0; j<p->size_pgid[i]; j++){
      if(p->pgid[i][j]==pid){
        num_terminal_proc = i;
        break;
      }
    }
    if(num_terminal_proc>=0){
      break;
    }
  }

  if(num_terminal_proc>=0){//Verifica se a linha de comando do processo foi encontrado
    if(pid!=p->pid_foreground[num_terminal_proc]){
      kill(p->pid_foreground[num_terminal_proc], signal);//Envia o sinal para o processo de foreground daquela linha de comando
    }
    for(int j=0; j<p->size_pgid[num_terminal_proc]; j++){
      if(p->pgid[num_terminal_proc][j] != pid){
        kill(-(p->pgid[num_terminal_proc][j]), signal);//Envia o sinal para os grupos de background daquela linha de comando
      }
    }
  }
}

void free_proc(Proc *p){
  for(int i=0; i<MAX_PROC; i++){
    free(p->pgid[i]);
  }
  free(p->pgid);
  free(p->size_pgid);
  free(p->pid_foreground);
  free(p);
}