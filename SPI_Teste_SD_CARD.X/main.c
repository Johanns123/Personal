/*----------------------------------------------------------------------*/
/* Foolproof FatFs sample project for AVR              (C)ChaN, 2014    */
/*----------------------------------------------------------------------*/

#include <avr/io.h>	/* Device specific declarations */
#include <stdio.h>
#include <string.h>
#include "ff.h"		/* Declarations of FatFs API */
#include "diskio.h"

FATFS FatFs;		/* FatFs work area needed for each volume */
FIL Fil;			/* File object needed for each open file */




int main (void)
{   
    /*Configurar SPI*/
	UINT bw;
	FRESULT fr;
	
	char buffer [10];//tamanho dado por strlen(buffer);
	
    int value = 1000;
    
    sprintf(buffer, "%4d,", value);

	f_mount(&FatFs, "", 0);		/* Give a work area to the default drive */

	fr = f_open(&Fil, "newfile.csv", FA_WRITE | FA_CREATE_ALWAYS);	/* Create a file */
	if (fr == FR_OK) {
		f_write(&Fil, buffer , 11, &bw);	/* Write data to the file */
		fr = f_close(&Fil);							/* Close the file */
		if (fr == FR_OK && bw == 11) {		/* Lights green LED if data written well */
			DDRB |= 0x10; PORTB |= 0x10;	/* Set PB4 high */
		}
	}

	for (;;) ;
}


