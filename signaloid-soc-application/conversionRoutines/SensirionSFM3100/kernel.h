/*
 *	Copyright (c) 2024, Signaloid.
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in all
 *	copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	SOFTWARE.
 */

#pragma once


/*
 *	These constants are based on the example in the SFM3100 datasheet,
 *	which has an example with (A/B) = 80 and (Offset/B) == 0.67.
 */
#define kSensirionSFM3100SensorCalibrationConstantSFM3100constA         (80)
#define kSensirionSFM3100SensorCalibrationConstantSFM3100constB         (1)
#define kSensirionSFM3100SensorCalibrationConstantSFM3100constOffset    (0.67)

#define kSensirionSFM3100DefaultInputDistributionUvUniformDistLow   (0.7)
#define kSensirionSFM3100DefaultInputDistributionUvUniformDistHigh  (0.8)

/*
 *	Input Variable:
 *		kInputVariableIndexAout	: Voltage Flow (in Volt).
 */
typedef enum
{
	kSensirionSFM3100InputVariableIndexUv = 0,
	kSensirionSFM3100InputVariableIndexMax,
} SensirionSFM3100InputVariableIndex;

/*
 *	Output Variable:
 *		kOutputVariableIndexCalibratedFlow	: Calibrated Air Flow (in slm).
 */
typedef enum
{
	kSensirionSFM3100OutputVariableIndexCalibratedFlow = 0,
	kSensirionSFM3100OutputVariableIndexMax,
} SensirionSFM3100OutputVariableIndex;

/**
 *	@brief	Sensor calibration routine taken from Section 3.1 in page 5 of SFM3100 datasheet.
 *
 *	@param	inputVariables		: The array of input variables used in the calculation.
 *	@param	outputVariables		: An array of output variables. Writes the result to
 *					  `outputVariables[kOutputVariableIndexCalibratedFlow]`.
 *	@return	double			: Returns the distributional value calculated.
 */
double
SensirionSFM3100_calculateOutput(double * inputVariables, double *  outputVariables);
