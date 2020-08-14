from typing import List, Tuple, Union
import pyuipc


DataSpecification = List[Tuple[int, Union[int, str]]]


class PreparedData():
    """Represents a data specification for reading and writing data.

    Use this class when repeatedly reading or writing the same set of offsets
    to reduce the cost of converting the sets of offsets and their types.

    Instances of PreparedData should be created using the FSUIPC class's
    prepare_data method.
    """

    def __init__(self, data_specification: DataSpecification, for_reading: bool) -> None:
        self._prepared_data = pyuipc.prepare_data(data_specification, for_reading)

    def read(self) -> List[Union[int, float, bytes]]:
        """Read data from FSUIPC.

        Returns a list of the data items read.
        """

        return pyuipc.read(self._prepared_data)

    def write(self, data: List[Union[int, float, bytes]]) -> None:
        """Write data to FSUIPC.

        Arguments:
        data - a list of values to write to the predefined offsets
        """

        pyuipc.write(self._prepared_data, data)
