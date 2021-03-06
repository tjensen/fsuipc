import unittest
from unittest import mock

import pyuipc

import fsuipc


class TestModule(unittest.TestCase):
    def test_error_constants_match_pyuipc_error_constants(self) -> None:
        self.assertEqual(pyuipc.ERR_OK, fsuipc.ERR_OK)
        self.assertEqual(pyuipc.ERR_OPEN, fsuipc.ERR_OPEN)
        self.assertEqual(pyuipc.ERR_NOFS, fsuipc.ERR_NOFS)
        self.assertEqual(pyuipc.ERR_REGMSG, fsuipc.ERR_REGMSG)
        self.assertEqual(pyuipc.ERR_ATOM, fsuipc.ERR_ATOM)
        self.assertEqual(pyuipc.ERR_MAP, fsuipc.ERR_MAP)
        self.assertEqual(pyuipc.ERR_VIEW, fsuipc.ERR_VIEW)
        self.assertEqual(pyuipc.ERR_VERSION, fsuipc.ERR_VERSION)
        self.assertEqual(pyuipc.ERR_WRONGFS, fsuipc.ERR_WRONGFS)
        self.assertEqual(pyuipc.ERR_NOTOPEN, fsuipc.ERR_NOTOPEN)
        self.assertEqual(pyuipc.ERR_NODATA, fsuipc.ERR_NODATA)
        self.assertEqual(pyuipc.ERR_TIMEOUT, fsuipc.ERR_TIMEOUT)
        self.assertEqual(pyuipc.ERR_SENDMSG, fsuipc.ERR_SENDMSG)
        self.assertEqual(pyuipc.ERR_DATA, fsuipc.ERR_DATA)
        self.assertEqual(pyuipc.ERR_RUNNING, fsuipc.ERR_RUNNING)
        self.assertEqual(pyuipc.ERR_SIZE, fsuipc.ERR_SIZE)

    def test_simulator_constants_match_pyuipc_simulator_constants(self) -> None:
        self.assertEqual(pyuipc.SIM_ANY, fsuipc.SIM_ANY)
        self.assertEqual(pyuipc.SIM_FS98, fsuipc.SIM_FS98)
        self.assertEqual(pyuipc.SIM_FS2K, fsuipc.SIM_FS2K)
        self.assertEqual(pyuipc.SIM_CFS2, fsuipc.SIM_CFS2)
        self.assertEqual(pyuipc.SIM_CFS1, fsuipc.SIM_CFS1)
        self.assertEqual(pyuipc.SIM_FLY, fsuipc.SIM_FLY)
        self.assertEqual(pyuipc.SIM_FS2K2, fsuipc.SIM_FS2K2)
        self.assertEqual(pyuipc.SIM_FS2K4, fsuipc.SIM_FS2K4)
        self.assertEqual(pyuipc.SIM_FSX, fsuipc.SIM_FSX)
        self.assertEqual(pyuipc.SIM_ESP, fsuipc.SIM_ESP)
        self.assertEqual(pyuipc.SIM_P3D, fsuipc.SIM_P3D)
        self.assertEqual(pyuipc.SIM_FSX64, fsuipc.SIM_FSX64)
        self.assertEqual(pyuipc.SIM_P3D64, fsuipc.SIM_P3D64)

    def test_fsuipc_exception_matches_pyuipc_fsuipc_exception(self) -> None:
        assert pyuipc.FSUIPCException is fsuipc.FSUIPCException


class TestFSUIPC(unittest.TestCase):
    def setUp(self) -> None:
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

    def test_constructor_calls_pyuipc_open(self) -> None:
        fsuipc.FSUIPC()

        self.mock_pyuipc_open.assert_called_once_with(pyuipc.SIM_ANY)

    def test_close_calls_pyuipc_close(self) -> None:
        fsuipc.FSUIPC().close()

        self.mock_pyuipc_close.assert_called_once_with()

    def test_object_can_be_used_as_a_context_manager(self) -> None:
        with fsuipc.FSUIPC():
            self.mock_pyuipc_open.assert_called_once_with(pyuipc.SIM_ANY)

        self.mock_pyuipc_close.assert_called_once_with()

    def test_constructor_passes_simulator_version_to_pyuipc_when_provided(self) -> None:
        fsuipc.FSUIPC(42)

        self.mock_pyuipc_open.assert_called_once_with(42)

    def test_prepare_data_creates_a_prepared_data_instance_and_returns_it(self) -> None:
        ipc = fsuipc.FSUIPC()

        result = ipc.prepare_data(mock.sentinel.data, mock.sentinel.for_reading)

        self.assertIsInstance(result, fsuipc.PreparedData)

    def test_read_calls_pyuipc_read_with_unprepared_data(self) -> None:
        ipc = fsuipc.FSUIPC()

        result = ipc.read(mock.sentinel.data)

        self.assertEqual(self.mock_pyuipc_read.return_value, result)

        self.mock_pyuipc_read.assert_called_once_with(mock.sentinel.data)

    def test_write_calls_pyuipc_write_with_unprepared_data(self) -> None:
        ipc = fsuipc.FSUIPC()

        ipc.write(mock.sentinel.data)

        self.mock_pyuipc_write.assert_called_once_with(mock.sentinel.data)

    @mock.patch("fsuipc.fsuipc.pyuipc")
    def test_get_simulator_version_returns_simulator_version(self, mock_pyuipc: mock.Mock) -> None:
        mock_pyuipc.fs_version = 23

        ipc = fsuipc.FSUIPC()

        self.assertEqual(23, ipc.get_simulator_version())

    @mock.patch("fsuipc.fsuipc.pyuipc")
    def test_get_fsuipc_version_returns_fsuipc_version(self, mock_pyuipc: mock.Mock) -> None:
        mock_pyuipc.fsuipc_version = 0x10230004

        ipc = fsuipc.FSUIPC()

        self.assertEqual("1.023d", ipc.get_fsuipc_version())

    @mock.patch("fsuipc.fsuipc.pyuipc")
    def test_get_library_version_returns_lib_version(self, mock_pyuipc: mock.Mock) -> None:
        mock_pyuipc.lib_version = 54021

        ipc = fsuipc.FSUIPC()

        self.assertEqual("54.021", ipc.get_library_version())
