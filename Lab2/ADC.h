// adc.h
// Jeremiah Bartlett
// jjb2954

/** 
 * Initialize the ADC('s) to be used later
 * 
 * @param bmask Initialize one or more ADCs. Bits 
 * 0-10 are used to designate which ADCs to turn on.
 */
void ADC_Init(uint32_t bmask);

/**
 * Choose which ADC to pull data from
 *
 * @param channelNum the ADC 0-11 to use
 * 
 * @return 0 if successful
 */
int ADC_Open(uint32_t channelNum);

/**
 * Returns the value from the adc that
 * is open
 *
 * @return the value from the ADC
 */
uint16_t ADC_In(void);

/**
 * Collect a number of samples
 *
 * @param channelNum The ADC 0-11 to use
 * @param fs The frequency to sample
 * @param buffer The output buffer to store values
 * @param numberOfSamples The number of samples
 * 
 * @return a buffer filled with ADC values
 */
void ADC_Collect(uint32_t channelNum, uint32_t fs, void(task(unsigned long data)));
/**
 * Returns the status of the ADC_Collect function
 *
 * @return 0 if the function is complete
 */
int ADC_Status(void);
