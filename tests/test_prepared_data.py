import unittest
from unittest import mock

from fsuipc.prepared_data import PreparedData
import pyuipc


class TestPreparedData(unittest.TestCase):
    def setUp(self):
        super().setUp()
        self.mock_pyuipc_prepare_data = mock.Mock(name="pyuipc.prepare_data")
        self.mock_pyuipc_read = mock.Mock(name="pyuipc.read")
        self.mock_pyuipc_write = mock.Mock(name="pyuipc.write")

        pyuipc_patcher = mock.patch.multiple(
            pyuipc, prepare_data=self.mock_pyuipc_prepare_data, read=self.mock_pyuipc_read,
            write=self.mock_pyuipc_write)
        pyuipc_patcher.start()
        self.addCleanup(pyuipc_patcher.stop)

    def test_constructor_calls_pyuipc_prepare_data_with_provided_arguments(self):
        PreparedData("DATA", "FOR-READING")

        self.mock_pyuipc_prepare_data.assert_called_once_with("DATA", "FOR-READING")

    def test_read_calls_pyuipc_read_with_prepared_data_and_returns_read_data(self):
        prepared = PreparedData("DATA", "FOR-READING")

        data = prepared.read()

        self.assertEqual(self.mock_pyuipc_read.return_value, data)

        self.mock_pyuipc_read.assert_called_once_with(self.mock_pyuipc_prepare_data.return_value)

    def test_write_calls_pyuipc_write_with_prepared_data(self):
        prepared = PreparedData("DATA", "FOR-READING")

        prepared.write("WRITE-DATA")

        self.mock_pyuipc_write.assert_called_once_with(
            self.mock_pyuipc_prepare_data.return_value, "WRITE-DATA")
