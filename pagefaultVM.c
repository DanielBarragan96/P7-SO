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
    frame=getfreeframe();

//printf("------------------------------%d \n",i);
//////////////////////////////////////////////////////modifications
    if(frame == -1)
    {
        int frames[3], f = 0;
        for(int j = 0; j < 6 ;j++)
            if(ptbr[j].presente == 1 && 3>f)
                frames[f++] = j;

//////////////////////////////checar cual de los access fue el menos reciente
        if(ptbr[frames[0]].tlastaccess < ptbr[frames[1]].tlastaccess && ptbr[frames[0]].tlastaccess < ptbr[frames[2]].tlastaccess){
            pag_a_expulsar = frames[0];//the frame 0 is the one to move to virtual memory
        }
        else if(ptbr[frames[1]].tlastaccess < ptbr[frames[0]].tlastaccess && ptbr[frames[1]].tlastaccess < ptbr[frames[2]].tlastaccess){
            pag_a_expulsar = frames[1];//the frame 1 is the one to move to virtual memory
        }
        else{
            pag_a_expulsar = frames[2];//the frame 2 is the one to move to virtual memory
	}		

		//avisa al sistema que la pagina ahora está libre ("no" ha sido modificada)
            ptbr[pag_a_expulsar].modificado = 0;

		//avisar al sistema que la pagina no esta ocupando memoria fisica
       ptbr[pag_a_expulsar].presente = 0;
		//guardar la dirección de la página a liberar
       frame = ptbr[pag_a_expulsar].framenumber;
		//guardar la página a liberar en memoria virtual (12 = tamaño de la tabla, al sumar 12 se cambia a la tabla de mem.virtual)
       ptbr[pag_a_expulsar].framenumber += 12;
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

