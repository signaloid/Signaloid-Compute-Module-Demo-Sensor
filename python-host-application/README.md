# Python host application

The host application drives a Signaloid compute module: it packs input
distributions, issues a conversion command, reads the output distributions back,
and plots them. See the top-level [README](../README.md) for the full workflow
and prerequisites.

## Installation

The easiest path is `make venv` from the repository root, which creates a
virtual environment and installs the dependencies for you.

To set it up manually:

```sh
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

## Usage

```sh
sudo python3 host_application.py --device-path <device-path> [options] <SensorName> <inputs...>
```

`sudo` is required because the application communicates with the module through
raw block reads and writes.

### Positional arguments

| Argument      | Description                                                                                                   |
| ------------- | ------------------------------------------------------------------------------------------------------------- |
| `device-path` | Path to the compute module block device, for example `/dev/disk4`.                                            |
| `SensorName`  | The conversion routine to run, for example `SensirionSHT3xARP`. See the sensor table in the top-level README. |
| `inputs`      | One value per input variable, in the `X.Y(Z)` format.                                                         |

### Options

| Option                                       | Description                                                                                                            |
| -------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------- |
| `--device-path                               | The Signaloid compute module device path                                                                               |
| `--variant {C0-microSD, C0-microSD+, C0-SD}` | Hardware variant. Default: `C0-microSD+`.                                                                              |
| `--benchmark`                                | Measure and report per-iteration device execution time from command issue until status is `Done`. Suppresses plotting. |
| `--iterations N`                             | Repeat the conversion kernel `N` times on the device per command (1 to 65536). Default: 1.                             |
| `--skip-printing-results`                    | Suppress printing.                                                                                                     |
| `--skip-plotting-results`                    | Suppress plotting.                                                                                                     |

### Input value format

Each input is a value with an uncertainty in the last significant digit, written
as `X.Y(Z)`, which becomes a uniform distribution. For example, `2.5(2)` is the
uniform distribution over `[2.3, 2.7]`. Pass one input per input variable, in
the order listed for that sensor.

## Example

```sh
sudo python3 host_application.py --device-path /dev/disk4 --variant C0-microSD+ SensirionSHT3xARP "2.5(2)" "2.5(2)" "5.1(3)"
```

This runs the SHT3x-ARP conversion with uniform input distributions for Vrh, Vt,
and Vsupply, then plots the calibrated relative humidity and temperature
distributions.

## Benchmarking

Use `--benchmark` with `--iterations` to measure device execution time. The
reported per-iteration time is the total device time divided by the iteration
count:

```sh
sudo python3 host_application.py --device-path /dev/disk4 --benchmark --iterations 100 SensirionSHT3xARP "2.5(2)" "2.5(2)" "5.1(3)"
```

The [run-all-demos.sh](run-all-demos.sh) script runs every sensor in benchmark
mode. The device path, variant, and iteration count are configurable through
environment variables:

```sh
DEVICE_PATH=/dev/disk4 VARIANT=C0-microSD+ ITERATIONS=100 ./run-all-demos.sh
```

## Adding a sensor

Each sensor is a subclass of `SensorBaseClass` in
[host_application.py](host_application.py). To add one, define its `command` id,
its `InputVariableIndex` and `OutputVariableIndex` enums, and its
`default_input_variable_ranges`, then add the class to the `sensor_classes`
list. The command id must match the one in the firmware
[main.c](../signaloid-soc-application/main.c).
