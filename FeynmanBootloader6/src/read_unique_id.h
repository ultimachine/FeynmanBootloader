/*
 * read_unique_id.h
 *
 * Created: 9/19/2017 12:10:16 PM
 *  Author: UltiMachine Developers
 *  Description: implementation of the Crystal-less USB SAMG55 application note
 */ 

#ifndef ___read_unique_id__h___
#define ___read_unique_id__h___

typedef struct {              //DATASHEET See 8.3.1.5 Unique Identifier
	unsigned int Unique_ID[4];  //DATASHEET Bytes [15..0]: 128 bits for unique identifier
	unsigned int unused0[8];    //DATASHEET Bytes [47..16]: Atmel reserved
	unsigned short RC32_18;     //DATASHEET Bytes [49..48]: Measured frequency (on tester) of the internal 32 kHz RC when V DDIO = 3.3V (measurement performed at 25°C). These two bytes contain the frequency in hertz.
	unsigned short RC32_33;     //DATASHEET Bytes [51..50]: Measured frequency (on tester) of the internal 32 kHz RC when V DDIO = 1.8V (measurement performed at 25°C). These two bytes contain the frequency in hertz.
	unsigned int unused1[3];    //DATASHEET Bytes [63..52]: Atmel reserved
	unsigned char code_1265_18; //DATASHEET Bytes [65..64]: Trimmed code of the internal regulator which allows the device to run at up to 120 MHz. The four LSB bits must be written in the SUPC_PWMR.ECPWRx.
	unsigned char code_0094_18; //...
	unsigned short code_18[7];
	unsigned char code_1265_33;
	unsigned char code_0094_33;
	unsigned short code_33[7];
} DATA_UNIQUE_ID;

extern unsigned short get_internalRC(void);

extern uint32_t read_uniqueid_rc32_3v3(void);
extern uint32_t read_uniqueid_rc32_1v8(void);

#endif // ___read_unique_id__h___