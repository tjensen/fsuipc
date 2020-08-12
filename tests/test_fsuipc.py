import unittest
from unittest import mock

import pyuipc
from fsuipc import FSUIPC


class TestFSUIPC(unittest.TestCase):
    def setUp(self):
        self.mock_pyuipc_open = mock.Mock(name="pyuipc.open")
        self.mock_pyuipc_close = mock.Mock(name="pyuipc.close")
        self.mock_pyuipc_prepare_data = mock.Mock(name="pyuipc.prepare_data")
        self.mock_pyuipc_read = mock.Mock(name="pyuipc.read")
        self.mock_pyuipc_write = mock.Mock(name="pyuipc.write")

        pyuipc_patcher = mock.patch.multiple(
            pyuipc, open=self.mock_pyuipc_open, close=self.mock_pyuipc_close,
            prepare_data=self.mock_pyuipc_prepare_data, read=self.mock_pyuipc_read,
            write=self.mock_pyuipc_write)
        pyuipc_patcher.start()
        self.addCleanup(pyuipc_patcher.stop)

    def test_constructor_calls_pyuipc_open(self):
        FSUIPC()

        self.mock_pyuipc_open.assert_called_once_with(pyuipc.SIM_ANY)

    def test_close_calls_pyuipc_close(self):
        FSUIPC().close()

        self.mock_pyuipc_close.assert_called_once_with()

    def test_object_can_be_used_as_a_context_manager(self):
        with FSUIPC():
            self.mock_pyuipc_open.assert_called_once_with(pyuipc.SIM_ANY)

        self.mock_pyuipc_close.assert_called_once_with()

    def test_prepare_data_calls_pyuipc_prepare_data(self):
        with FSUIPC() as fsuipc:
            result = fsuipc.prepare_data(mock.sentinel.data, mock.sentinel.for_reading)

            self.assertEqual(self.mock_pyuipc_prepare_data.return_value, result)

            self.mock_pyuipc_prepare_data.assert_called_once_with(
                mock.sentinel.data, mock.sentinel.for_reading)

    def test_read_calls_pyuipc_read(self):
        with FSUIPC() as fsuipc:
            result = fsuipc.read(mock.sentinel.data)

            self.assertEqual(self.mock_pyuipc_read.return_value, result)

            self.mock_pyuipc_read.assert_called_once_with(mock.sentinel.data)

    def test_write_calls_pyuipc_write_when_only_data_is_provided(self):
        with FSUIPC() as fsuipc:
            result = fsuipc.write(mock.sentinel.data)

            self.assertEqual(self.mock_pyuipc_write.return_value, result)

            self.mock_pyuipc_write.assert_called_once_with(mock.sentinel.data)

    def test_write_calls_pyuipc_write_when_data_and_prepared_data_are_both_provided(self):
        with FSUIPC() as fsuipc:
            result = fsuipc.write(mock.sentinel.data, mock.sentinel.prepared_data)

            self.assertEqual(self.mock_pyuipc_write.return_value, result)

            self.mock_pyuipc_write.assert_called_once_with(
                mock.sentinel.prepared_data, mock.sentinel.data)
