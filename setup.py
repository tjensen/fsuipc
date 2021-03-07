from os import path
import platform
import setuptools
import struct
import sys


with open(path.join(path.abspath(path.dirname(__file__)), "README.md"), "r", encoding="utf8") as fh:
    long_description = fh.read()


if platform.system() != "Windows":
    sys.stderr.write("This package only supports Windows platforms\n")
    exit(1)

if struct.calcsize("P") == 8:
    ipcuser_source = "src/UIPC64_SDK_C_version2/IPCuser64.c"
    include_dir = "src/UIPC64_SDK_C_version2"
else:
    ipcuser_source = "src/UIPC_SDK_C/IPCuser.c"
    include_dir = "src/UIPC_SDK_C"


setuptools.setup(
    name="fsuipc",
    version="1.3.1",
    author="Tim Jensen",
    author_email="tim.l.jensen@gmail.com",
    description="Client wrapper for FSUIPC",
    long_description=long_description,
    long_description_content_type="text/markdown",
    license="MIT",
    url="https://github.com/tjensen/fsuipc",
    project_urls={
        "Source": "https://github.com/tjensen/fsuipc",
        "Tracker": "https://github.com/tjensen/fsuipc/issues"
    },
    keywords=["flight", "simulator", "pyuipc"],
    packages=["fsuipc", "pyuipc-stubs"],
    package_data={
        "fsuipc": ["py.typed"],  # PEP 561
        "pyuipc-stubs": ["__init__.pyi"]
    },
    zip_safe=False,  # Enables mypy to find the installed package
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Environment :: Console",
        "Environment :: Win32 (MS Windows)",
        "License :: OSI Approved :: MIT License",
        "Operating System :: Microsoft :: Windows",
        "Programming Language :: Python :: 3.5",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Topic :: Games/Entertainment :: Simulation"
    ],
    python_requires=">=3.5",
    setup_requires=["wheel"],
    ext_modules=[setuptools.Extension(
        name="pyuipc",
        sources=[
            ipcuser_source,
            "src/UIPC_SDK_Python/pyuipc.cc"
        ],
        include_dirs=[include_dir],
        libraries=["user32"])]
)
