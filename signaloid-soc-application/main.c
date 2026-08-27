/*
 *	Copyright (c) 2026, Signaloid.
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


#include <stdint.h>
#include <math.h>
#include <uxhw.h>
#include "C0HAL.h"


#ifdef INCLUDE_FLIRAx5
#include "conversionRoutines/FLIRAx5/kernel.h"
#endif

#ifdef INCLUDE_FlussoFLS110
#include "conversionRoutines/FlussoFLS110/kernel.h"
#endif

#ifdef INCLUDE_NXPMPX4100A
#include "conversionRoutines/NXPMPX4100A/kernel.h"
#endif

#ifdef INCLUDE_NXPMPXx6250A
#include "conversionRoutines/NXPMPXx6250A/kernel.h"
#endif

#ifdef INCLUDE_SensirionSDP3x
#include "conversionRoutines/SensirionSDP3x/kernel.h"
#endif

#ifdef INCLUDE_SensirionSDP8xx
#include "conversionRoutines/SensirionSDP8xx/kernel.h"
#endif

#ifdef INCLUDE_SensirionSFM3100
#include "conversionRoutines/SensirionSFM3100/kernel.h"
#endif

#ifdef INCLUDE_SensirionSHT3xARP
#include "conversionRoutines/SensirionSHT3xARP/kernel.h"
#endif

#ifdef INCLUDE_SensirionSHT4xI
#include "conversionRoutines/SensirionSHT4xI/kernel.h"
#endif

#ifdef INCLUDE_TexasInstrumentsTMAG5253
#include "conversionRoutines/TexasInstrumentsTMAG5253/kernel.h"
#endif

#ifdef INCLUDE_TexasInstrumentsTMCS112x
#include "conversionRoutines/TexasInstrumentsTMCS112x/kernel.h"
#endif



typedef enum
{
	kCalculateNoCommand                 = 0,
	kFLIRAx5Convert                     = 1,
	kFlussoFLS110Convert                = 2,
	kNXPMPX4100AConvert                 = 3,
	kNXPMPXx6250AConvert                = 4,
	kSensirionSDP3xConvert              = 5,
	kSensirionSDP8xxConvert             = 6,
	kSensirionSFM3100Convert            = 7,
	kSensirionSHT3xARPConvert           = 8,
	kSensirionSHT4xIConvert             = 9,
	kTexasInstrumentsTMAG5253Convert    = 10,
	kTexasInstrumentsTMCS112xConvert    = 11,
} SignaloidSoCCommand;


/*
 * Helper functions
 */

SignaloidSoCCommand
waitForCommand(void)
{
	SignaloidSoCCommand command = kCalculateNoCommand;

	/*
	 *	Set status to "waitingForCommand"
	 */
	C0HALSetStatusRegister(kSignaloidSoCStatusWaitingForCommand);

	/*
	 *	Block until command is issued
	 */
	while (command == kCalculateNoCommand)
	{
		command = C0HALGetCommandRegister();
	}

	return command;
}

void
waitForIdle(void)
{
	/*
	 *	Block until command is cleared
	 */
	while (C0HALGetCommandRegister() != kCalculateNoCommand) {}
}

void
readInputVariables(double * inputVariables, uint8_t count)
{
	for (uint8_t i = 0; i < count; i++)
	{
		inputVariables[i] = (double) UxHwFloatUniformDist(
			kC0HALInputBufferFloat[i * 2],
			kC0HALInputBufferFloat[(i * 2) + 1]
		);
	}
}


void
writeOutputVariables(double * outputVariables, uint8_t count)
{
	volatile uint8_t *  outputBufferTemp    = kC0HALOutputBufferUint8 + sizeof(uint32_t);
	uint8_t *           outputBufferEnd     = ((uint8_t *) kC0HALOutputBufferUint8) + kC0HALOutputBufferUint8Length;
	int                 remainingBufferSize = 0;
	ssize_t             resultSize;

	kC0HALOutputBufferUint32[0] = count;

	for (uint8_t i = 0; i < count; i++)
	{
		remainingBufferSize = ((int) outputBufferEnd) - ((int) outputBufferTemp) - ((int) sizeof(uint32_t));
		if (remainingBufferSize <= 0)
		{
			return;
		}

		resultSize = UxHwFloatDistributionToByteArray(
			outputVariables[i],
			(uint8_t *) (outputBufferTemp + sizeof(uint32_t)),
			(uint32_t) remainingBufferSize
		);
		*((uint32_t *) outputBufferTemp)    = (uint32_t) resultSize;
		outputBufferTemp                    += ((uint32_t) resultSize) + sizeof(uint32_t);

		/* Align to 4-byte boundary */
		/* C0-microSD blocks if not aligned */
		outputBufferTemp += (4 - (resultSize % 4)) % 4;
	}
}


/*
 * Sensor conversion routine handlers
 */

#ifdef INCLUDE_FLIRAx5
void
handleFLIRAx5(uint16_t iterationCount)
{
	double  inputVariables[kFLIRAx5InputVariableIndexMax];
	double  outputVariables[kFLIRAx5OutputVariableIndexMax];

	C0HALSetStatusRegister(kSignaloidSoCStatusCalculating);
	C0HALSetLed(true);

	readInputVariables(inputVariables, kFLIRAx5InputVariableIndexMax);

	for (uint16_t iteration = 0; iteration < iterationCount; iteration++)
	{
		FLIRAx5_calculateOutput(
			(double) NAN,
			inputVariables,
			outputVariables
		);
	}

	writeOutputVariables(outputVariables, kFLIRAx5OutputVariableIndexMax);

	C0HALSetStatusRegister(kSignaloidSoCStatusDone);
	C0HALSetLed(false);
}
#endif

#ifdef INCLUDE_FlussoFLS110
void
handleFlussoFLS110(uint16_t iterationCount)
{
	double  inputVariables[kFlussoFLS110InputVariableIndexMax];
	double  outputVariables[kFlussoFLS110OutputVariableIndexMax];

	C0HALSetStatusRegister(kSignaloidSoCStatusCalculating);
	C0HALSetLed(true);

	readInputVariables(inputVariables, kFlussoFLS110InputVariableIndexMax);

	for (uint16_t iteration = 0; iteration < iterationCount; iteration++)
	{
		FlussoFLS110_calculateOutput(
			kFlussoFLS110OutputVariableIndexMax,
			inputVariables,
			outputVariables
		);
	}

	writeOutputVariables(outputVariables, kFlussoFLS110OutputVariableIndexMax);

	C0HALSetStatusRegister(kSignaloidSoCStatusDone);
	C0HALSetLed(false);
}
#endif

#ifdef INCLUDE_NXPMPX4100A
void
handleNXPMPX4100A(uint16_t iterationCount)
{
	double  inputVariables[kNXPMPX4100AInputVariableIndexMax];
	double  outputVariables[kNXPMPX4100AOutputVariableIndexMax];

	C0HALSetStatusRegister(kSignaloidSoCStatusCalculating);
	C0HALSetLed(true);

	readInputVariables(inputVariables, kNXPMPX4100AInputVariableIndexMax);

	for (uint16_t iteration = 0; iteration < iterationCount; iteration++)
	{
		NXPMPX4100A_calculateOutput(
			inputVariables,
			outputVariables
		);
	}

	writeOutputVariables(outputVariables, kNXPMPX4100AOutputVariableIndexMax);

	C0HALSetStatusRegister(kSignaloidSoCStatusDone);
	C0HALSetLed(false);
}
#endif

#ifdef INCLUDE_NXPMPXx6250A
void
handleNXPMPXx6250A(uint16_t iterationCount)
{
	double  inputVariables[kNXPMPXx6250AInputVariableIndexMax];
	double  outputVariables[kNXPMPXx6250AOutputVariableIndexMax];

	C0HALSetStatusRegister(kSignaloidSoCStatusCalculating);
	C0HALSetLed(true);

	readInputVariables(inputVariables, kNXPMPXx6250AInputVariableIndexMax);

	for (uint16_t iteration = 0; iteration < iterationCount; iteration++)
	{
		NXPMPXx6250A_calculateOutput(
			inputVariables,
			outputVariables
		);
	}

	writeOutputVariables(outputVariables, kNXPMPXx6250AOutputVariableIndexMax);

	C0HALSetStatusRegister(kSignaloidSoCStatusDone);
	C0HALSetLed(false);
}
#endif

#ifdef INCLUDE_SensirionSDP3x
void
handleSensirionSDP3x(uint16_t iterationCount)
{
	double  inputVariables[kSensirionSDP3xInputVariableIndexMax];
	double  outputVariables[kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax];

	C0HALSetStatusRegister(kSignaloidSoCStatusCalculating);
	C0HALSetLed(true);

	readInputVariables(inputVariables, kSensirionSDP3xInputVariableIndexMax);

	for (uint16_t iteration = 0; iteration < iterationCount; iteration++)
	{
		SensirionSDP3x_calculateOutput(
			kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax,
			inputVariables,
			outputVariables
		);
	}

	writeOutputVariables(outputVariables, kSensirionSDP3xOutputVariableIndexCalibratedSensorOutputMax);

	C0HALSetStatusRegister(kSignaloidSoCStatusDone);
	C0HALSetLed(false);
}
#endif

#ifdef INCLUDE_SensirionSDP8xx
void
handleSensirionSDP8xx(uint16_t iterationCount)
{
	double  inputVariables[kSensirionSDP8xxInputVariableIndexMax];
	double  outputVariables[kSensirionSDP8xxOutputVariableIndexCalibratedSensorOutputMax];

	C0HALSetStatusRegister(kSignaloidSoCStatusCalculating);
	C0HALSetLed(true);

	readInputVariables(inputVariables, kSensirionSDP8xxInputVariableIndexMax);

	for (uint16_t iteration = 0; iteration < iterationCount; iteration++)
	{
		SensirionSDP8xx_calculateOutput(
			kSensirionSDP8xxOutputVariableIndexCalibratedSensorOutputMax,
			inputVariables,
			outputVariables
		);
	}

	writeOutputVariables(outputVariables, kSensirionSDP8xxOutputVariableIndexCalibratedSensorOutputMax);

	C0HALSetStatusRegister(kSignaloidSoCStatusDone);
	C0HALSetLed(false);
}
#endif

#ifdef INCLUDE_SensirionSFM3100
void
handleSensirionSFM3100(uint16_t iterationCount)
{
	double  inputVariables[kSensirionSFM3100InputVariableIndexMax];
	double  outputVariables[kSensirionSFM3100OutputVariableIndexMax];

	C0HALSetStatusRegister(kSignaloidSoCStatusCalculating);
	C0HALSetLed(true);

	readInputVariables(inputVariables, kSensirionSFM3100InputVariableIndexMax);

	for (uint16_t iteration = 0; iteration < iterationCount; iteration++)
	{
		SensirionSFM3100_calculateOutput(
			inputVariables,
			outputVariables
		);
	}

	writeOutputVariables(outputVariables, kSensirionSFM3100OutputVariableIndexMax);

	C0HALSetStatusRegister(kSignaloidSoCStatusDone);
	C0HALSetLed(false);
}
#endif

#ifdef INCLUDE_SensirionSHT3xARP
void
handleSensirionSHT3xARP(uint16_t iterationCount)
{
	double  inputVariables[kSensirionSHT3xARPInputVariableIndexMax];
	double  outputVariables[kSensirionSHT3xARPOutputVariableIndexMax];

	C0HALSetStatusRegister(kSignaloidSoCStatusCalculating);
	C0HALSetLed(true);

	readInputVariables(inputVariables, kSensirionSHT3xARPInputVariableIndexMax);

	for (uint16_t iteration = 0; iteration < iterationCount; iteration++)
	{
		SensirionSHT3xARP_calculateOutput(
			kSensirionSHT3xARPOutputVariableIndexMax,
			inputVariables,
			outputVariables
		);
	}

	writeOutputVariables(outputVariables, kSensirionSHT3xARPOutputVariableIndexMax);

	C0HALSetStatusRegister(kSignaloidSoCStatusDone);
	C0HALSetLed(false);
}
#endif

#ifdef INCLUDE_SensirionSHT4xI
void
handleSensirionSHT4xI(uint16_t iterationCount)
{
	double  inputVariables[kSensirionSHT4xIInputVariableIndexMax];
	double  outputVariables[kSensirionSHT4xIOutputVariableIndexMax];

	C0HALSetStatusRegister(kSignaloidSoCStatusCalculating);
	C0HALSetLed(true);

	readInputVariables(inputVariables, kSensirionSHT4xIInputVariableIndexMax);

	for (uint16_t iteration = 0; iteration < iterationCount; iteration++)
	{
		SensirionSHT4xI_calculateOutput(
			kSensirionSHT4xIOutputVariableIndexMax,
			inputVariables,
			outputVariables
		);
	}

	writeOutputVariables(outputVariables, kSensirionSHT4xIOutputVariableIndexMax);

	C0HALSetStatusRegister(kSignaloidSoCStatusDone);
	C0HALSetLed(false);
}
#endif

#ifdef INCLUDE_TexasInstrumentsTMAG5253
void
handleTexasInstrumentsTMAG5253(uint16_t iterationCount)
{
	double  inputVariables[kTexasInstrumentsTMAG5253InputVariableIndexMax];
	double  outputVariables[kTexasInstrumentsTMAG5253OutputVariableIndexMax];

	C0HALSetStatusRegister(kSignaloidSoCStatusCalculating);
	C0HALSetLed(true);

	readInputVariables(inputVariables, kTexasInstrumentsTMAG5253InputVariableIndexMax);

	for (uint16_t iteration = 0; iteration < iterationCount; iteration++)
	{
		TexasInstrumentsTMAG5253_calculateOutput(
			inputVariables,
			outputVariables
		);
	}

	writeOutputVariables(outputVariables, kTexasInstrumentsTMAG5253OutputVariableIndexMax);

	C0HALSetStatusRegister(kSignaloidSoCStatusDone);
	C0HALSetLed(false);
}
#endif

#ifdef INCLUDE_TexasInstrumentsTMCS112x
void
handleTexasInstrumentsTMCS112x(uint16_t iterationCount)
{
	double  inputVariables[kTexasInstrumentsTMCS112xInputVariableIndexMax];
	double  outputVariables[kTexasInstrumentsTMCS112xOutputVariableIndexMax];

	C0HALSetStatusRegister(kSignaloidSoCStatusCalculating);
	C0HALSetLed(true);

	readInputVariables(inputVariables, kTexasInstrumentsTMCS112xInputVariableIndexMax);

	for (uint16_t iteration = 0; iteration < iterationCount; iteration++)
	{
		TexasInstrumentsTMCS112x_calculateOutput(
			inputVariables,
			outputVariables
		);
	}

	writeOutputVariables(outputVariables, kTexasInstrumentsTMCS112xOutputVariableIndexMax);

	C0HALSetStatusRegister(kSignaloidSoCStatusDone);
	C0HALSetLed(false);
}
#endif



/*
 * Application Logic
 */

void
handleCommand(uint32_t commandRegisterValue)
{
	/*
	 *	The command register is treated as read-only inside this function.
	 *	Snapshot a copy at the top of each invocation, then split into the
	 *	lower 16 bits (the actual conversion command) and the upper 16 bits
	 *	(the benchmark iteration count, biased by 1 so that 0 still means a
	 *	single iteration).
	 */
	const uint16_t  command         = (uint16_t) (commandRegisterValue & 0xFFFFu);
	const uint16_t  iterationCount  = (uint16_t) ((commandRegisterValue >> 16) & 0xFFFFu) + 1;

	if (command == kCalculateNoCommand)
	{
		return;
	}

	switch (command)
	{
#ifdef INCLUDE_FLIRAx5
		case kFLIRAx5Convert:
			handleFLIRAx5(iterationCount);
			break;
#endif

#ifdef INCLUDE_FlussoFLS110
		case kFlussoFLS110Convert:
			handleFlussoFLS110(iterationCount);
			break;
#endif

#ifdef INCLUDE_NXPMPX4100A
		case kNXPMPX4100AConvert:
			handleNXPMPX4100A(iterationCount);
			break;
#endif

#ifdef INCLUDE_NXPMPXx6250A
		case kNXPMPXx6250AConvert:
			handleNXPMPXx6250A(iterationCount);
			break;
#endif

#ifdef INCLUDE_SensirionSDP3x
		case kSensirionSDP3xConvert:
			handleSensirionSDP3x(iterationCount);
			break;
#endif

#ifdef INCLUDE_SensirionSDP8xx
		case kSensirionSDP8xxConvert:
			handleSensirionSDP8xx(iterationCount);
			break;
#endif

#ifdef INCLUDE_SensirionSFM3100
		case kSensirionSFM3100Convert:
			handleSensirionSFM3100(iterationCount);
			break;
#endif

#ifdef INCLUDE_SensirionSHT3xARP
		case kSensirionSHT3xARPConvert:
			handleSensirionSHT3xARP(iterationCount);
			break;
#endif

#ifdef INCLUDE_SensirionSHT4xI
		case kSensirionSHT4xIConvert:
			handleSensirionSHT4xI(iterationCount);
			break;
#endif

#ifdef INCLUDE_TexasInstrumentsTMAG5253
		case kTexasInstrumentsTMAG5253Convert:
			handleTexasInstrumentsTMAG5253(iterationCount);
			break;
#endif

#ifdef INCLUDE_TexasInstrumentsTMCS112x
		case kTexasInstrumentsTMCS112xConvert:
			handleTexasInstrumentsTMCS112x(iterationCount);
			break;
#endif


		default:
			C0HALSetStatusRegister(kSignaloidSoCStatusInvalidCommand);
			break;
	}
}

void
setup(void)
{
	C0HALSetLed(false);
	C0HALSetStatusRegister(kSignaloidSoCStatusWaitingForCommand);
}

void
loop(void)
{
	SignaloidSoCCommand command = waitForCommand();
	handleCommand(command);
	waitForIdle();
}

int
main(void)
{
	setup();
	while (1)
	{
		loop();
	}
}
