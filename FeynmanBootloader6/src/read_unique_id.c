/*
 * read_unique_id.c
 *
 * Created: 9/19/2017 12:09:59 PM
 *  Author: UltiMachine Developers
 *  Description: implementation of the Crystal-less USB Atmel SAMG55 application note
 */

#include <efc.h>
#include "read_unique_id.h"

// No Previous Prototypes!!!
void read_Unique_ID( Efc *pEfc , DATA_UNIQUE_ID *UniqueID);

//__ramfunc 
__no_inline
RAMFUNC
void read_Unique_ID( Efc *pEfc , DATA_UNIQUE_ID *UniqueID)
{
  unsigned int *ptFlash = (unsigned int *) 0x00400000;
  unsigned int *ptRAM = (unsigned int *) UniqueID;

  pEfc->EEFC_FCR = EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_STUI ;
  //* Wait the end of command while ((pEfc->EEFC_FSR & EEFC_FSR_FRDY) == EEFC_FSR_FRDY )
  for (unsigned int ii = 0; ii < (sizeof (DATA_UNIQUE_ID))/4 ;ii++) {
  ptRAM[ii]=ptFlash[ii];

  }
  
  pEfc->EEFC_FCR = EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_SPUI;
}


unsigned short get_internalRC(void)
{
  DATA_UNIQUE_ID UniqueID;

  read_Unique_ID(EFC , &UniqueID);
  return UniqueID.RC32_18;
}

uint32_t read_uniqueid_rc32_3v3() {
	uint32_t uniqueid_buf[13] = {0};
	efc_perform_read_sequence(EFC, EFC_FCMD_STUI, EFC_FCMD_SPUI, uniqueid_buf, 13); //length in 32bit ints
	return uniqueid_buf[12] & 0x0000FFFF; //Bytes [49..48]: Frequency of 32 kHz RC at 3.3V
}

uint32_t read_uniqueid_rc32_1v8() {
	uint32_t uniqueid_buf[13] = {0};
	efc_perform_read_sequence(EFC, EFC_FCMD_STUI, EFC_FCMD_SPUI, uniqueid_buf, 13); //length in 32bit ints
	return uniqueid_buf[12] >> 16;
}
