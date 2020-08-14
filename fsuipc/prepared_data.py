import pyuipc


class PreparedData():
    def __init__(self, data_specification, for_reading):
        self._prepared_data = pyuipc.prepare_data(data_specification, for_reading)

    def read(self):
        return pyuipc.read(self._prepared_data)

    def write(self, data):
        pyuipc.write(self._prepared_data, data)
