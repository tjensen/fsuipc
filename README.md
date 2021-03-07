# fsuipc

This project provides a simple Python client class wrapper around the
[FSUIPC](http://www.fsuipc.com/) tool, which allows third-party programs to
interact with the "inner workings" of popular flight simulator software.

Fsuipc is built on top of (and includes) `pyuipc`, by István Váradi
(ivaradi@varadiistvan.hu).

## Installation

The easiest way to install this package is using `pip`:

```
pip install fsuipc
```

Note that fsuipc only supports Python on Windows platforms.

## Basic Usage

```python
from fsuipc import FSUIPC


with FSUIPC() as fsuipc:
    prepared = fsuipc.prepare_data([
        (0x560, "l"),
        (0x568, "l"),
        (0x570, "l")
    ], True)

    while True:
        latitude, longitude, altitude = prepared.read()

        print(f"Latitude: {latitude}")
        print(f"Longitude: {longitude}")
        print(f"Altitude: {altitude}")

        input("Press ENTER to read again")
```

## Troubleshooting

### IPC request contains bad data

This error can be caused by a word length mismatch between the client and
server processes. If your flight simulator software is 32-bit, you probably
need to be running a 32-bit version of Python. Similarly, if your flight
simulator is 64-bit, you need to run a 64-bit version of Python.

You can check if your Python is 32-bit or 64-bit by running the command:

```
python -VV
```

### IPC sendmessage failed all retries

This error can occur if you need to run your Python script as an administrator
in order to communicate with the server process.
