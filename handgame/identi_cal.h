/*
 * identi_cal.h
 *
 *  Created on: 2016Äê7ÔÂ11ÈÕ
 *      Author: Thinkpad-T440p
 */

#ifndef SRC_IDENTI_CAL_H_
#define SRC_IDENTI_CAL_H_

#define cal_I (8*24*24)//input 4608
#define cal_O 3//output


#pragma SDS data mem_attribute(input:PHYSICAL_CONTIGUOUS, output:PHYSICAL_CONTIGUOUS)
//#pragma SDS data access_pattern(input:SEQUENTIAL, out:SEQUENTIAL)
//#pragma SDS data zero_copy(input)	//worked but to small BRAM
#pragma SDS data buffer_depth(input:18432,output:3)
void identi_cal(float* input,float* output);


#endif /* SRC_IDENTI_CAL_H_ */
