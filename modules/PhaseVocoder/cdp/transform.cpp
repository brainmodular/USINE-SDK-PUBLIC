/* transform.cpp */
/* License Pending. This code is Copyright (c) Trevor Wishart, Richard Dobson and 
 * Composers Desktop Project, March 2001. This code is made freely available for non-commercial use.
 * This copyright notice should be preserved in any redistributed and/or modified version.
 */


#include "transform.h"
#include <math.h>

spectralexaggerator::spectralexaggerator()
{
	frame = 0;
	inptr = outptr = 0;
	inbuf  = outbuf = 0;
	inpv = outpv = 0;


}

spectralexaggerator::~spectralexaggerator()
{
	cleanup();

}
bool spectralexaggerator::init(int samplerate,int fftlen,int overlap,pvocmode mode)
{
	int i;
	
	if(inpv)
		return false;
	if(fftlen <= 0 || samplerate <= 0)
		return false;
	if(overlap > fftlen/2 || overlap <=0)
		return false; 
	decfac = overlap;
	srate = samplerate;
	inptr = outptr = 0;
	nbins = (fftlen + 2) / 2;
	try {
		frame = new float[fftlen + 2];
		inbuf = new float[decfac];
		outbuf = new float[decfac];

		inpv= new phasevocoder();
		outpv= new phasevocoder();
	}

	catch(...){
		cleanup();

		return false;

	}

	if(!inpv->init(srate,fftlen, decfac,mode)){
		cleanup();
		return false;
	}

	if(!outpv->init(srate,fftlen, decfac,mode)){
		cleanup();
		return false;
	}
	 for(i=0;i < decfac; i++)
		 inbuf[i] = outbuf[i] = 0.0f;
	 for(i=0;i < fftlen + 2;i++)
		 frame[i] = 0.0f;

	return true;
}


void spectralexaggerator::cleanup(void)
{
	if(inpv){
		delete inpv;
		inpv = 0;
	}
	if(outpv) {
		delete outpv;
		outpv = 0;
	}
	if(inbuf) {
		delete [] inbuf;
		inbuf = 0;
	}
	if(outbuf) {
		delete [] outbuf;
		outbuf = 0;
	}
	if(frame) {
		delete [] frame;
		frame = 0;
	}

}


		
float spectralexaggerator::tick(float insamp,float exag)
{
	int got;
	if(!outbuf)
		return 0.0f;
	float outval = outbuf[outptr++];
	if(outptr== decfac){
		got = inpv->generate_frame(inbuf,frame,decfac,PVOC_AMP_FREQ);
		inptr = outptr = 0;
		pv_specexag(nbins,frame,exag,nbins * 2);
		got = outpv->process_frame(frame,outbuf,PVOC_AMP_FREQ);
	}
	inbuf[inptr++] = insamp;

	return outval;

}


/************ SPEC TRANSP  ***********/
spectraltransposer::spectraltransposer()
{
	frame = 0;
	inptrForw = outptrForw = 0;
	inptrInv = outptrInv = 0;
	inbuf  = outbuf = 0;
	inpv = outpv = 0;
	amp = freq = 0;

}
spectraltransposer::~spectraltransposer()
{
	cleanup();

}

bool spectraltransposer::init(int samplerate,int fftlen,int overlap,pvocmode mode)
{
	int i;
	
	if(inpv)
		return false;
	if(fftlen <= 0 || samplerate <= 0)
		return false;
	if(overlap > fftlen/2 || overlap <=0)
		return false;
	decfac = overlap;
	srate = samplerate;
	nyquist = (float)samplerate  * 0.5f;
	inptrInv = outptrInv = 0;
	inptrForw = outptrForw = 0;
	nbins = (fftlen + 2) / 2;
	chwidth = nyquist/(float)(nbins - 1);
	try {
		
		frame = new float[fftlen + 2];
		inbuf = new float[decfac];
		outbuf = new float[decfac];
		
		inpv= new phasevocoder();
		outpv= new phasevocoder();
		amp   =  new float[nbins];
		freq   = new float[nbins];
	}

	catch(...){
		cleanup();

		return false;

	}

	if(!inpv->init(srate,fftlen, decfac,mode)){
		cleanup();
		return false;
	}

	if(!outpv->init(srate,fftlen, decfac,mode)){
		cleanup();
		return false;
	}
	 for(i=0;i < decfac; i++)
		 inbuf[i] = outbuf[i] = 0.0f;
	 for(i=0;i < fftlen + 2;i++)
		 frame[i] = 0.0f;
	 
	return true;
}

void spectraltransposer::cleanup(void)
{
	if(inpv){
		delete inpv;
		inpv = 0;
	}
	if(outpv) {
		delete outpv;
		outpv = 0;
	}
	if(inbuf) {
		delete [] inbuf;
		inbuf = 0;
	}
	if(outbuf) {
		delete [] outbuf;
		outbuf = 0;
	}
	if(frame) {
		delete [] frame;
		frame = 0;
	}
	if(amp){
		delete [] amp;
		amp = 0;
	}
	if(freq){
		delete [] freq;
		freq = 0;
	}
}

float spectraltransposer::tick(float insamp,float semitone)
{
	int got;
	if(!outbuf)
		return 0.0f;
	float outval = outbuf[outptrForw++];
	if(outptrForw== decfac){
		got = inpv->generate_frame(inbuf,frame,decfac,PVOC_AMP_FREQ);
   	    inptrInv = outptrInv = 0;
	    inptrForw = outptrForw = 0;
		/* tidy up value if very close to zero! */
		if(fabs(semitone) < 0.0001)
			semitone = 0.0f;		
		get_amp_and_frq(frame,amp,freq,nbins);			
		do_spectral_shiftp(amp,freq,convert_shiftp_vals(semitone),nbins);
		put_amp_and_frq(frame,amp,freq,nbins);
		got = outpv->process_frame(frame,outbuf,PVOC_AMP_FREQ);
	
	}
	inbuf[inptrForw++] = insamp;

	return outval;

}


/********* SPEC ACCU **********/
accutransformer::accutransformer()
{
	frame = framestore = 0;
	inptr = outptr = 0;
	inbuf  = outbuf = 0;
	inpv = outpv = 0;


}

accutransformer::~accutransformer()
{
	cleanup();

}
bool accutransformer::init(int samplerate,int fftlen,int overlap,pvocmode mode)
{
	int i;
	
	if(inpv)
		return false;
	if(fftlen <= 0 || samplerate <= 0)
		return false;
	if(overlap > fftlen/2 || overlap <=0)
		return false;
	decfac = overlap;
	srate = samplerate;
	nyquist  = (float) (srate/2) * 0.9f;
	arate = (float) srate / (float) decfac;
	frametime = (float)(1.0 / arate);
	inptr = outptr = 0;
	nbins = (fftlen + 2) / 2;
	try {
		frame = new float[fftlen + 2];
		framestore = new float[fftlen * 2];
		inbuf = new float[decfac];
		outbuf = new float[decfac];

		inpv= new phasevocoder();
		outpv= new phasevocoder();
	}

	catch(...){
		cleanup();

		return false;

	}

	if(!inpv->init(srate,fftlen, decfac,mode)){
		cleanup();
		return false;
	}

	if(!outpv->init(srate,fftlen, decfac,mode)){
		cleanup();
		return false;
	}
	 for(i=0;i < decfac; i++)
		 inbuf[i] = outbuf[i] = 0.0f;
	 for(i=0;i < fftlen + 2;i++)
		 frame[i] = framestore[i] = 0.0f;

	return true;
}


void accutransformer::cleanup(void)
{
	if(inpv){
		delete inpv;
		inpv = 0;
	}
	if(outpv) {
		delete outpv;
		outpv = 0;
	}
	if(inbuf) {
		delete [] inbuf;
		inbuf = 0;
	}
	if(outbuf) {
		delete [] outbuf;
		outbuf = 0;
	}
	if(frame) {
		delete [] frame;
		frame = 0;
	}
	if(framestore) {
		delete [] framestore;
		frame = 0;
	}
}


		
float accutransformer::tick(float insamp,float decay,float glis)
{
	int got;
	int doglis = 1, dodecay = 1;
	float thisglis = 0, thisdecay = 0;

	if(!outbuf)
		return 0.0f;
	float outval = outbuf[outptr++];
	if(outptr== decfac){
		got = inpv->generate_frame(inbuf,frame,decfac,PVOC_AMP_FREQ);
		inptr = outptr = 0;
		thisglis = (float)(pow(2.0f,glis * frametime));
		if(decay==0.0f){
			//  RWD need some non-zero value 'cos of log 
			decay = 0.000001f;
		}
		else
			thisdecay = (float)( exp(log(decay) * frametime));
		pv_specaccu(doglis,dodecay,thisdecay,thisglis,nbins,framestore,frame,nyquist);
		got = outpv->process_frame(frame,outbuf,PVOC_AMP_FREQ);
	}
	inbuf[inptr++] = insamp;

	return outval;

}


void spectraltransposer::tickForw(float insamp)
{
	int got;
	outptrForw++;
	if(outptrForw== decfac){
		got = inpv->generate_frame(inbuf,frame,decfac,PVOC_AMP_FREQ);
		inptrForw = outptrForw = 0;
		get_amp_and_frq(frame,amp,freq,nbins);			
		
	}
	inbuf[inptrForw++] = insamp;

	
}


float spectraltransposer::tickInv ()
{
	int got;
	float outval = outbuf[outptrInv++];
	if(outptrInv== decfac){
		inptrInv = outptrInv = 0;
		put_amp_and_frq(frame,amp,freq,nbins);
		got = outpv->process_frame(frame,outbuf,PVOC_AMP_FREQ);
	
	}
	
	return outval;

}

