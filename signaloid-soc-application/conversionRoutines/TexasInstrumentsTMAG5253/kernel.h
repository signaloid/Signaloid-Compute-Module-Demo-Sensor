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
 *	These constant values are taken from Equation 11 in page 15
 *	of TMAG5253 Datasheet, 2024-07-03.
 */
#define kTexasInstrumentsTMAG5253SensorCalibrationConstantTMAG5253BA3VccNominal     (3.3)
#define kTexasInstrumentsTMAG5253SensorCalibrationConstantTMAG5253BA3Sensitivity    (0.015)

#define kTexasInstrumentsTMAG5253DefaultInputDistributionVoutUniformDistLow     (2.6)
#define kTexasInstrumentsTMAG5253DefaultInputDistributionVoutUniformDistHigh    (2.8)
#define kTexasInstrumentsTMAG5253DefaultInputDistributionVccUniformDistLow      (3.2)
#define kTexasInstrumentsTMAG5253DefaultInputDistributionVccUniformDistHigh     (3.4)

/*
 *	Input Variables:
 *		kInputVariableIndexVout	: Ratiometric Analog Voltage Output (in Volt)
 *		kInputVariableIndexVcc	: Supply Voltage (in Volt)
 */
typedef enum
{
	kTexasInstrumentsTMAG5253InputVariableIndexVout = 0,
	kTexasInstrumentsTMAG5253InputVariableIndexVcc  = 1,
	kTexasInstrumentsTMAG5253InputVariableIndexMax,
} TexasInstrumentsTMAG5253InputVariableIndex;

/*
 *	Output Variable:
 *		kOutputVariableIndexCalibratedMagneticFluxDensity	: Calibrated Magnetic Flux Density (in mT)
 */
typedef enum
{
	kTexasInstrumentsTMAG5253OutputVariableIndexCalibratedMagneticFluxDensity = 0,
	kTexasInstrumentsTMAG5253OutputVariableIndexMax,
} TexasInstrumentsTMAG5253OutputVariableIndex;

/**
 *	@brief  Sensor calibration routine taken from Equation 11 in page 15
 *		of tmag5253.pdf, 2024-07-03.
 *
 *	@param  inputVariables	: The array of input variables used in the calculation.
 *	@param  outputVariables	: An array of output variables.
 *				  Writes the result to `outputVariables[kOutputVariableIndexCalibratedMagneticFluxDensity]`.
 *	@return	double		: Returns the distributional value calculated.
 */
double
TexasInstrumentsTMAG5253_calculateOutput(double * inputVariables, double *  outputVariables);
