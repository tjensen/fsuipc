from typing import List, Tuple, Union, overload


ERR_OK: int
ERR_OPEN: int
ERR_NOFS: int
ERR_REGMSG: int
ERR_ATOM: int
ERR_MAP: int
ERR_VIEW: int
ERR_VERSION: int
ERR_WRONGFS: int
ERR_NOTOPEN: int
ERR_NODATA: int
ERR_TIMEOUT: int
ERR_SENDMSG: int
ERR_DATA: int
ERR_RUNNING: int
ERR_SIZE: int
SIM_ANY: int
SIM_FS98: int
SIM_FS2K: int
SIM_CFS2: int
SIM_CFS1: int
SIM_FLY: int
SIM_FS2K2: int
SIM_FS2K4: int
SIM_FSX: int
SIM_ESP: int
SIM_P3D: int
SIM_FSX64: int
SIM_P3D64: int


fs_version: int
fsuipc_version: int
lib_version: int


class FSUIPCException(Exception):
    def __init__(self, code: int) -> None:
        ...


class OpaqueData:
    ...


def open(version: int) -> None:
    ...


def close() -> None:
    ...


def prepare_data(
    data_specification: List[Tuple[int, Union[int, str]]], for_reading: bool
) -> OpaqueData:
    ...


def read(
    prepared_or_unprepared_data: Union[OpaqueData, List[Tuple[int, Union[int, str]]]]
) -> List[Union[int, float, bytes]]:
    ...


@overload
def write(data: List[Tuple[int, Union[int, str], Union[int, float, bytes]]]) -> None:
    ...


@overload
def write(prepared_data: OpaqueData, data: List[Union[int, float, bytes]]) -> None:
    ...
