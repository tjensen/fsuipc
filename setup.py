import setuptools


with open("README.md", "r") as fh:
    long_description = fh.read()


setuptools.setup(
    name="fsuipc",
    version="1.0.0a0",
    author="Tim Jensen",
    author_email="tim.l.jensen@gmail.com",
    description="Client wrapper for FSUIPC",
    long_description=long_description,
    long_description_content_type="text/markdown",
    license="MIT",
    url="https://github.com/tjensen/fsuipc",
    packages=setuptools.find_packages(exclude=["tests"]),
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Environment :: Console",
        "Environment :: Win32 (MS Windows)",
        "License :: OSI Approved :: MIT License",
        "Operating System :: Microsoft :: Windows",
        "Programming Language :: Python :: 3.8",
        "Topic :: Games/Entertainment :: Simulation"
    ],
    python_requires=">=3.3,<3.9",
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
