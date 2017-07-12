/*
 * white_balance.h
 *
 *  Created on: 2016Äê5ÔÂ25ÈÕ
 *      Author: Thinkpad-T440p
 */


#ifndef SRC_WHITE_BALANCE_H_
#define SRC_WHITE_BALANCE_H_

#include <inttypes.h>

#define FRAME_WIDTH  1920
#define FRAME_HEIGHT 1080

#define B_MASK   0x00FF0000
#define G_MASK   0x0000FF00
#define R_MASK   0x000000FF

// pull out the color component and return as a value between 0 and 255
#define getBlueCMOS(colorPixel)  (colorPixel & B_MASK) >> 16
#define getGreenCMOS(colorPixel) (colorPixel & G_MASK) >> 8
#define getRedCMOS(colorPixel)   (colorPixel & R_MASK)


//sys port
//#pragma SDS data sys_port(input: AFI, output: AFI)
//data mover
#pragma SDS data mem_attribute(input: PHYSICAL_CONTIGUOUS)
#pragma SDS data mem_attribute(output: PHYSICAL_CONTIGUOUS)
#pragma SDS data copy(input[0:2073600])
#pragma SDS data copy(output[0:2073600])
#pragma SDS data zero_copy(kval[0:4])
//accelerator
#pragma SDS data access_pattern(input: SEQUENTIAL)
#pragma SDS data access_pattern(output: SEQUENTIAL)

int white_balance(unsigned int* input, unsigned int kval[4],unsigned int* output);

#endif /* SRC_WHITE_BALANCE_H_ */
