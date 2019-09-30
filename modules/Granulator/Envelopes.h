
#ifndef _ENVELOPES_H_
#define _ENVELOPES_H_

#define ENV_VAR_MIN 0.0002f
#define ENV_VAR_MAX 1.0f

//-------------------------------------------------------------------------
//
class Envelope
{
public:
    Envelope(int size = 10)
    { };

    virtual ~Envelope(){};

    virtual void setVariables(float skew){envelopeVar = skew;};
    virtual void getGraphPoints(TPointF Points[], int NumOfPoints){};

    virtual void processEnvelope(int index, float& amp) = 0;

    void setSize(int size) { envelopeSize = size;};

protected:
    int envelopeSize;
    float envelopeVar;

};

//-------------------------------------------------------------------------
//
class AsrEnvelope
    : public Envelope
{
public:
    AsrEnvelope(int size);
    ~AsrEnvelope(){};

    void setVariables(float skew);
    void getGraphPoints(TPointF Points[], int NumOfPoints);

protected:
    void processEnvelope(int index, float& amp);

    /// ASR variables
    float asrAttackSize, asrSustainSize;
	float asrInvAttackSize, asrInvSustainSize;
};

//-------------------------------------------------------------------------
//
class RcbEnvelope
    : public Envelope
{
public:
    RcbEnvelope(int size);
    ~RcbEnvelope(){};

    void setVariables(float skew);
    void getGraphPoints(TPointF Points[], int NumOfPoints);

protected:

    /// RCB variables
    float rcbSectionSize;
    float angle_increment, attack_angle, release_angle;
    int attack_samples, sustain_samples, release_samples, release_boundary;

    void processEnvelope(int index, float& amp);

    /// fast Cosine function
    inline float fastCosine(float angle)
    {
        angle += 1.57079632f;
        if (angle >  3.14159265f) {
            angle -= 6.28318531f;
        }
        if (angle < 0) {
            return 1.273239545f * angle + 0.405284735f * angle * angle;
        } else {
            return 1.273239545f * angle - 0.405284735f * angle * angle;
        }
    }
};

//-------------------------------------------------------------------------
//
class BezierEnvelope
    : public Envelope
{
public:
    BezierEnvelope(int size);
    ~BezierEnvelope(){};

    void setVariables(float skew);
    void getGraphPoints(TPointF Points[], int NumOfPoints);

protected:
    void processEnvelope(int index, float& amp);

    float left0, left1, left2, center;
};


//-------------------------------------------------------------------------
//
class GaussianEnvelope
    : public Envelope
{
public:
    GaussianEnvelope(int size);
    ~GaussianEnvelope(){};

    void setVariables(float skew);
    void getGraphPoints(TPointF Points[], int NumOfPoints);

protected:
    void processEnvelope(int index, float& amp);

    /// Gaussian variables
    float gaussMax, gaussSqrDev;
};

//-------------------------------------------------------------------------
//
class HanningEnvelope
    : public Envelope
{
public:
    HanningEnvelope(int size);
    ~HanningEnvelope(){};

    void setVariables(float skew);
    void getGraphPoints(TPointF Points[], int NumOfPoints);

protected:
    void processEnvelope(int index, float& amp);
        
    /// fast Cosine function
    inline float fastCosine(float angle)
    {
        angle += 1.57079632f;
        if (angle >  3.14159265f) {
            angle -= 6.28318531f;
        }
        if (angle < 0) {
            return 1.273239545f * angle + 0.405284735f * angle * angle;
        } else {
            return 1.273239545f * angle - 0.405284735f * angle * angle;
        }
    }
};

#endif // _ENVELOPES_H_
