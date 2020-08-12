import pyuipc


class FSUIPC():
    def __init__(self):
        pyuipc.open(pyuipc.SIM_ANY)

    def close(self):
        pyuipc.close()

    def prepare_data(self, data, for_reading):
        return pyuipc.prepare_data(data, for_reading)

    def read(self, data):
        return pyuipc.read(data)

    def write(self, data, prepared_data=None):
        print(prepared_data)
        if prepared_data is None:
            return pyuipc.write(data)
        else:
            return pyuipc.write(prepared_data, data)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()
