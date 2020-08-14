import unittest
from unittest import mock

from fsuipc.prepared_data import PreparedData
import pyuipc


class TestPreparedData(unittest.TestCase):
    def setUp(self) -> None:
        super().setUp()
        self.mock_pyuipc_prepare_data = mock.Mock(name="pyuipc.prepare_data")
        self.mock_pyuipc_read = mock.Mock(name="pyuipc.read")
        self.mock_pyuipc_write = mock.Mock(name="pyuipc.write")

        pyuipc_patcher = mock.patch.multiple(
            pyuipc, prepare_data=self.mock_pyuipc_prepare_data, read=self.mock_pyuipc_read,
            write=self.mock_pyuipc_write)
        pyuipc_patcher.start()
        self.addCleanup(pyuipc_patcher.stop)

    def test_constructor_calls_pyuipc_prepare_data_with_provided_arguments(self) -> None:
        PreparedData(mock.sentinel.data_specification, mock.sentinel.for_reading)

        self.mock_pyuipc_prepare_data.assert_called_once_with(
            mock.sentinel.data_specification, mock.sentinel.for_reading)

    def test_read_calls_pyuipc_read_with_prepared_data_and_returns_read_data(self) -> None:
        prepared = PreparedData([(42, "f")], True)

        data = prepared.read()

        self.assertEqual(self.mock_pyuipc_read.return_value, data)

        self.mock_pyuipc_read.assert_called_once_with(self.mock_pyuipc_prepare_data.return_value)

    def test_write_calls_pyuipc_write_with_prepared_data(self) -> None:
        prepared = PreparedData([(42, "f")], False)

        prepared.write(mock.sentinel.write_data)

        self.mock_pyuipc_write.assert_called_once_with(
            self.mock_pyuipc_prepare_data.return_value, mock.sentinel.write_data)
