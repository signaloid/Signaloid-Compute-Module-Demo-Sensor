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
 *	These values of the MPXx6250's Constant1 and Constant2
 *	are taken from the Nominal Transfer value on page 6 of MPXH6250A.pdf, 2024-07-03.
 */
#define kNXPMPXx6250ASensorCalibrationConstant1 (0.040)
#define kNXPMPXx6250ASensorCalibrationConstant2 (0.004)

#define kNXPMPXx6250ADefaultInputDistributionVsensorUniformDistLow  (2.3)
#define kNXPMPXx6250ADefaultInputDistributionVsensorUniformDistHigh (2.7)
#define kNXPMPXx6250ADefaultInputDistributionVsupplyUniformDistLow  (4.8)
#define kNXPMPXx6250ADefaultInputDistributionVsupplyUniformDistHigh (5.4)

/*
 *	Input Variables:
 *		kInputVariableIndexVsensorADC	: Ratiometric Analog Voltage (in Volt)
 *		kInputVariableIndexVsupplyADC	: Supply Voltage (in Volt)
 */
typedef enum
{
	kNXPMPXx6250AInputVariableIndexVsensorADC   = 0,
	kNXPMPXx6250AInputVariableIndexVsupplyADC   = 1,
	kNXPMPXx6250AInputVariableIndexMax,
} NXPMPXx6250AInputVariableIndex;

/*
 *	Output Variable:
 *		kOutputVariableIndexCalibratedSensorOutput	: Calibrated Pressure Output (in kPa)
 */
typedef enum
{
	kNXPMPXx6250AOutputVariableIndexCalibratedSensorOutput = 0,
	kNXPMPXx6250AOutputVariableIndexMax,
} NXPMPXx6250AOutputVariableIndex;

/**
 *	@brief  Sensor calibration routine taken from the Nominal Transfer value on page 6
 *		of MPXH6250A-3139207.pdf, 2024-07-03.
 *
 *	@param  inputVariables	: The array of input variables used in the calculation.
 *	@param  outputVariables	: An array of output variables. Writes the result to
 *				  `outputVariables[kOutputVariableIndexCalibratedSensorOutput]`.
 *	@return	double		: Returns the distributional value calculated.
 */
double
NXPMPXx6250A_calculateOutput(double * inputVariables, double *  outputVariables);
