import platform
import setuptools
import struct
import sys


with open("README.md", "r") as fh:
    long_description = fh.read()


if struct.calcsize("P") != 4 or platform.system() != "Windows":
    sys.stderr.write("This package requires a 32-bit version of Python for Windows\n")
    exit(1)


setuptools.setup(
    name="fsuipc",
    version="1.0.3",
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
    python_requires=">=3.5,<3.9",
    setup_requires=["wheel"],
    ext_modules=[setuptools.Extension(
        name="pyuipc",
        sources=[
            "src/UIPC_SDK_C/IPCuser.c",
            "src/UIPC_SDK_Python/pyuipc.cc"
        ],
        include_dirs=["src/UIPC_SDK_C"],
        libraries=["user32"])]
)
