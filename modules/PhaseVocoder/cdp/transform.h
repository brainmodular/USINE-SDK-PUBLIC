/* transform.h : GUI-independent spectral transformation classes */
/* License Pending. This code is Copyright (c) Trevor Wishart, Richard Dobson and 
 * Composers Desktop Project, March 2001. This code is made freely available for non-commercial use.
 * This copyright notice should be preserved in any redistributed and/or modified version.
 */

#ifndef TRANSFORM_H_
#define TRANSFORM_H_

/* definition of the phase vococder class wrapper */
#include "../cdp/pvpp.h"

//#include "../../../../HelperStuff.h"


extern "C" {
#include "../cdp/plugins.h"
}

/* You may notice that these classes have a lot in common. I noticed this too! */


class spectralexaggerator {

public:
	spectralexaggerator();
	virtual ~spectralexaggerator();
	bool init(int samplerate,int fftlen,int overlap,pvocmode mode);
	float tick(float insamp,float exag);

private:
	phasevocoder *inpv, *outpv;
	float *frame;
	float *inbuf,*outbuf;
	int inptr,outptr,decfac,srate;
	int nbins;
protected:
	void cleanup();
};





class spectraltransposer {
public:
	spectraltransposer();
	virtual ~spectraltransposer();
	bool init(int samplerate,int fftlen,int overlap,pvocmode mode);
	float tick(float insamp,float semitones);
    void tickForw(float insamp);
	float tickInv ();

	phasevocoder *inpv, *outpv;
	float *frame;
	float *inbuf,*outbuf;
	float *amp,*freq;
	float nyquist,chwidth;
	int inptrInv,outptrInv,inptrForw,outptrForw,decfac,srate;
	int nbins;

	
protected:
	void cleanup();

};



class accutransformer {

public:
	accutransformer();
	virtual ~accutransformer();
	bool init(int samplerate,int fftlen,int overlap,pvocmode mode);
	float tick(float insamp,float decay,float glis);
	
private:
	phasevocoder *inpv, *outpv;
	float *frame, *framestore;
	float *inbuf,*outbuf;
	float nyquist,arate,frametime;
	int inptr,outptr,decfac,srate;
	int nbins;
protected:
	void cleanup();
};

#endif
