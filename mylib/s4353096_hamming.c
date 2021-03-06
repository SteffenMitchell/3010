/**
  ******************************************************************************
  * @file    s4353096_hamming.c
  * @author  Steffen Mitchell
  * @date    20042016
  * @brief   Hamming encoder and decoder
  *			 Bytes received are can use the below functions to be hamming encoded
	*			 or decoded.
  ******************************************************************************
	* EXTERNAL FUNCTIONS
  ******************************************************************************
  * extern uint16_t hamming_byte_encoder(uint8_t input) - Hamming encodes input
	* byte
  * extern uint8_t hamming_byte_decoder(uint8_t lower, uint8_t upper) - Hamming
	*	decodes input bytes
	* extern uint16_t crc_update(uint16_t crc, uint8_t c) - Place the character/byte
	*													you would like to update the crc value with
	* extern uint16_t crc_calculation(unsigned char *rxpacket) - Calculates the CRC
	*																						of 30 bytes in the recieved packet
	*
  ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4353096_hamming.h"
#include <string.h>
//#define DEBUG 1
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/*Place the character/byte you would like to update the crc value with*/
extern uint16_t crc_update(uint16_t crc, uint8_t c) {
	int j;
	crc ^= (c << 8);

	/*Calculates CRC*/
	for (j = 0; j < 8; j++) {
		if (crc & 0x8000) {
			crc = (crc << 1) ^ POLY;
		} else {
			crc = (crc << 1);
		}
	}
	return(crc);
}

/*Calculates the CRC of 30 bytes in the recieved packet*/
extern uint16_t crc_calculation(unsigned char *rxpacket) {
	int length;
	uint16_t crc_output = 0x0000;
	length = 30;

	/*Increment through each byte in the recieved packet and update the CRC value*/
	for(int k = 0; k < length; k++){
		crc_output = crc_update(crc_output, rxpacket[k]);
	}

	/*Change to LSB ORDER*/
	crc_output = (crc_output >> 8) ^ (crc_output << 8);
	return crc_output;
}

/*Hamming encodes the input byte*/
extern uint16_t hamming_byte_encoder(uint8_t input) {

	uint16_t out;
	/* first encode D0..D3 (first 4 bits),
	 * then D4..D7 (second 4 bits).
	 */
	out = hamming_hbyte_encoder(input & 0xF) |
		(hamming_hbyte_encoder(input >> 4) << 8);

	return(out);

}

/*Hamming encodes the input byte*/
uint8_t hamming_hbyte_encoder(uint8_t in) {

	uint8_t d0, d1, d2, d3;
	uint8_t p0 = 0, h0, h1, h2;
	uint8_t z;
	uint8_t out;

	/* extract bits */
	d0 = !!(in & 0x1);
	d1 = !!(in & 0x2);
	d2 = !!(in & 0x4);
	d3 = !!(in & 0x8);

	/* calculate hamming parity bits */
	h0 = d0 ^ d1 ^ d2;
	h1 = d0 ^ d1 ^ d3;
	h2 = d0 ^ d2 ^ d3;

	/* generate out byte without parity bit P0 */
	out = (h0 << 1) | (h1 << 2) | (h2 << 3) |
		(d0 << 4) | (d1 << 5) | (d2 << 6) | (d3 << 7);

	/* calculate even parity bit */
	for (z = 1; z<8; z++)
		p0 = p0 ^ !!(out & (1 << z));

	out |= p0;
	/*Insert error here, i.e possibly wait for terminal input and insert given hex value, Do this for both*/
	/*Enter a certain value maybe to skip error input*/
	return(out);
}

/*Enter lower and upper in MSB form*/
extern uint8_t hamming_byte_decoder(uint8_t lower, uint8_t upper) {
	uint8_t d0, d1, d2, d3;
	uint8_t p0 = 0, pr, h0, h1, h2;
	uint8_t ed0, ed1, ed2, ed3, eh0, eh1, eh2;
	uint8_t s0, s1, s2, S;
	uint8_t c_low = 0; //corrected bytes
	uint16_t c_up = 0, C, R, E;
	uint8_t decode_byte;
	uint8_t z;

	/*Start by decoding upper*/
	for (int i =0; i < 2; i++) {
		p0 = 0;

		/*Select which byte to decode*/
		if (i == 0) {
			decode_byte = upper;
		} else if (i == 1) {
			decode_byte = lower;
		} else {

		}

		/*Extract bits from MSB byte*/
		pr = !!(decode_byte & 0x1);
		h0 = !!(decode_byte & 0x2);
		h1 = !!(decode_byte & 0x4);
		h2 = !!(decode_byte & 0x8);
		d0 = !!(decode_byte & 0x10);
		d1 = !!(decode_byte & 0x20);
		d2 = !!(decode_byte & 0x40);
		d3 = !!(decode_byte & 0x80);
		/*Construct Syndrome*/
		s0 = d0 ^ d1 ^ d2 ^ h0;
		s1 = d0 ^ d1 ^ d3 ^ h1;
		s2 = d0 ^ d2 ^ d3 ^ h2;
		S = (s0 << 0) | (s1 << 1) | (s2 << 2);
		/*Set Various Error Values*/
		ed1 = (1 << 0) | (1 << 1) | (0 << 2);
		ed2 = (1 << 0) | (0 << 1) | (1 << 2);
		ed3 = (0 << 0) | (1 << 1) | (1 << 2);
		ed0 = (1 << 0) | (1 << 1) | (1 << 2);
		eh2 = (0 << 0) | (0 << 1) | (1 << 2);
		eh1 = (0 << 0) | (1 << 1) | (0 << 2);
		eh0 = (1 << 0) | (0 << 1) | (0 << 2);
/*Try opposite direction*/

		/*Calculate the parity bit based on the decode_byte*/
		for (z = 1; z<8; z++)
			p0 = p0 ^ !!(decode_byte & (1 << z));

		/*If there is no parity error*/
		if (pr == p0) {
			if (S == 0x00) {
				/*No bit error */
			} else {
				/*2 bit error*/
				debug_printf("2 Bit error\n");
				
			}
		} else if (pr != p0) {
			/*If there is a parity error*/

				if (S == 0x00) {
				/*1 Bit error in parity bit*/
				p0 = (~p0 & 0x1);
				debug_printf("Error in Parity\n");
			} else if (S == ed3) {
				d3 = (~d3 & 0x1);
				/*Error in D3*/
				debug_printf("Error in D3\n");
			} else if (S == ed2) {
				d2 = (~d2 & 0x1);
				/*Error in D2*/
				debug_printf("Error in D2\n");
			} else if (S == ed1) {
				/*Error in D1*/
				d1 = (~d1 & 0x1);
				debug_printf("Error in D1\n");
			} else if (S == ed0) {
				/*Error in D0*/
				d0 = (~d0 & 0x1);
				debug_printf("Error in D0\n");
			} else if (S == eh2) {
				/*Error in H2*/
				h2 = (~h2 & 0x1);
				debug_printf("Error in H2\n");
			} else if (S == eh1) {
				/*Error in H1*/
				h1 = (~h1 & 0x1);
				debug_printf("Error in H1\n");
			} else if (S == eh0) {
				/*Error in H0*/
				h0 = (~h0 & 0x1);
				debug_printf("Error in H0\n");
			}
		}
		/*Construct Decoded Byte*/
		decode_byte = (p0 << 0) | (h0 << 1) | (h1 << 2) | (h2 << 3) |
			(d0 << 4) | (d1 << 5) | (d2 << 6) | (d3 << 7);

		if (i == 0) {
			/*Compile corrected upper*/
			c_up = decode_byte;
		} else if (i == 1) {
			/*Compile corrected lower*/
			c_low = decode_byte;
		}
	}

	#ifdef DEBUG
	C = (c_low & 0xFF) | (c_up << 8);
	R = (lower & 0xFF) | (upper << 8);
	E = R ^ C;
	debug_printf("RECIEVED FROM LASER: %c - RAW: %04x (ErrMask %04x)\n", decode_byte, R, E);
	#endif

	decode_byte = (c_up & 0xF0) ^ ((c_low & 0xF0 ) >> 4);
	return decode_byte;
}
