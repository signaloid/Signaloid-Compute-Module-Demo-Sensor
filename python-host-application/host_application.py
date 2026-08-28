#!/usr/bin/env python -u
# PYTHON_ARGCOMPLETE_OK

#   Copyright (c) 2026, Signaloid.
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included in
#   all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.


import argparse
import signal
import time
from enum import IntEnum

import argcomplete
from app_helpers import (
    C0Status,
    SignaloidComputeModuleInterface,
    compute_module_args,
    init_compute_module,
    pack_floats,
    parse_output_buffer,
    parse_tolerance_value,
    print_output_values,
    sigint_handler,
)
from signaloid_utilities.c0microsd.interface import (
    C0microSDSignaloidSoCInterface,
)


class Commands(IntEnum):
    CalculateNoCommand = 0
    FLIRAx5 = 1
    FlussoFLS110 = 2
    NXPMPX4100A = 3
    NXPMPXx6250A = 4
    SensirionSDP3x = 5
    SensirionSDP8xx = 6
    SensirionSFM3100 = 7
    SensirionSHT3xARP = 8
    SensirionSHT4xI = 9
    TexasInstrumentsTMAG5253 = 10
    TexasInstrumentsTMCS112x = 11


class SensorBaseClass:
    command: Commands

    @classmethod
    def get_command_id(cls) -> int:
        return cls.command

    class InputVariableIndex(IntEnum):
        pass

    class OutputVariableIndex(IntEnum):
        pass

    default_input_variable_ranges: dict[
        "InputVariableIndex", tuple[float, float]
    ]


class FLIRAx5(SensorBaseClass):
    command = Commands.FLIRAx5

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        Counts = 0

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedSensorOutput = 0

    default_input_variable_ranges = {
        InputVariableIndex.Counts: (30000, 30100),
    }


class FlussoFLS110(SensorBaseClass):
    command = Commands.FlussoFLS110

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        Hxfer = 0
        Tflow = 1
        T0 = 2
        Pflow = 3
        P0 = 4

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedMassFlowOutput = 0
        CalibratedDifferentialPressureOutput = 1

    default_input_variable_ranges = {
        InputVariableIndex.Hxfer: (0.010, 0.050),
        InputVariableIndex.Tflow: (293.0, 294.0),
        InputVariableIndex.T0: (273.0, 273.5),
        InputVariableIndex.Pflow: (420000.00, 425000.00),
        InputVariableIndex.P0: (400000.00, 405000.00),
    }


class NXPMPX4100A(SensorBaseClass):
    command = Commands.NXPMPX4100A

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        VsensorADC = 0
        VsupplyADC = 1

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedSensorOutput = 0

    default_input_variable_ranges = {
        InputVariableIndex.VsensorADC: (2.3, 2.7),
        InputVariableIndex.VsupplyADC: (4.8, 5.4),
    }


class NXPMPXx6250A(SensorBaseClass):
    command = Commands.NXPMPXx6250A

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        VsensorADC = 0
        VsupplyADC = 1

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedSensorOutput = 0

    default_input_variable_ranges = {
        InputVariableIndex.VsensorADC: (2.3, 2.7),
        InputVariableIndex.VsupplyADC: (4.8, 5.4),
    }


class SensirionSDP3x(SensorBaseClass):
    command = Commands.SensirionSDP3x

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        Aout = 0
        Vdd = 1

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedSensorOutputSDP36Linear500Pa = 0
        CalibratedSensorOutputSDP37Linear125Pa = 1
        CalibratedSensorOutputSDP36Sqrt500Pa = 2
        CalibratedSensorOutputSDP37Sqrt125Pa = 3

    default_input_variable_ranges = {
        InputVariableIndex.Aout: (1.3, 1.7),
        InputVariableIndex.Vdd: (3.3, 3.9),
    }


class SensirionSDP8xx(SensorBaseClass):
    command = Commands.SensirionSDP8xx

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        Aout = 0
        Vdd = 1

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedSensorOutputSDP8x6Linear500Pa = 0
        CalibratedSensorOutputSDP8x6Linear125Pa = 1
        CalibratedSensorOutputSDP8x6Sqrt500Pa = 2
        CalibratedSensorOutputSDP8x6Sqrt125Pa = 3

    default_input_variable_ranges = {
        InputVariableIndex.Aout: (1.3, 1.7),
        InputVariableIndex.Vdd: (3.5, 3.9),
    }


class SensirionSFM3100(SensorBaseClass):
    command = Commands.SensirionSFM3100

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        Uv = 0

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedFlow = 0

    default_input_variable_ranges = {
        InputVariableIndex.Uv: (0.7, 0.8),
    }


class SensirionSHT3xARP(SensorBaseClass):
    command = Commands.SensirionSHT3xARP

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        Vrh = 0
        Vt = 1
        Vsupply = 2

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedRelativeHumidity = 0
        CalibratedTemperatureCelcius = 1
        CalibratedTemperatureFahrenheit = 2

    default_input_variable_ranges = {
        InputVariableIndex.Vt: (2.3, 2.7),
        InputVariableIndex.Vrh: (2.3, 2.7),
        InputVariableIndex.Vsupply: (4.8, 5.4),
    }


class SensirionSHT4xI(SensorBaseClass):
    command = Commands.SensirionSHT4xI

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        Vrh = 0
        Vt = 1
        Vsupply = 2

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedRelativeHumidity = 0
        CalibratedTemperatureCelcius = 1
        CalibratedTemperatureFahrenheit = 2

    default_input_variable_ranges = {
        InputVariableIndex.Vt: (2.3, 2.7),
        InputVariableIndex.Vrh: (2.3, 2.7),
        InputVariableIndex.Vsupply: (4.8, 5.4),
    }


class TexasInstrumentsTMAG5253(SensorBaseClass):
    command = Commands.TexasInstrumentsTMAG5253

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        Vout = 0
        Vcc = 1

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedMagneticFluxDensity = 0

    default_input_variable_ranges = {
        InputVariableIndex.Vout: (2.6, 2.8),
        InputVariableIndex.Vcc: (3.2, 3.4),
    }


class TexasInstrumentsTMCS112x(SensorBaseClass):
    command = Commands.TexasInstrumentsTMCS112x

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        Vout = 0
        Vref = 1

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedCurrent = 0

    default_input_variable_ranges = {
        InputVariableIndex.Vout: (3.2, 3.4),
        InputVariableIndex.Vref: (2.4, 2.6),
    }




sensor_classes: list[type[SensorBaseClass]] = [
    FLIRAx5,
    FlussoFLS110,
    NXPMPX4100A,
    NXPMPXx6250A,
    SensirionSDP3x,
    SensirionSDP8xx,
    SensirionSFM3100,
    SensirionSHT3xARP,
    SensirionSHT4xI,
    TexasInstrumentsTMAG5253,
    TexasInstrumentsTMCS112x,
]

command_to_sensor_class: dict[str, type[SensorBaseClass]] = {
    sensor_class.__name__: sensor_class for sensor_class in sensor_classes
}


def encode_command_register(command_id: int, iterations: int) -> int:
    """
    Build the 32-bit value to write into the device's command register.

    The lower 16 bits hold the conversion command id; the upper 16 bits
    hold the benchmark iteration count, biased by one so that an upper
    half of 0 still triggers a single kernel invocation on the device.
    """
    if not 1 <= iterations <= 0x10000:
        raise ValueError(
            f"iterations must be between 1 and 65536, got {iterations}"
        )
    if not 0 <= command_id <= 0xFFFF:
        raise ValueError(f"command_id must fit in 16 bits, got {command_id}")

    return ((iterations - 1) & 0xFFFF) << 16 | (command_id & 0xFFFF)


def run_and_measure_device(
    compute_module: SignaloidComputeModuleInterface,
    command_value: int,
    timeout: float = 5.0,
) -> float:
    """
    Issue ``command_value`` to the device and return the elapsed time, in
    seconds, between sending the command and observing the Done status.

    After the measurement, the device is returned to the WaitingForCommand
    state, mirroring the cleanup that ``calculate_command`` performs.
    """
    if isinstance(compute_module, C0microSDSignaloidSoCInterface):
        send_command = compute_module.send_signaloid_soc_command
        get_status = compute_module.get_signaloid_soc_status
    else:
        send_command = compute_module.set_command
        get_status = compute_module.get_status

    start_time = time.perf_counter()
    send_command(command_value)
    while True:
        status = get_status()
        if status == C0Status.Done:
            break
        if status == C0Status.InvalidCommand:
            raise RuntimeError("Device returned 'Invalid command'.")
        if (
            status == C0Status.WaitingForCommand
            and time.perf_counter() - start_time > timeout
        ):
            raise TimeoutError("Device blocked.")
    end_time = time.perf_counter()

    while get_status() != C0Status.WaitingForCommand:
        send_command(Commands.CalculateNoCommand)

    return end_time - start_time


def parse_arguments(
    explicit_args: list[str] | None = None,
):
    parser = argparse.ArgumentParser(
        description="Host application for the Signaloid C0 compute modules "
        "sensor conversion routine demo"
    )

    compute_module_args(parser=parser)

    subparsers = parser.add_subparsers(
        dest="command",
        help="Commands",
    )

    for sensor_class in sensor_classes:
        sensor_parser = subparsers.add_parser(sensor_class.__name__)
        for input_variable in sensor_class.InputVariableIndex:
            sensor_parser.add_argument(
                f"argument_{input_variable.name}",
                type=str,
                help=f"Value with uncertainty for {input_variable.name} in "
                "the format X.Y(Z)",
            )

    parser.add_argument(
        "--skip-printing-results",
        action="store_true",
        help="Skip printing the resulting Ux-Strings. "
        "Useful when benchmarking.",
        default=False,
        required=False,
    )

    parser.add_argument(
        "--skip-plotting-results",
        action="store_true",
        help="Skip plotting the resulting Ux-Strings. "
        "Useful when benchmarking.",
        default=False,
        required=False,
    )

    parser.add_argument(
        "--benchmark",
        default=False,
        action="store_true",
        help="Enable benchmarking mode. Measures and reports the "
        "per-iteration device execution time from command issue until "
        "status=Done.",
    )

    parser.add_argument(
        "--iterations",
        type=int,
        default=20,
        help="Number of times the conversion kernel is repeated on the device "
        "for a single command. The value is encoded as (iterations - 1) "
        "in the upper 16 bits of the command register. Default: 20",
    )

    argcomplete.autocomplete(parser)
    args = parser.parse_args(explicit_args)

    if args.iterations < 1 or args.iterations > 0x10000:
        parser.error("--iterations must be between 1 and 65536 (inclusive).")

    return args


def main(explicit_args: list[str] | None = None):
    signal.signal(signal.SIGINT, sigint_handler)

    args = parse_arguments(explicit_args)

    compute_module = init_compute_module(
        device_path=args.device_path,
        variant=args.variant,
        reset_on_launch=args.reset_on_launch,
    )

    sensor_class: type[SensorBaseClass] = command_to_sensor_class[args.command]

    input_data: list[float] = []
    for input_variable in sensor_class.InputVariableIndex:
        arg_min, arg_max = parse_tolerance_value(
            getattr(args, f"argument_{input_variable.name}")
        )
        input_data.extend([arg_min, arg_max])

    input_buffer = pack_floats(
        floats=input_data,
        size=compute_module.INPUT_BUFFER_SIZE_BYTES,
    )

    command_value = encode_command_register(
        command_id=sensor_class.get_command_id(),
        iterations=args.iterations,
    )

    compute_module.write_input_buffer(input_buffer)
    device_time = run_and_measure_device(compute_module, command_value)
    result_buffer = compute_module.read_output_buffer()

    if args.benchmark:
        per_iteration_time = device_time / args.iterations
        print(
            f"Benchmark: {args.iterations} iteration(s), "
            f"total device time {device_time * 1000:.3f} ms, "
            f"per iteration {per_iteration_time * 1000:.4f} ms"
        )
    else:
        print(f"Process time: {device_time*1000:.3f} ms")

    # Plot results
    value_labels = [
        output_variable.name
        for output_variable in sensor_class.OutputVariableIndex
    ]

    dists = parse_output_buffer(
        buffer=result_buffer,
        expected_output_count=len(sensor_class.OutputVariableIndex),
    )

    print_output_values(
        values=dists,
        value_labels=value_labels,
        skip_printing=args.skip_printing_results,
        skip_plotting=args.skip_plotting_results,
    )


if __name__ == "__main__":
    main()
