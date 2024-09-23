#ifndef PROC_H
#define PROC_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

typedef struct proc Proc;
/**
 * @brief Inicializa uma estrutura que será responsável por armazenar dados de processos
 * 
 * @return Proc* Estrutura alocada
 */
Proc *proc_initialize();
/**
 * @brief Responsável por atualizar o input da última linha de comando passada
 * 
 * @param p Estrutura que se deseja atualizar o input
 * @param input Input que se deseja processar futuramente
 */
void set_input(Proc *p, char ***input);
/**
 * @brief Retorna os pids dos processos de foreground
 * 
 * @param p Estrutura que se deseja extrair os pids de foreground
 * @return pid_t* Vetor que contém os pids dos processos de foreground
 */
pid_t *get_pids(Proc *p);
/**
 * @brief Retorna um vetor em que cada posição indica a quantidade de processos executados no i-ésimo terminal
 * 
 * @param p Estrutura que se deseja extrair
 * @return int* Vetor que contém a quantidade de processos executados em cada terminal
 */
int *get_size_pgid(Proc *p);
/**
 * @brief Retorna quantos terminais executaram na estrutura "p"
 * 
 * @param p Estrutura que se deseja saber quantos terminais executaram
 * @return int Número de terminais executados por "p"
 */
int get_num_terminal(Proc *p);
/**
 * @brief Retorna uma matriz onde cada linha corresponde a um terminal e cada coluna o pgid de cada um dos grupos criados nesse terminal
 * 
 * @param p Estrutura que se deseja extrair a matriz
 * @return pid_t** Matriz com os pgids
 */
pid_t **get_pgids(Proc *p);
/**
 * @brief Função responsável por executar o comando de número "num" do terminal de número "num_terminal"
 * 
 * @param p Estrutura que se deseja executar a estrutura interna "command" na posição "num"
 * @param num Posição do programa que será executado
 * @param num_terminal Posição do terminal
 */
void exec_proc(Proc *p, int num, int num_terminal);
/**
 * @brief Função responsável por enviar o sinal "signal" para todos os processos do grupo do processo de pid "pid"
 * 
 * @param p Estruta que armazena os dados dos processos
 * @param pid pid que se deseja achar os processos que pertencem ao mesmo terminal
 * @param signal sinal que deve ser passado aos processos do mesmo grupo de "pid"
 */
void send_signal_to_group(Proc *p, pid_t pid, int signal);
/**
 * @brief Libera a estrutura de processos "p"
 * 
 * @param p Estrutura a ser liberada
 */
void free_proc(Proc *p);

#endif