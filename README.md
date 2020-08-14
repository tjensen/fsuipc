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

Note that fsuipc only supports Windows platforms.

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
