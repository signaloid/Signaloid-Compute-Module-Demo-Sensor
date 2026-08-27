# Signaloid SoC application

This directory holds the firmware that runs on the Signaloid SoC inside the
compute modules. It receives sensor readings as input distributions, runs the
selected conversion routine with uncertainty tracking, and returns the
calibrated output as a distribution.

The firmware is built in the Signaloid Cloud Developer Platform. Use the targets
in the top-level [Makefile](../Makefile) to build, download, and flash it.

## Files

| File                                       | Purpose                                                                                                                               |
| ------------------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------- |
| [main.c](main.c)                           | Entry point. Polls the command register, dispatches to the selected conversion routine, and packs the results into the output buffer. |
| [config.mk](config.mk)                     | Build configuration. Selects the target device sources and which conversion routines to include.                                      |
| [conversionRoutines/](conversionRoutines/) | Per-sensor kernels. Each directory holds a `kernel.c` and `kernel.h`.                                                                 |

## Control flow

`main` runs `setup` once, then loops on `handleCommand`:

1. The command register value is split into a command id (lower 16 bits) and a
   benchmark iteration count (upper 16 bits, biased by one).
2. `readInputVariables` reconstructs each input as a uniform distribution from
   the low and high bounds in the input buffer.
3. The `switch` on the command id calls the matching conversion routine,
   repeated for the requested iteration count.
4. `writeOutputVariables` serializes each output distribution into the output
   buffer with a 4-byte length prefix, following a Ux-Binary.
5. The status register is set to `Done`, then back to `WaitingForCommand` once
   the host clears the command.

## Configuration

`config.mk` sets the sources for the selected `DEVICE_TYPE` and the Signaloid
Compute Module Utilities path, then includes the conversion routine per enabled
`INCLUDE_<Sensor>` flag. Set a flag to `0` to leave that routine out of the
build.

Add your own compiler flags through the `BUILD_FLAGS` variable, and add your
sources and include paths on the `SOURCES` and `INC` variables respectively of
`config.mk`.

## Adding a conversion routine

1. Create `conversionRoutines/<Name>/kernel.c` and `kernel.h`, following an
   existing routine.
2. Add an `INCLUDE_<Name>` block to `config.mk` that appends the kernel source
   and defines `-DINCLUDE_<Name>`.
3. In `main.c`, add the `#include` guard, a command id in the
   `SignaloidSoCCommand` enum, and a `case` in `handleCommand`.
4. Register a matching sensor class in the host application. See
   [python-host-application/README.md](../python-host-application/README.md).
