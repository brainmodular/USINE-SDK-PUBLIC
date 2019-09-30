/*plugins.h */
/* License Pending. This code is Copyright (c) Trevor Wishart, Richard Dobson and 
 * Composers Desktop Project, March 2001. This code is made freely available for non-commercial use.
 * This copyright notice should be preserved in any redistributed and/or modified version.
 */

#ifndef PLUGINS_H_INCLUDED
#define PLUGINS_H_INCLUDED

#define VERY_TINY_VAL (1e-20)
#define MAXGLISRATE	(0.0625)
int pv_specexag(int clength,float *sbufptr,double exag_exag,int wanted);
int pv_normalise(float *sbufptr,double pre_totalamp,double post_totalamp,int wanted);
void pv_specaccu(char glisflag,
				 char decayflag,
				 double dindex, double gindex,
				 int clength,
				 float *sampbuf_0, 		
				 float *sbufptr_0,float nyquist);

void pv_accumulate(int index,float *sbufptr, float *sampbuf);

void get_amp_and_frq(const float *floatbuf,float *amp,float *freq,long clength);
void put_amp_and_frq(float *floatbuf,const float *amp, const float *freq,long clength);

#define OCTAVES_PER_SEMITONE (0.08333333333)

float convert_shiftp_vals(float semitone);
void do_spectral_shiftp(float *amp, float *freq,float semi,long clength);

#endif
