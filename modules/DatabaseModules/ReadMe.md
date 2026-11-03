# Database Creation

## Database format

The SQL database has a row for each sound file analyzed.

- unique key (`id`)
- complete path to the sound file (`filePath`)
- duration in seconds (`duration`)
- sample rate (`sampleRate`)
- number of samples in the file (`numSamples`)

- spectral centroid (`spectralCentroid`)
- spectral spread (`spectralSpread`)
- spectral flatness (`spectralFlatness`)
- RMS (`RMS`)
- MFCC (`mfcc_1` to `mfcc_13`)
- pitch (pYIN) (`f0_frequency`)
- pitch probability (pYIN) (`f0_probability`)

for each descriptor we calculate :
- mean (`_mean`)
- standard deviation (`_std`)

For the resynthesis audio output, we store the mean of the magnitudes of the spectrums :
- mean of the magnitudes (`mag_mean`)

We use the t-SNE algorithm for the cluster representation, their coordinates are stored in the database as : 
- t-SNE X coordinate (`tsne_x`)
- t-SNE Y coordinate (`tsne_y`)

Which makes a total of 46 columns.   
You can use tools like [sqlite-viewer](https://inloop.github.io/sqlite-viewer/) to inspect the database.

## Detailled functionning

Once a sound folder path is entered, we analyze it recursively and gather all the sound files paths matching the duration criteria and with the correct extension (`.wav`, `.aiff`).   
When we trigger the analysis, we will go through each individual file and store the analysis results for each frame.
At the start of the analysis, we store the "meta-data" of the files : its path, duration, sampleRate and number of samples.   
We have two different frames. The first one is for the audio descriptors with 2048 samples length and no overlap. The second one is for the magnitudes spectrum with the specified size from the fft size entry and an overlap of 75% (hop-size of specified size divided by 4 samples).    
At the end of a file, we have a vector of length equals to the number of frames for each descriptor, on which we compute the needed statistics and store them. This architecture allows us to limit the memory size needed which can be quite significant for large sound banks.   
For the magnitudes spectrum, we sum each frame spectrum and finally divide it by the number of frames.   
Once all files analyzed, we are left with a vector of size equals to the number of sound files which our database to store.   
We use the SQL wrapper for C++ `sqlite3` allowing us to use SQL functions inside a C++ source code. Hence, we create a database with a column for each entry inside of data structure and a row for each file.
Finally, we use the t-SNE algorithm to compute for us a clusterized 2D representation. We store the X and Y coordinates of each file in the database to spare us the calculations in the Database Representation module.

# Database Player

## Detailled functionning

### Opening a database and making a request

This module takes as an entry a database made with the Database Creation module. Upon opening the database, we retrieve the FFT size used during database creation to correctly resize vectors required for resynthesis. We then create a vector with our points and our magnitudes since they are independent of the dimensions.    
Magnitudes and points are stored in separate vectors, each point holds a shared pointer to its corresponding magnitude vector, and they are also linked by a unique identifier upon creation.  
We can then start displaying the dimensions asked by the user or the patch. For that, we create a SQL query to the database using the requested dimensions to gather the X and Y coordinates of our points. The SQL query normalizes the coordinates between the minimum and the maximum value in the dimension :    

<img src="https://latex.codecogs.com/svg.image?\begin{align*}\text{norm}_x&=\frac{x-\min(x)}{\max(x)-\min(x)}\\\text{norm}_y&=\frac{y-\min(y)}{\max(y)-\min(y)}\end{align*}" title="\begin{align*}\text{norm}_x&=\frac{x-\min(x)}{\max(x)-\min(x)}\\\text{norm}_y&=\frac{y-\min(y)}{\max(y)-\min(y)}\end{align*}" />

We then update each point's screen coordinates and color based on its normalized X and Y values.   
We use KD-Tree data structure to perform spatial queries around the cursor. Since it is static, a new KD-Tree is constructed at each request.

### Action Queue

We have a dedicated thread to perform large operations. This thread uses an action queue which it processes sequentially, allowing us to have no data races and lock-free execution since a single function can be executed at the same time. The only locking occurs when pushing a new operation to the queue or retrieving the next function to execute. We have a high priority on the IFFT function since it is the most critical part. If the IFFT operation is present in the queue, it is always prioritized and executed before any other task. The search function is pushed to the queue every 5 milliseconds if a database is opened from a separate thread. To prevent unbounded task accumulation, we cap the queue to a maximum of 100 pending operations (it can happen if the execution of the operations is slower than the rate at which we insert operations).

### Samples Queue

We have a dedicated class that handles the samples strings and coefficients output. This class is mainly composed of a vector of 40 SampleStrings, a dedicated structure for each entry.    
Each SampleString contains the sample path, a pointer to the corresponding magnitude vector element, and a coefficient structure that manages smoothing.   
When executing a research, we gather every point inside our cursor, put them inside a vector and sort them by increasing distance to the center. We then compare the result of the research to our samples queue. 
If a result from the research is already present in the samples queue then we only update its distance and move to the next.   
If it is a new result, we look for empty space in the samples queue. If an empty slot is found, the sample is inserted into the queue.   
If we have a sample in the queue that is not present in the search result, we know that we need to clear it. To handle this, each SampleString includes a boolean flag indicating whether the sample is in a release phase. Now that he is in that state, it is not anymore counted as an active sample, and we will update its coefficient to get to zero in the specified release time.   
To monitor our available space in the queue, we go through it at every insert or clearing operation counting every sample and its state. The number of concurrently active samples is limited by the polyphony parameter, and the total number of managed samples (including releasing ones) is 2 × polyphony. Hence, for a polyphony of 10, we can have 10 active samples and 10 releasing samples at the same time (or 5 active and 15 releasing, etc.) making a total of 20 samples playing simultaneously.    
The smoothing of the coefficients is made in the audio thread by a simple linear smoothing. We have a target coefficient to reach by the specified release or attack time : we calculate a step decrement or increment by knowing the current coefficient and the number of processing buffers corresponding to the attack or release duration.
Sample paths and coefficient values are collected in the audio thread, which iterates over all non-empty entries to produce a comma-separated list and gather the currents coefficient values.

