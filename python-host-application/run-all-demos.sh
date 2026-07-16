#!/usr/bin/env bash
#
# Runs every host_application.py example listed in the top-level README.
#
# The device path, hardware variant, and benchmark iteration count are
# configurable via environment variables (or by editing the defaults
# below):
#
#     DEVICE_PATH=/dev/disk4 VARIANT=C0-microSD+ ITERATIONS=100 ./run-all-demos.sh
#

set -euo pipefail

DEVICE_PATH="${DEVICE_PATH:-/dev/disk4}"
VARIANT="${VARIANT:-C0-microSD+}"
ITERATIONS="${ITERATIONS:-1}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${SCRIPT_DIR}"

run() {
    echo "+ sudo python -u ./host_application.py ${DEVICE_PATH} --benchmark --iterations ${ITERATIONS} --variant ${VARIANT} $*"
    sudo python -u ./host_application.py "${DEVICE_PATH}" --benchmark --iterations "${ITERATIONS}" --variant "${VARIANT}" "$@"
}

run FLIRAx5 "30050(50)"
run FlussoFLS110 "0.03(2)" "293.5(5)" "273.25(25)" "422500(2500)" "402500(2500)"
run NXPMPX4100A "2.5(2)" "5.1(3)"
run NXPMPXx6250A "2.5(2)" "5.1(3)"
run SensirionSDP3x "1.5(2)" "3.6(3)"
run SensirionSDP8xx "1.5(2)" "3.6(3)"
run SensirionSFM3100 "0.75(5)"
run SensirionSHT3xARP "2.5(2)" "2.5(2)" "5.1(3)"
run SensirionSHT4xI "2.5(2)" "2.5(2)" "5.1(3)"
run TexasInstrumentsTMAG5253 "2.7(1)" "3.3(1)"
run TexasInstrumentsTMCS112x "3.3(1)" "2.5(1)"
