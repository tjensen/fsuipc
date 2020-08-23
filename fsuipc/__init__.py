"""fsuipc

This module provides a is simple Python client wrapper around the FSUIPC C
SDK, which allows third-party programs to interact with the "inner workings"
of popular flight simulator software.

Fsuipc is built on top of (and includes) pyuipc, by István Váradi
(ivaradi@varadiistvan.hu).

Basic Usage:

    from fsuipc import FSUIPC


    with FSUIPC() as fsuipc:
        prepared = fsuipc.prepare_data([
            (0x560, "l"),
            (0x568, "l"),
            (0x570, "l")
        ], True)

        while True:
            latitude, longitude, altitude = prepared.read()

            print(f"Latitude: {latitude}")
            print(f"Longitude: {longitude}")
            print(f"Altitude: {altitude}")

            input("Press ENTER to read again")
"""

from pyuipc import ERR_OK, ERR_OPEN, ERR_NOFS, ERR_REGMSG, ERR_ATOM, ERR_MAP, ERR_VIEW, ERR_VERSION
from pyuipc import ERR_WRONGFS, ERR_NOTOPEN, ERR_NODATA, ERR_TIMEOUT, ERR_SENDMSG, ERR_DATA
from pyuipc import ERR_RUNNING, ERR_SIZE
from pyuipc import SIM_ANY, SIM_FS98, SIM_FS2K, SIM_CFS2, SIM_CFS1, SIM_FLY, SIM_FS2K2, SIM_FS2K4
from pyuipc import SIM_FSX, SIM_ESP, SIM_P3D, SIM_FSX64, SIM_P3D64, SIM_FS2020
from pyuipc import FSUIPCException

from fsuipc.fsuipc import FSUIPC
from fsuipc.prepared_data import PreparedData

__all__ = [
    "ERR_OK", "ERR_OPEN", "ERR_NOFS", "ERR_REGMSG", "ERR_ATOM", "ERR_MAP", "ERR_VIEW",
    "ERR_VERSION", "ERR_WRONGFS", "ERR_NOTOPEN", "ERR_NODATA", "ERR_TIMEOUT", "ERR_SENDMSG",
    "ERR_DATA", "ERR_RUNNING", "ERR_SIZE",
    "SIM_ANY", "SIM_FS98", "SIM_FS2K", "SIM_CFS2", "SIM_CFS1", "SIM_FLY", "SIM_FS2K2", "SIM_FS2K4",
    "SIM_FSX", "SIM_ESP", "SIM_P3D", "SIM_FSX64", "SIM_P3D64", "SIM_FS2020",
    "FSUIPCException",
    "FSUIPC", "PreparedData"
]
