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
 *	These values of the MPX4100's Constant1 and Constant2
 *	are taken from the Transfer Function on page 6 of MPX4100A.pdf, 2024-07-03
 */
#define kNXPMPX4100ASensorCalibrationConstant1 (0.1518)
#define kNXPMPX4100ASensorCalibrationConstant2 (0.01059)

#define kNXPMPX4100ADefaultInputVariableVsensorUniformDistLow  (2.3)
#define kNXPMPX4100ADefaultInputVariableVsensorUniformDistHigh (2.7)
#define kNXPMPX4100ADefaultInputVariableVsupplyUniformDistLow  (4.8)
#define kNXPMPX4100ADefaultInputVariableVsupplyUniformDistHigh (5.4)

/*
 *	Input Variables:
 *		kNXPMPX4100AInputVariableIndexVsensorADC	: Ratiometric Analog Voltage (in Volt)
 *		kNXPMPX4100AInputVariableIndexVsupplyADC	: Supply Voltage (in Volt)
 */
typedef enum
{
	kNXPMPX4100AInputVariableIndexVsensorADC   = 0,
	kNXPMPX4100AInputVariableIndexVsupplyADC   = 1,
	kNXPMPX4100AInputVariableIndexMax,
} NXPMPX4100AInputVariableIndex;

/*
 *	Output Variable:
 *		kOutputVariableIndexCalibratedSensorOutput	: Calibrated Pressure Output (in kPa)
 */
typedef enum
{
	kNXPMPX4100AOutputVariableIndexCalibratedSensorOutput = 0,
	kNXPMPX4100AOutputVariableIndexMax,
} NXPMPX4100AOutputVariableIndex;

/**
 *	@brief	Sensor calibration routine taken from the Transfer Function on page 6 of
 *		MPX4100A-3139452.pdf, 2024-07-03.
 *
 *	@param	inputVariables	: The array of input variables used in the calculation.
 *	@param	outputVariables	: An array of output variables. Writes the result to
 *					  `outputVariables[kOutputVariableIndexCalibratedSensorOutput]`.
 *	@return	double			: Returns the distributional value calculated.
 */
double
NXPMPX4100A_calculateOutput(double * inputVariables, double *  outputVariables);
