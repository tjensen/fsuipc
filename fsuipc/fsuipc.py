from types import TracebackType
from typing import List, Optional, Tuple, Type, Union

from fsuipc.prepared_data import PreparedData, DataSpecification
import pyuipc


class FSUIPC():
    """A connection to FSUIPC.

    Note that FSUIPC objects can be used as context managers so that the
    connection is automatically closed upon exiting the context. For
    example:

        with fsuipc.FSUIPC() as ipc:
            altitude = ipc.read([(0x570, "l")])
    """

    def __init__(self, version: int = pyuipc.SIM_ANY) -> None:
        """Opens a connection to FSUIPC.

        By default, connects to any available flight simulator.

        Arguments:
        version -- the type of flight simulator to connect to (e.g. fsuipc.SIM_FSX)
        """

        pyuipc.open(version)

    def close(self) -> None:
        """Closes the connection to FSUIPC."""

        pyuipc.close()

    def get_simulator_version(self) -> int:
        """Returns the actual version of the simulator, as one of the SIM_xxx constants"""

        return pyuipc.fs_version

    def get_fsuipc_version(self) -> str:
        """Returns the FSUIPC version"""

        bcd_version = int(f"{pyuipc.fsuipc_version >> 16:x}")
        major = bcd_version // 1000
        minor = bcd_version % 1000
        build = pyuipc.fsuipc_version % 65536
        return f"{major}.{minor:03}{chr(build + 96)}"

    def get_library_version(self) -> str:
        """Returns the FSUIPC client library version"""

        return f"{pyuipc.lib_version // 1000}.{pyuipc.lib_version % 1000:03}"

    def prepare_data(
            self, data_specification: DataSpecification, for_reading: bool
    ) -> PreparedData:
        """Prepare a data specification for reading or writing.

        Preparing a data specification can reduce the cost of converting sets
        of offsets and their types when repeatedly reading or writing the
        same sets of offsets.

        Returns a PreparedData object, which can be used to read and write the
        data.

        Arguments:
        data_specification -- a list of tuples of two items, as described below
        for_reading -- if True, prepare for reading and writing; False for writing, only

        Data Specification:
        The first item of each tuple should be an offset to read or write.

        The second item denotes the type of the value to be read from or
        written into the offset. It should be either a string or an integer.
        If it is a string, it denotes a type of a fixed size:

            - b: a 1-byte unsigned value, to be converted into a Python int
            - c: a 1-byte signed value, to be converted into a Python int
            - h: a 2-byte signed value, to be converted into a Python int
            - H: a 2-byte unsigned value, to be converted into a Python int
            - d: a 4-byte signed value, to be converted into a Python int
            - u: a 4-byte unsigned value, to be converted into a Python long
            - l: an 8-byte signed value, to be converted into a Python long
            - L: an 8-byte unsigned value, to be converted into a Python long
            - f: an 8-byte floating point value, to be converted into a Python double
            - F: a 4-byte floating point value, to be converted into a Python double

        If the second item is an integer, it denotes a string. If the value
        is positive, the string is of a fixed length with no zero terminator.
        If the value is negative, its absolute value is the maximal length of
        the string, but the string itself is zero terminated. Such data is
        returned as a Python bytes object when reading, and a bytes object is
        expected to be written.
        """

        return PreparedData(data_specification, for_reading)

    def read(self, unprepared_data: DataSpecification) -> List[Union[int, float, bytes]]:
        """Read data from FSUIPC using an unprepared data specification.

        Returns a list of the data items read.

        Arguments:
        unprepared_data - a list of tuples of two items, as described by prepare_data
        """

        return pyuipc.read(unprepared_data)

    def write(
            self, unprepared_data: List[Tuple[int, Union[int, str], Union[int, float, bytes]]]
    ) -> None:
        """Write data to FSUIPC using an unprepared data specification.

        Arguments:
        unprepared_data - a list of tuples of three items, as described below

        Data Specification:
        The first two items of each tuple are the offset and data type, as
        described by prepare_data. The third item of each tuple is the actual
        data to write at the given offset.
        """

        return pyuipc.write(unprepared_data)

    def __enter__(self) -> "FSUIPC":
        return self

    def __exit__(
            self, exc_type: Optional[Type[BaseException]], exc_value: Optional[BaseException],
            traceback: Optional[TracebackType]
    ) -> None:
        self.close()
