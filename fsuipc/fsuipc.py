from fsuipc.prepared_data import PreparedData
import pyuipc


class FSUIPC():
    def __init__(self, version=pyuipc.SIM_ANY):
        """Opens a connection to FSUIPC. By default, connects to any available flight simulator."""
        pyuipc.open(version)

    def close(self):
        """Closes connection to FSUIPC."""
        pyuipc.close()

    def prepare_data(self, data_specification, for_reading):
        return PreparedData(data_specification, for_reading)

    def read(self, unprepared_data):
        return pyuipc.read(unprepared_data)

    def write(self, unprepared_data):
        return pyuipc.write(unprepared_data)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()
