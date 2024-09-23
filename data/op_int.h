#ifndef OP_INT_H
#define OP_INT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include "proc.h"

/**
 * @brief Responsável por liberar os descendentes da shell que estejam no estado "Zombie"
 * 
 */
void waitall();
/**
 * @brief Mata-se todos os processos ainda vivos descendentes da shell
 * 
 * @param p Estrutura que contém os processos filhos da shell
 */
void die(Proc *p);

#endif