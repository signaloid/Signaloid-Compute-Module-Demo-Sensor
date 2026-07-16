#   Copyright (c) 2025, Signaloid.
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
from enum import Enum
import signal
import struct
import sys
import time
import types
import time

import matplotlib.pyplot as plt
from signaloid.distributional.distributional import DistributionalValue
from signaloid.distributional_information_plotting.plot_histogram_dirac_deltas import \
    PlotData
from signaloid.distributional_information_plotting.plot_wrapper import plot
from signaloid_utilities.c0microsd.interface import C0microSDSignaloidSoCInterface
from signaloid_utilities.c0sd.interface import \
    C0SDBaseInterface, \
    C0microSDPlusInterface, \
    C0SDInterface


# Definitions -----------------------------------------------------------------
INTERFACE_BY_VARIANT: dict[
    str,
    type[C0microSDSignaloidSoCInterface | C0SDBaseInterface]
] = {
    "C0-microSD":    C0microSDSignaloidSoCInterface,
    "C0-microSD+": C0microSDPlusInterface,
    "C0-SD":         C0SDInterface,
}


class C0Status(Enum):
    WaitingForCommand = 0
    Calculating = 1
    Done = 2
    InvalidCommand = 3


class Commands(Enum):
    CalculateNoCommand = 0,
    FLIRAx5 = 1,
    FlussoFLS110 = 2,
    NXPMPX4100A = 3,
    NXPMPXx6250A = 4,
    SensirionSDP3x = 5,
    SensirionSDP8xx = 6,
    SensirionSFM3100 = 7,
    SensirionSHT3xARP = 8,
    SensirionSHT4xI = 9,
    TexasInstrumentsTMAG5253 = 10,
    TexasInstrumentsTMCS112x = 11,


class SensorBaseClass:
    command: Commands

    @classmethod
    def get_command_id(cls) -> int:
        return cls.command.value[0]

    class InputVariableIndex(Enum):
        pass

    class OutputVariableIndex(Enum):
        pass

    default_input_variable_ranges: dict[InputVariableIndex, tuple[float, float]]


class FLIRAx5(SensorBaseClass):
    command = Commands.FLIRAx5

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        Counts = 0,

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedSensorOutput = 0,

    default_input_variable_ranges = {
        InputVariableIndex.Counts: (30000, 30100),
    }


class FlussoFLS110(SensorBaseClass):
    command = Commands.FlussoFLS110

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        Hxfer = 0,
        Tflow = 1,
        T0 = 2,
        Pflow = 3,
        P0 = 4,

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedMassFlowOutput = 0,
        CalibratedDifferentialPressureOutput = 1,

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
        VsensorADC = 0,
        VsupplyADC = 1,

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedSensorOutput = 0,

    default_input_variable_ranges = {
        InputVariableIndex.VsensorADC: (2.3, 2.7),
        InputVariableIndex.VsupplyADC: (4.8, 5.4),
    }


class NXPMPXx6250A(SensorBaseClass):
    command = Commands.NXPMPXx6250A

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        VsensorADC = 0,
        VsupplyADC = 1,

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedSensorOutput = 0,

    default_input_variable_ranges = {
        InputVariableIndex.VsensorADC: (2.3, 2.7),
        InputVariableIndex.VsupplyADC: (4.8, 5.4),
    }


class SensirionSDP3x(SensorBaseClass):
    command = Commands.SensirionSDP3x

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        Aout = 0,
        Vdd = 1,

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedSensorOutputSDP36Linear500Pa = 0,
        CalibratedSensorOutputSDP37Linear125Pa = 1,
        CalibratedSensorOutputSDP36Sqrt500Pa = 2,
        CalibratedSensorOutputSDP37Sqrt125Pa = 3,

    default_input_variable_ranges = {
        InputVariableIndex.Aout: (1.3, 1.7),
        InputVariableIndex.Vdd: (3.3, 3.9),
    }


class SensirionSDP8xx(SensorBaseClass):
    command = Commands.SensirionSDP8xx

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        Aout = 0,
        Vdd = 1,

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedSensorOutputSDP8x6Linear500Pa = 0,
        CalibratedSensorOutputSDP8x6Linear125Pa = 1,
        CalibratedSensorOutputSDP8x6Sqrt500Pa = 2,
        CalibratedSensorOutputSDP8x6Sqrt125Pa = 3,

    default_input_variable_ranges = {
        InputVariableIndex.Aout: (1.3, 1.7),
        InputVariableIndex.Vdd: (3.5, 3.9),
    }


class SensirionSFM3100(SensorBaseClass):
    command = Commands.SensirionSFM3100

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        Uv = 0,

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedFlow = 0,

    default_input_variable_ranges = {
        InputVariableIndex.Uv: (0.7, 0.8),
    }


class SensirionSHT3xARP(SensorBaseClass):
    command = Commands.SensirionSHT3xARP

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        Vrh = 0,
        Vt = 1,
        Vsupply = 2,

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedRelativeHumidity = 0,
        CalibratedTemperatureCelcius = 1,
        CalibratedTemperatureFahrenheit = 2,

    default_input_variable_ranges = {
            InputVariableIndex.Vt: (2.3, 2.7),
            InputVariableIndex.Vrh: (2.3, 2.7),
            InputVariableIndex.Vsupply: (4.8, 5.4),
    }


class SensirionSHT4xI(SensorBaseClass):
    command = Commands.SensirionSHT4xI

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        Vrh = 0,
        Vt = 1,
        Vsupply = 2,

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedRelativeHumidity = 0,
        CalibratedTemperatureCelcius = 1,
        CalibratedTemperatureFahrenheit = 2,

    default_input_variable_ranges = {
        InputVariableIndex.Vt: (2.3, 2.7),
        InputVariableIndex.Vrh: (2.3, 2.7),
        InputVariableIndex.Vsupply: (4.8, 5.4),
    }




class TexasInstrumentsTMAG5253(SensorBaseClass):
    command = Commands.TexasInstrumentsTMAG5253

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        Vout = 0,
        Vcc = 1,

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedMagneticFluxDensity = 0,

    default_input_variable_ranges = {
        InputVariableIndex.Vout: (2.6, 2.8),
        InputVariableIndex.Vcc: (3.2, 3.4),
    }




class TexasInstrumentsTMCS112x(SensorBaseClass):
    command = Commands.TexasInstrumentsTMCS112x

    class InputVariableIndex(SensorBaseClass.InputVariableIndex):
        Vout = 0,
        Vref = 1,

    class OutputVariableIndex(SensorBaseClass.OutputVariableIndex):
        CalibratedCurrent = 0,

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
    sensor_class.__name__: sensor_class
    for sensor_class in sensor_classes
}
# -----------------------------------------------------------------------------


def sigint_handler(signal: int, frame: types.FrameType | None):
    plt.close()
    sys.exit(0)


def pack_floats(
    floats: list[float],
    size: int,
    double_precision: bool = True
) -> bytes:
    """
    Pack a list of floats to a zero-padded bytes buffer of length size

    :param floats: List of floats to be packed
    :param size: Size of target buffer
    :param double_precision: If the floats are in double precision representation

    :return: The padded bytes buffer
    """
    format_str = f"{len(floats)}{'d' if double_precision else 'f'}"
    buffer = struct.pack(format_str, *floats)

    # Pad the buffer with zeros
    if len(buffer) < size:
        buffer += bytes(size - len(buffer))
    elif len(buffer) > size:
        raise ValueError(
            f"Buffer length exceeds {size} bytes after packing floats."
        )
    return buffer


def unpack_floats(
    byte_buffer: bytes,
    count: int,
    double_precision: bool = True
) -> list[int]:
    """
    This function unpacks 'count' number of single or double precision
    floating-point numbers from the given byte buffer. It checks if the
    buffer has enough data to unpack.

    :param byte_buffer: A bytes object containing the binary data.
    :param count: The number of floats to unpack.
    :param double_precision: If the floats are in double precision representation

    :return A list of unpacked floating-point values.
    """

    # 4 bytes for each single-precision float
    # 8 bytes for each double-precision float
    float_size = 8 if double_precision else 4

    # Check if the buffer has enough bytes to unpack the requested
    # number of floats
    expected_size = float_size * count
    if len(byte_buffer) < expected_size:
        raise ValueError(
            f"Buffer too small: expected at least {expected_size} bytes, "
            f"got {len(byte_buffer)} bytes."
        )

    # Unpack the 'count' number of floats
    format_string = f"{count}{'d' if double_precision else 'f'}"
    floats = struct.unpack(format_string, byte_buffer[:expected_size])

    return list(floats)


def pack_unsigned_integers(
    uint: list[int],
    size: int
) -> bytes:
    """
    Pack a list of unsigned integers to a zero-padded bytes
    buffer of length size

    :param uint: List of unsigned integers to be packed
    :param size: Size of target buffer

    :return: The padded bytes buffer
    """
    buffer = struct.pack(f"<{len(uint)}I", *uint)

    # Pad the buffer with zeros
    if len(buffer) < size:
        buffer += bytes(size - len(buffer))
    elif len(buffer) > size:
        raise ValueError(
            f"Buffer length exceeds {size} bytes after packing unsigned integers."
        )
    return buffer


def parse_tolerance_value(
    value_with_uncertainty: str
) -> tuple[float, float]:
    # Split the value and the uncertainty part
    if '(' not in value_with_uncertainty or ')' not in value_with_uncertainty:
        raise ValueError(
            "Invalid format. Please provide value in the format 'X.Y(Z)'")

    # Extract the main value and the uncertainty
    value_str, uncertainty_str = value_with_uncertainty.split('(')
    uncertainty_str = uncertainty_str.strip(')')

    # Find smallest order
    if "." in value_str:
        # Split at the decimal point
        _, decimal_part = value_str.split(".")
        order = -(len(decimal_part))
    else:
        order = 0

    # Convert to appropriate types
    value = float(value_str)
    uncertainty = int(uncertainty_str)

    # Calculate minimum and maximum values
    min_value = value - (uncertainty * (10 ** order))
    max_value = value + (uncertainty * (10 ** order))

    return min_value, max_value


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
        raise ValueError(
            f"command_id must fit in 16 bits, got {command_id}"
        )

    return ((iterations - 1) & 0xFFFF) << 16 | (command_id & 0xFFFF)


def run_and_measure_device(
    compute_module: C0microSDSignaloidSoCInterface | C0SDBaseInterface,
    command_value: int,
    timeout: float = 1.0,
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

    done_status = C0Status.Done.value
    invalid_status = C0Status.InvalidCommand.value
    wait_status = C0Status.WaitingForCommand.value
    idle_command = Commands.CalculateNoCommand.value[0]

    start_time = time.perf_counter()
    send_command(command_value)
    while True:
        status = get_status()
        if status == done_status:
            break
        if status == invalid_status:
            raise RuntimeError("Device returned 'Invalid command'.")
        if status == wait_status and time.perf_counter() - start_time > timeout:
            raise TimeoutError("Device blocked.")
    end_time = time.perf_counter()

    while get_status() != wait_status:
        send_command(idle_command)

    return end_time - start_time


def parse_arguments(explicit_args: list[str] | None = None):
    # Create the top-level parser
    parser = argparse.ArgumentParser(
        description='Host application for C0-microSD+ \
            calculator application'
    )

    parser.add_argument(
        '--quiet',
        action="store_true",
        help='Use for benchmarking the C0-microSD. It suppresses all prints, \
            and does not plot results.',
        default=False,
        required=False
    )

    parser.add_argument(
        'device_path',
        type=str,
        help='Path of C0 compute module device (e.g., /dev/disk4)',
    )

    parser.add_argument(
        "--variant",
        choices=INTERFACE_BY_VARIANT.keys(),
        default="C0-microSD+",
        help="Hardware variant (default: C0-microSD+)"
    )

    subparsers = parser.add_subparsers(dest='command', help='Commands', required=True)

    for sensor_class in sensor_classes:
        sensor_parser = subparsers.add_parser(sensor_class.__name__)
        for input_variable in sensor_class.InputVariableIndex:
            sensor_parser.add_argument(
                f"argument_{input_variable.name}",
                type=str,
                help=f"Value with uncertainty for {input_variable.name} in the format X.Y(Z)",
            )

    parser.add_argument(
        "--benchmark",
        default=False,
        action="store_true",
        help="Enable benchmarking mode. Measures and reports the per-iteration "
             "device execution time from command issue until status=Done."
    )

    parser.add_argument(
        "--iterations",
        type=int,
        default=1,
        help="Number of times the conversion kernel is repeated on the device "
             "for a single command. The value is encoded as (iterations - 1) "
             "in the upper 16 bits of the command register. Default: 1."
    )

    # Parse the arguments
    args = parser.parse_args(explicit_args)

    if args.iterations < 1 or args.iterations > 0x10000:
        parser.error(
            "--iterations must be between 1 and 65536 (inclusive)."
        )

    return args


def main(explicit_args: list[str] | None = None):
    double_precision = False
    args = parse_arguments(explicit_args)

    interface = INTERFACE_BY_VARIANT.get(args.variant)
    if interface is None:
        return

    # Register the signal handler for SIGINT
    signal.signal(signal.SIGINT, sigint_handler)

    compute_module = interface(args.device_path)

    if isinstance(compute_module, C0SDBaseInterface):
        print("Resetting Signaloid SoC core")
        compute_module.reset_core(0.5)

    try:
        sensor_class: type[SensorBaseClass] = command_to_sensor_class[args.command]
        input_data: list[float] = []
        for input_variable in sensor_class.InputVariableIndex:
            arg_min, arg_max = parse_tolerance_value(
                getattr(args, f"argument_{input_variable.name}")
            )
            input_data.extend([arg_min, arg_max])

        # print(f"Input data: {input_data}")

        input_buffer = pack_floats(
            input_data,
            compute_module.INPUT_BUFFER_SIZE_BYTES,
            double_precision=double_precision
        )

        command_value = encode_command_register(
            command_id=sensor_class.get_command_id(),
            iterations=args.iterations,
        )

        compute_module.write_input_buffer(input_buffer)

        if args.benchmark:
            device_time = run_and_measure_device(
                compute_module, command_value
            )
            per_iteration_time = device_time / args.iterations
            print(
                f"Benchmark: {args.iterations} iteration(s), "
                f"total device time {device_time * 1000:.3f} ms, "
                f"per iteration {per_iteration_time * 1000:.4f} ms"
            )
            result_buffer = compute_module.read_output_buffer()
        else:
            startTime = time.perf_counter()
            if isinstance(compute_module, C0microSDSignaloidSoCInterface):
                compute_module.calculate_command(
                    command_value,
                    poll_sleep_time=0.0001,
                    skip_MISO_read=True,
                    verbose=True
                )
            else:
                compute_module.calculate_command(
                    command_value,
                    poll_sleep_time=0.0001,
                    skip_MMIO_buffer_read=True,
                    verbose=False
                )
            result_buffer = compute_module.read_output_buffer()
            endTime = time.perf_counter()
            iterationTime = endTime - startTime
            print(f"Process time: {iterationTime*1000:.2f} ms")

        if isinstance(result_buffer, bytes) is False:
            raise RuntimeError("No result buffer received.")

        # Plot results
        print("Plotting results. Press Ctrl+C to exit.")
        for i, output_variable in enumerate(sensor_class.OutputVariableIndex):
            # Interpret and remove the first 4 bytes as an unsigned integer
            returned_bytes = struct.unpack("I", result_buffer[:4])[0]

            # Keep only needed bytes in buffer
            result_buffer = result_buffer[4:]
            result_buffer_dist = result_buffer[:returned_bytes]

            # Align to 4-byte boundary for next iteration
            result_buffer = result_buffer[returned_bytes + ((4 - (returned_bytes % 4)) % 4):]

            distribution = DistributionalValue.parse(
                dist=result_buffer_dist,
                double_precision=double_precision
            )

            if distribution is None:
                raise RuntimeError(
                    "Error parsing distribution from result buffer."
                )

            print(
                f"[{i+1}/{len(sensor_class.OutputVariableIndex)}] "
                f"{output_variable.name}: {distribution}"
            )

            if not args.benchmark:
                plot(
                    plot_data=PlotData(
                        dist=distribution,
                        plotting_resolution=32,
                    ),
                    matplotlib_rc_params_override={
                        "figure.facecolor": "FFFFFF",
                        "axes.facecolor": "FFFFFF"
                    },
                    x_label="Distribution Support",
                    verbose=False,
                )
    except Exception as e:
        print(
            f"An error occurred while calculating: \n{e} \nAborting.",
            file=sys.stderr
        )
    finally:
        if isinstance(compute_module, C0SDBaseInterface):
            compute_module.apply_configure_action("core-stop")


if __name__ == "__main__":
    main()
