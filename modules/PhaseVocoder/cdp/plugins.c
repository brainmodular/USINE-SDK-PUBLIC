/* plugins.c. */
/* License Pending. This code is Copyright (c) Trevor Wishart, Richard Dobson and 
 * Composers Desktop Project, March 2001. This code is made freely available for non-commercial use.
 * This copyright notice should be preserved in any redistributed and/or modified version.
 */

#include "./plugins.h"
#ifdef _DEBUG
#include <assert.h>
#endif
#include <math.h>
#include <stdlib.h>

/*
#if defined _WIN32 && defined _MSC_VER
__inline static long pvround(double fval)
{
	int result;
	_asm{
		fld		fval
		fistp	result
		mov		eax,result
	}
	return result;
}

#else
*/
static long pvround(double val)
{
	long k;
	k = (long)(fabs(val)+0.5);
	if(val < 0.0)
		k = -k;
	return k;
}
/*
#endif
*/

/******* EXAGGERATE **********/


/*NB clength will always be wanted/2 */
int pv_specexag(int clength,float *sbufptr,double exag_exag,int wanted)				 
{
	int cc, vc;	
	double post_totalamp = 0.0, pre_totalamp = 0.0;
	double maxamp = 0.0, normaliser;

	for( cc = 0 ,vc = 0; cc < clength; cc++, vc += 2)  {
		pre_totalamp += sbufptr[vc];
		if(sbufptr[vc] > maxamp)
			maxamp = sbufptr[vc];
		
	}
	if(maxamp<=0.0)
		return 1;		/*pseudo error: announce a zero*/
	normaliser = 1.0/maxamp;
	for(cc = 0 ,vc = 0; cc < clength; cc++, vc += 2)  {
		sbufptr[vc]  = (float)(sbufptr[vc] * normaliser);
		sbufptr[vc]  = (float)(pow(sbufptr[vc],exag_exag));
		post_totalamp += sbufptr[vc];
	}
	/*RWD TODO: do pre/post calc here, so normalize() is a simple vector op */
	return pv_normalise(sbufptr,pre_totalamp,post_totalamp,wanted);
}


__inline
int pv_normalise(float *sbufptr,double pre_totalamp,double post_totalamp,int wanted)
{
	double normaliser;
	int vc,zeroset = 0;
	if(post_totalamp < VERY_TINY_VAL)	
		zeroset = 1;		
 	else {
		/*normaliser = pre_totalamp/post_totalamp;*/
		/*RWD, needs a bit more trimming ! */
		normaliser = (pre_totalamp/post_totalamp) * 0.5;	

		for(vc = 0; vc < wanted; vc += 2)
			sbufptr[vc] = (float)(sbufptr[vc] * normaliser);
	}
	return zeroset;
}

/*************** SPEC ACCU ***********/

void pv_specaccu(char glisflag,
				 char decayflag,
				 double dindex, double gindex,
				 int clength,
				 float *sampbuf_0, 		
				 float *sbufptr_0,float nyquist)
{
	
	int vc, cc;
	if(glisflag) {
		if(decayflag)
		{		 /*ie both set*/
			for(cc = 0, vc = 0; cc< clength; cc++,  vc += 2)
			{
				sampbuf_0[vc] = (float)(sampbuf_0[vc] * dindex);
				sampbuf_0[vc+1] = (float)(sampbuf_0[vc+1] * gindex);
				/*RWD avoid nyquist overruns?*/
				if(sampbuf_0[vc+1] >= nyquist)
					sampbuf_0[vc] = 0.0f;
				pv_accumulate(vc,sbufptr_0,sampbuf_0);				
			}
		}
		else {					   /*just GLIS*/
			for(cc = 0, vc = 0; cc < clength; cc++,  vc += 2) {
				sampbuf_0[vc+1] = (float)(sampbuf_0[vc+1] * gindex);
				/*RWD avoid nyquist overruns?*/
				if(sampbuf_0[vc+1] >= nyquist)
					sampbuf_0[vc] = 0.0f;
				pv_accumulate(vc,sbufptr_0,sampbuf_0);				
			}
		}
	} else if(decayflag) {	  /*ie only this set*/
		for(cc = 0, vc = 0; cc < clength; cc++,  vc += 2) {
			sampbuf_0[vc] = (float)(sampbuf_0[vc] * dindex);			
			pv_accumulate(vc,sbufptr_0,sampbuf_0);			
		}
	} else {	 						 /*neither*/
		for(cc = 0, vc = 0; cc < clength; cc++,  vc += 2) {			
			pv_accumulate(vc,sbufptr_0,sampbuf_0);			
		}
	}
	
}

__inline
void pv_accumulate(int index,float *sbufptr, float *sampbuf)
{
	
	int frq = index+1;
	if(sbufptr[index] > sampbuf[index])  {	 /* if current amp > amp in accumulator */
		sampbuf[index] = sbufptr[index]; 
		sampbuf[frq] = sbufptr[frq];	 /* replace amp in accumulator with current amp */
	} else {
		sbufptr[index] = sampbuf[index]; 	 /* else replace current amp with amp in accumulator */
		sbufptr[frq] = sampbuf[frq];								
	}	
}


/* split/merge amp and freq in frames */

void get_amp_and_frq(const float *floatbuf,float *amp,float *freq,long clength)
{
	long cc, vc;
#ifdef _DEBUG
	assert(floatbuf != NULL);
	assert(amp != NULL);
	assert(freq	!= NULL);
#endif


	for( cc = 0 ,vc = 0; cc < clength; cc++, vc += 2){
		amp[cc]  = floatbuf[vc];
		freq[cc] = floatbuf[vc+1];
	}
	
}

void put_amp_and_frq(float *floatbuf,const float *amp, const float *freq,long clength)
{
	long cc, vc;

	for(cc = 0, vc = 0; cc < clength; cc++, vc += 2){
		floatbuf[vc] = amp[cc];
		floatbuf[vc+1] = freq[cc];
	}

}

/*RWD: NB: calcs the ~theoretical~ chan-no...doesn't always correspond to reality! */
int get_channel_corresponding_to_frq(double thisfrq,double chwidth,double nyquist)
{	
	int chan;
	double halfchwidth = chwidth * 0.5;

 	chan = (int)((fabs(thisfrq) + halfchwidth)/chwidth);  /* TRUNCATE */

	return chan;
}

/******** SPEC TRANSP *************/

float convert_shiftp_vals(float semitone)
{
	float val;	
	val = (float)(semitone * OCTAVES_PER_SEMITONE);
	val = (float) pow(2.0,val);
	return val;
}


void do_spectral_shiftp(float *amp, float *freq,float pitch,long clength)
{
	double shft = (double) pitch;
	long   j, k;
	
	if( shft > 1.0f) {
		j = clength-1;
		k  = pvround((double)j/shft);
		while( k >= 0) {
			/*RWD*/
			if(j < 0)
				break;
			amp[j]  = amp[k];
			freq[j] = (float)(shft * freq[k]);
			j-- ;
			k  = pvround((double)j/shft);
		}
		for( k=j; k>= 0;k-- ) {  /*RWD was k > */
			amp[k]  = 0.0f;
			freq[k] = 0.0f;
		}				
	} else if(shft < 1.0){		/*RWD : shft = 1 = no change */
		j = 0;
		k  = pvround((double)j/shft);
		while( k <= (clength-1)) {
			amp[j]  = amp[k];
			freq[j] = (float)(shft * freq[k]);
			j++ ;
			k  = pvround((double)j/shft);
		}
		for( k=j; k < clength; k++ ) {
			amp[k]  = 0.0f;
			freq[k] = 0.0f;
		}				
	}
}



