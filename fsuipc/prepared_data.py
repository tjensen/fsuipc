import pyuipc


class PreparedData():
    """Represents a data specification for reading and writing data.

    Use this class when repeatedly reading or writing the same set of offsets
    to reduce the cost of converting the sets of offsets and their types.

    Instances of PreparedData should be created using the FSUIPC class's
    prepare_data method.
    """

    def __init__(self, data_specification, for_reading):
        self._prepared_data = pyuipc.prepare_data(data_specification, for_reading)

    def read(self):
        """Read data from FSUIPC.

        Returns a list of the data items read.
        """

        return pyuipc.read(self._prepared_data)

    def write(self, data):
        """Write data to FSUIPC.

        Arguments:
        data - a list of values to write to the predefined offsets
        """

        pyuipc.write(self._prepared_data, data)
