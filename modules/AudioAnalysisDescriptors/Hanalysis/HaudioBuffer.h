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
    
    /*! add a tab of size iLength to the buffer with memcpy, make sure that the buffer is large enough
     \param ptr_fBufferToAdd : pointer to the first value in the tab
     \param iLength : length of the buffer to add, must be a multiple of m_iBufferLength
     */
    void add(const T* ptr_fBufferToAdd, int iLength)
    {
        assert(m_iBufferLength >=  iLength);

        std::memcpy(&m_vecBuffer[m_iWriteIdx], ptr_fBufferToAdd, sizeof(T)*iLength);
        incIdx(m_iWriteIdx,m_iBufferLength,iLength); 
        incIdx(m_FftIdx, m_FftSize, iLength);
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
    
    /*! return a pointer to the first value in m_vecBuffer on which we need to do the FFT
     */
    float* getProcessBufferPtr()
    {
        int currentPos = m_iWriteIdx;
        if(currentPos == 0)
        {
            currentPos = m_iBufferLength;
        }
        if(currentPos - m_FftSize > 0 && currentPos - m_FftSize < m_iBufferLength)
        {
            return &m_vecBuffer[currentPos - m_FftSize];
        }
        return nullptr;
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
    
    /*! reset buffer's content and indices to 0
    */
    void reset(){
        std::fill(m_vecBuffer.begin(),m_vecBuffer.end(),T(0));
        m_iReadIdx = 0;
        m_iWriteIdx = 0;
        m_FftIdx = 0;
        m_FirstFFTPassed = false;
    }
    
    /*! returns true if the buffer is full
     */
    bool isFull() const
    {
        return (m_iWriteIdx % m_iBufferLength == 0);
    }
    
    /*! Copies the needed part for overlap to the beginning of the buffer
     */
    void copyToBeginning()
    {
        if((m_FftSize - m_ClockTime) < m_iBufferLength && m_FftSize - m_ClockTime != 0)
        {
            std::memcpy(m_vecBuffer.data(), &m_vecBuffer[m_iBufferLength - (m_FftSize - m_ClockTime)], sizeof(T) * (m_FftSize - m_ClockTime));
            //printContentBuffer();
            incIdx(m_iWriteIdx, m_iBufferLength,(m_FftSize - m_ClockTime));
        }
    }
    
    /*! returns true when the buffer is ready to process. Depending on FFT size and Clock time
     */
    bool isReadyToProcess()
    {
        // for first FFT
        if((m_FftIdx % m_FftSize) == 0 && m_FirstFFTPassed == false)
        {
            m_FirstFFTPassed = true;
            return true;
        }
        // for other FFT with overlap
        else if((m_iWriteIdx % m_ClockTime == 0) && m_FirstFFTPassed == true )
        {
            return true;
        }
        

        return false;
    }
 
    void setFFTSize(int fftSize)
    {
        if(fftSize > 0) // and check if power of 2
        {
            reset();
            int oldClock = m_FftSize / m_ClockTime;
            m_FftSize = fftSize;
            setClockTime(oldClock);
        }
    }
    
    void setClockTime(int clockTime)
    {
        if(clockTime >= 1 && clockTime <= 4)
        {
            m_ClockTime = m_FftSize/clockTime;
        }
        
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
        
    // variables for fft

    int m_FftIdx  = 0;
    bool m_FirstFFTPassed = false;
    
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







