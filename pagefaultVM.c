#include <stdio.h>
#include <stdlib.h>
#include "mmu.h"

#define RESIDENTSETSIZE 24

extern char *base;
extern int framesbegin;
extern int idproc;
extern int systemframetablesize;
extern int ptlr;

extern struct SYSTEMFRAMETABLE *systemframetable;
extern struct PROCESSPAGETABLE *ptbr;
extern struct PROCESSPAGETABLE *gprocesspagetable;

int getfreeframe();

int frames_used = 0;

// Rutina de fallos de página

int pagefault(char *vaddress)
{
    int i;
    int frame;
    unsigned int pag_a_expulsar;
    long pag_del_proceso;

    // Calcula la página del proceso
    pag_del_proceso=(long) vaddress>>12;
    // Cuenta los marcos asignados al proceso
    i=countframesassigned();
   
    // Busca un marco libre en el sistema

//printf("------------------------------%d \n",i);
//////////////////////////////////////////////////////
  //  if(i != 3)
        frame=getfreeframe();
    if(frame == -1)
    {
        int frames[3], f = 0;
        for(int j = 0; j < 6 ;j++){
            if(ptbr[j].presente == 1 && 3>f){
                frames[f] = j;
                f++;
            }
        }

        if(ptbr[frames[0]].tlastaccess < ptbr[frames[1]].tlastaccess && ptbr[frames[0]].tlastaccess < ptbr[frames[2]].tlastaccess){
            pag_a_expulsar = frames[0];
        }

        else if(ptbr[frames[1]].tlastaccess < ptbr[frames[0]].tlastaccess && ptbr[frames[1]].tlastaccess < ptbr[frames[2]].tlastaccess){
            pag_a_expulsar = frames[1];
        }

        else{
            pag_a_expulsar = frames[2];
	}

        if(ptbr[pag_a_expulsar].modificado == 1){
            ptbr[pag_a_expulsar].modificado = 0;
        }

       ptbr[pag_a_expulsar].presente = 0;
       frame = ptbr[pag_a_expulsar].framenumber;
       ptbr[pag_a_expulsar].framenumber = ptbr[pag_a_expulsar].framenumber + 12;
    }

//////////////////////////////////////////////////////
    (ptbr+pag_del_proceso)->presente=1;
    (ptbr+pag_del_proceso)->framenumber=frame;

    return(1); // Regresar todo bien
}


int getfreeframe()
{
    int i;
    // Busca un marco libre en el sistema
    for(i=framesbegin;i<systemframetablesize+framesbegin;i++)
        if(!systemframetable[i].assigned)
        {
            systemframetable[i].assigned=1;
            break;
        }
    if(i<systemframetablesize+framesbegin)
        systemframetable[i].assigned=1;
    else
        i=-1;
    return(i);
}

