#ifndef __AUDIO_BUFFER_H__
#define __AUDIO_BUFFER_H__

#define _USE_MATH_DEFINES

#include <cassert>
#include <cmath>
#include <cstring>
#include <vector>
#include <iostream>
#include <algorithm>
#include <math.h>



/*! \brief Audio buffer of type T
*/

/*! \enum Overlap parameters for the isReadyToProcess() function
 */
enum OverlapParameter {
    NO_OVERLAP = 1,
    OVERLAP_50
};


template<typename T>
class AudioBuffer {
public:
    /*! constructor
     \param iBufferLengthSamples : Initialize the audio buffer length in samples
     */
    AudioBuffer(int iBufferLengthSamples) :
    m_iBufferLength(iBufferLengthSamples),
    m_vecBuffer(iBufferLengthSamples,T(0)),        // Init length of the buffer and vector
    m_hannWindow(iBufferLengthSamples,T(0))        // Init length of the hanning window
    {
        assert(iBufferLengthSamples > 0);
        reset();
        initHann();
    }
    
    /*! destructor
     */
    virtual ~AudioBuffer(){
        
    }
    
    /*! add a value of type T to the circular buffer at current write index
    \param tNewValue : new value to add
    */
    void add(T tNewValue){
        m_vecBuffer[m_iWriteIdx] = tNewValue;
    }
    
    /*! add a vector of values to the circular buffer
    \param vecNewValues : Vector with the values you want to add to the circular buffer
    */
    void add(const std::vector<T>& vecNewValues){
        int iLength = static_cast<int>(vecNewValues.size());
        assert(iLength >= 0);
        
        int iSpaceRemaining = m_iBufferLength - m_iWriteIdx;
        
        if(iLength <= iSpaceRemaining){
            //no wrap-around, simply copy it
            std::memcpy(&m_vecBuffer[m_iWriteIdx], vecNewValues.data(), sizeof(T)*iLength);
        }
        else {
            //if wrap-around needed
            //copy first part
            std::memcpy(&m_vecBuffer[m_iWriteIdx], vecNewValues.data(), sizeof(T)*iSpaceRemaining);
            //copy second part
            std::memcpy(m_vecBuffer.data(),&vecNewValues.data()[iSpaceRemaining], sizeof(T)*(iLength - iSpaceRemaining));
        }
    }
    
    /*! add a value of type T to the circular buffer and increment write index
    \param newValue : new value to add
    */
    void addAndInc(T newValue){
        add(newValue);
        incIdx(m_iWriteIdx,m_iBufferLength);
    }
    
    /*! add a vector of values to the circular buffer and increment write index
    \param vecNewValues : Vector with the values you want to add to the circular buffer
    */
    void addAndInc(const std::vector<T>& vecNewValues){
        add(vecNewValues);
        int iLength = static_cast<int>(vecNewValues.size());
        incIdx(m_iWriteIdx,m_iBufferLength,iLength);
    }
    
    /*! add a tab of iLength samples to the ring buffer, wrapping around its end, and count the analysis frames:
     the first frame ends m_FftSize samples after the reset, the next ones every m_ClockTime samples (the hop)
     \param ptr_fBufferToAdd : pointer to the first value in the tab
     \param iLength : number of samples, at most the buffer length minus the fft size (a frame must stay in the buffer)
     */
    void add(const T* ptr_fBufferToAdd, int iLength)
    {
        assert(iLength >= 0 && iLength <= m_iBufferLength - m_FftSize);
        iLength = std::max(0, std::min(iLength, m_iBufferLength - m_FftSize));

        const int iFirst = std::min(iLength, m_iBufferLength - m_iWriteIdx);
        std::memcpy(&m_vecBuffer[m_iWriteIdx], ptr_fBufferToAdd, sizeof(T) * iFirst);
        std::memcpy(m_vecBuffer.data(), ptr_fBufferToAdd + iFirst, sizeof(T) * (iLength - iFirst));
        incIdx(m_iWriteIdx, m_iBufferLength, iLength);

        // a frame ended in these samples: keep the latest one, m_iFrameLag samples before the write index
        m_bFrameReady = (iLength >= m_iToNextFrame);
        if (m_bFrameReady)
        {
            m_iFrameLag = (iLength - m_iToNextFrame) % m_ClockTime;
            m_iToNextFrame = m_ClockTime - m_iFrameLag;
            m_bFirstFramePassed = true;
        }
        else
        {
            m_iToNextFrame -= iLength;
        }
    }
    
    /*! get the value in the circular buffer at the specified indice
    \param fOffset : index
    */
    T get(float fOffset = 0) const {
        if(fOffset == 0){
            return m_vecBuffer[m_iReadIdx];
        }
        else {
            // compute linear interpolation
            int iOffset = static_cast<int>(std::floor(fOffset));
            float fFrac = fOffset - iOffset;
            int iRead = m_iReadIdx + iOffset;
            while(iRead > m_iBufferLength-1){
                iRead -= m_iBufferLength;
            }
            while(iRead < 0){
                iRead += m_iBufferLength;
            }
            return (1-fFrac) * m_vecBuffer[iRead] 
            + fFrac * m_vecBuffer[(iRead+1) % m_iBufferLength];
        }
    }
    
    
    /*! return the next values in the circular buffer
    \param vecNewBuffer : vector to where values will be copied, it will resize it if needed 
    \param iLength : number of values
    */
    void get(std::vector<T>& vecNewBuffer, int iLength){
        assert(iLength <= m_iBufferLength && iLength > 0);
        
        int iSpaceRemaining = m_iBufferLength - m_iReadIdx;
        
        vecNewBuffer.resize(iLength);
        
        if(iLength <= iSpaceRemaining){
            //no wrap-around, simply copy it
            std::memcpy(vecNewBuffer.data(), &m_vecBuffer[m_iReadIdx], sizeof(T)*iLength);
        }
        else {
            //if wrap-around needed
            //copy first part
            std::memcpy(vecNewBuffer.data(), &m_vecBuffer[m_iReadIdx], sizeof(T)*iSpaceRemaining);
            //copy second part
            std::memcpy(&vecNewBuffer[iSpaceRemaining],m_vecBuffer.data(), sizeof(T)*(iLength - iSpaceRemaining));
            
        }
        
    }
    
    /*! return the value at the current read index and increment read index
    \return T value from read index 
    */
    T getAndInc(){
        T tValue = get();
        incIdx(m_iReadIdx,m_iBufferLength);
        return tValue;
    }
    /*! copy the values at the current read index  in a new vector and increment read index
    \param vecNewBuffer : vector to where values will be copied 
    \param iLength : number of values
    */
    void getAndInc(std::vector<T>& vecNewBuffer, int iLength){
        get(vecNewBuffer, iLength);
        incIdx(m_iReadIdx,m_iBufferLength,iLength);
    }
    
    /*! copy the entire buffer to another vector
    \param vecNewBuffer : vector to where values will be copied 
    */
    void copyTo(std::vector<T>& vecNewBuffer){
        int currentReadIdx = getReadIdx();
        setReadIdx(0);
        get(vecNewBuffer, m_iBufferLength);
        setReadIdx(currentReadIdx);
    }
    
    /*! copy the vector values, resize and put them in the circular buffer, reset indexes
    \param vecNewValues : vector from which values will be copied
    */
    void copyFrom(const std::vector<T>& vecNewValues){
        reset();
        int iLength = static_cast<int>(vecNewValues.size());
        if(m_iBufferLength)
        resize(iLength);
        add(vecNewValues);
    }
    
    /*! return a pointer to the first value in m_vecBuffer
     */
    T* getBufferPtr()
    {
        return m_vecBuffer.data();
    }
    
    /*! copy the m_FftSize samples of the latest frame (see isReadyToProcess) to dest, in order
     \param dest : destination, m_FftSize values
     */
    void copyFrame(T* dest) const
    {
        int iStart = m_iWriteIdx - m_iFrameLag - m_FftSize;
        while (iStart < 0)
        {
            iStart += m_iBufferLength;
        }
        const int iFirst = std::min(m_FftSize, m_iBufferLength - iStart);
        std::memcpy(dest, &m_vecBuffer[iStart], sizeof(T) * iFirst);
        std::memcpy(dest + iFirst, m_vecBuffer.data(), sizeof(T) * (m_FftSize - iFirst));
    }
    
    /*! return the current index for writing/add
    \return int
    */
    int getWriteIdx() const
    {
        return m_iWriteIdx;
    }

    /*! move the write index to a new position
     \param iNewWriteIdx new position
    */
    void setWriteIdx(int iNewWriteIdx)
    {
        incIdx(m_iWriteIdx,m_iBufferLength, iNewWriteIdx - m_iWriteIdx);
    }

    /*! return the current index for reading/get
    \return int
    */
    int getReadIdx() const
    {
        return m_iReadIdx;
    }

    /*! move the read index to a new position
     \param iNewReadIdx new position
    */
    void setReadIdx(int iNewReadIdx)
    {
        incIdx(m_iReadIdx,m_iBufferLength, iNewReadIdx - m_iReadIdx);
    }
    
    /*! resize the circular buffer
    \param iNewSize : new size of the buffer in samples
    */
    void resize(int iNewSize){
        assert(iNewSize >= 0); 
        m_vecBuffer.resize(iNewSize);
        m_iBufferLength = iNewSize;
    }
    
    int getSize() const
    {
        return static_cast<int>(m_vecBuffer.size());
    }
    
    /*! reset buffer's content and indices to 0, the next frame ends m_FftSize samples later
    */
    void reset(){
        std::fill(m_vecBuffer.begin(),m_vecBuffer.end(),T(0));
        m_iReadIdx = 0;
        m_iWriteIdx = 0;
        m_iToNextFrame = m_FftSize;
        m_iFrameLag = 0;
        m_bFrameReady = false;
        m_bFirstFramePassed = false;
    }
    
    /*! returns true when a frame of m_FftSize samples ended in the samples of the last add(): a hop boundary
     was crossed and the buffer holds at least m_FftSize samples since the reset. copyFrame() gives the latest one
     */
    bool isReadyToProcess() const
    {
        return m_bFrameReady;
    }
 
    /*! change the frame size and reset the buffer, keeping the number of frames per window
     \param fftSize : frame size, from 1 to the buffer length minus the largest number of samples added at once
     */
    void setFFTSize(int fftSize)
    {
        if(fftSize > 0 && fftSize < m_iBufferLength)
        {
            int oldClock = m_FftSize / m_ClockTime;
            m_FftSize = fftSize;
            setClockTime(oldClock);
            reset();
        }
    }
    
    /*! change the hop to m_FftSize/clockTime samples, keeping the buffer content
     \param clockTime : number of frames per window, from 1 to 4
     */
    void setClockTime(int clockTime)
    {
        if(clockTime >= 1 && clockTime <= 4)
        {
            m_ClockTime = std::max(1, m_FftSize/clockTime);
            // once the first frame has passed, the next one comes after one new hop at most
            if(m_bFirstFramePassed)
            {
                m_iToNextFrame = std::min(m_iToNextFrame, m_ClockTime);
            }
        }
        
    }

    /*! return the frame size
     */
    int getFFTSize() const
    {
        return m_FftSize;
    }
    
    
    /*! apply the hanning window function to the ring buffer
     */
    void applyHann()
    {
        std::transform(m_vecBuffer.begin(), m_vecBuffer.end(),
                       m_hannWindow.begin(),
                       m_vecBuffer.begin(),
                       std::multiplies<>());
    }

    /*! initialize the hann window vector
     */
    void initHann()
    {
        for(int i = 0; i < m_iBufferLength; i++)
        {
            m_hannWindow[i] = static_cast<T>(0.5 * (1.0 - std::cos((2.0 * M_PI * i) / (m_iBufferLength - 1))));
        }
        
    }
    

    
#ifdef ENABLE_PRINT_CONTENT
    //DEBUGGING FUNCTION
    /*! To use, define ENABLE_PRINT_CONTENT. Print the content of the buffer to the console.
     */
    void printContentBuffer() const {
        printf("Printing the content of the circular buffer ... \n");
        for(int i = 0; i < m_iBufferLength; i++){
            std::cout << "[" << i << "] :" << m_vecBuffer[i] << "\n";
        }
        printf("Print ended... \n");
    }
#endif //ENABLE_PRINT_CONTENT
    
public:
    //public variables
    int m_FftSize = 4096;
    
private:
    // private variables
    int m_iBufferLength  = 0,
        m_iReadIdx       = 0,
        m_iWriteIdx      = 0;
        
    // variables for fft: frames of m_FftSize samples, one every m_ClockTime samples (the hop)

    int m_iToNextFrame = 4096;          // samples to add before the end of the next frame
    int m_iFrameLag = 0;                // samples added after the end of the latest frame
    bool m_bFrameReady = false;         // a frame ended in the last add()
    bool m_bFirstFramePassed = false;   // a full frame was added since the reset
    
    int m_ClockTime = 4096;
    
    std::vector<T> m_vecBuffer;
    
    std::vector<T> m_hannWindow;
    
    // private methods
    void incIdx(int& iIdx, int buffer_size, int iOffset = 1)
    {
        while((iIdx + iOffset) < 0)
        {
            iOffset += buffer_size;
        }
        iIdx = (iIdx + iOffset) % buffer_size;
    }
    
    
    
};

#endif //__AUDIO_BUFFER_H__







