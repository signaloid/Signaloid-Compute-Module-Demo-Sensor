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

#include <stdint.h>


/*
 *	These example values of the FLS110's C1, C2, and C3
 *	are taken from the screenshot on page 6 of FL-000986-TN-7, 2022-01-30.
 */
#define kFlussoFLS110SensorCalibrationConstant1 (2499.26)
#define kFlussoFLS110SensorCalibrationConstant2 (117682.20)
#define kFlussoFLS110SensorCalibrationConstant3 (-314364.00)

#define kFlussoFLS110DefaultInputVariableHxferUniformDistLow    (0.010)
#define kFlussoFLS110DefaultInputVariableHxferUniformDistHigh   (0.050)

#define kFlussoFLS110DefaultInputVariableTflowUniformDistLow    (293.0)
#define kFlussoFLS110DefaultInputVariableTflowUniformDistHigh   (294.0)

#define kFlussoFLS110DefaultInputVariableT0UniformDistLow   (273.0)
#define kFlussoFLS110DefaultInputVariableT0UniformDistHigh  (273.5)

#define kFlussoFLS110DefaultInputVariablePflowUniformDistLow    (420000.00)
#define kFlussoFLS110DefaultInputVariablePflowUniformDistHigh   (425000.00)

#define kFlussoFLS110DefaultInputVariableP0UniformDistLow   (400000.00)
#define kFlussoFLS110DefaultInputVariableP0UniformDistHigh  (405000.00)

/*
 *	Input Variables:
 *		kInputVariableIndexHxfer	: Heat power transfer (in Watt)
 *		kInputVariableIndexTflow	: Flow temperature (in Kelvin)
 *		kInputVariableIndexT0	: Temperature at time 0 (in Kelvin)
 *		kInputVariableIndexPflow	: Flow pressure (in Pascal)
 *		kInputVariableIndexP0	: Pressure at time 0 (in Pascal)
 */
typedef enum
{
	kFlussoFLS110InputVariableIndexHxfer    = 0,
	kFlussoFLS110InputVariableIndexTflow    = 1,
	kFlussoFLS110InputVariableIndexT0       = 2,
	kFlussoFLS110InputVariableIndexPflow    = 3,
	kFlussoFLS110InputVariableIndexP0       = 4,
	kFlussoFLS110InputVariableIndexMax,
} FlussoFLS110InputVariableIndex;

/*
 *	Output Variables:
 *		kOutputVariableIndexCalibratedMassFlowOutput		: Mass flow (in sccm)
 *		kOutputVariableIndexCalibratedDifferentialPressureOutput	: Differential Pressure (in Pascal)
 */
typedef enum
{
	kFlussoFLS110OutputVariableIndexCalibratedMassFlowOutput                = 0,
	kFlussoFLS110OutputVariableIndexCalibratedDifferentialPressureOutput    = 1,
	kFlussoFLS110OutputVariableIndexMax,
} FlussoFLS110OutputVariableIndex;

/**
 *	@brief	Sensor calibration routines taken from the screenshot on page 6 of
 *		FL-000986-TN-7, 2022-01-30.
 *
 *	@param	arguments		: Pointer to command line arguments struct.
 *	@param	inputVariables	: The array of input variables used in the calculation.
 *	@param	outputVariables	: An array of output variables. Writes the result to `outputVariables[outputSelectValue]`.
 *	@return	double			: Returns the distributional value calculated.
 */
double
FlussoFLS110_calculateOutput(
	uint8_t                 outputSelect,
	double *                inputVariables,
	double *                outputVariables);
