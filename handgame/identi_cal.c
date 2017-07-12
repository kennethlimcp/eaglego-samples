/*
 * identi_cal.c
 *
 *  Created on: 2016Äê7ÔÂ11ÈÕ
 *      Author: Thinkpad-T440p
 */

#include "ip1_w.h"
#include "identi_cal.h"
float ip1_b[cal_O]={0.06077459454536,0.07276204973459,-0.13353802263737};
/*
void ip1(float in[cal_I],float rst[cal_O])
{
    int i,o;
    for(o=0;o<cal_O;o++)
    {
        for(i=0;i<cal_I;i++)
        {
            rst[o]+=in[i]*ip1_w[o][i];
        }
        rst[o]+=ip1_b[o];
    }
}
*/
//
//void identi_cal(float* input, float* output) {
//#pragma AP INTERFACE ap_fifo port=input depth=cal_I
//#pragma AP INTERFACE ap_fifo port=output depth=cal_O
//	int i, j, k, t;
//	int index = 0;
//	float temp = 0;
//	float temp_sum[cal_O] = { 0 };
//	//float* in = (float*)input;
//	for (t = 0; t < cal_O; t++) {
//		for (i = 0; i < 8; i++)
//#pragma AP PIPELINE II = 1
//			for (j = 0; j < 24; j++)
//#pragma AP unroll
//				for (k = 0; k < 24; k++) {
//					index = i * 24 * 24 + j * 24 + k;
//					temp = input[index] * ip1_w[t][index];	// input SEQUENTIAL
//					temp_sum[t] += temp;
//				}
//		temp_sum[t] += ip1_b[t];
//		temp = 0;
//	}
//	output = (float*) (temp_sum);
//
//}
//
void identi_cal(float* input, float* output)
{
    int i,j;
    for(i=0;i<cal_O;i++)
    {
        for(j=0;j<cal_I;j++)
        {
        	output[i]+=input[j]*ip1_w[i][j];
        }
        output[i]+=ip1_b[i];
    }
}

