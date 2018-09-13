#include "vectorstats.h"

/**
 * This is the implementation of the Smoothed Z-Score Algorithm.
 * This is direction translation of https://stackoverflow.com/a/22640362/1461896.
 *
 * @param input - input signal
 * @param lag - the lag of the moving window
 * @param threshold - the z-score at which the algorithm signals
 * @param influence - the influence (between 0 and 1) of new signals on the mean and standard deviation
 * @return a hashmap containing the filtered signal and corresponding mean and standard deviation.
 */



