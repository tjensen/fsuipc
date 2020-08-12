# -*- coding: utf-8 -*-

from distutils.core import setup, Extension
import distutils.ccompiler as ccompiler
import sys
if sys.version_info.major>=3 and sys.version_info.minor>=5:
    import distutils._msvccompiler as msvccompiler
else:
    import distutils.msvccompiler as msvccompiler
import os
import shutil

scriptdir=os.path.abspath(os.path.dirname(sys.argv[0]))

pyuipc_pyd=os.path.join(scriptdir, "pyuipc.pyd")

class DummyCompiler(ccompiler.CCompiler):
    """A dummy compiler class.

    This does not do anything when compiling, and it copies an already existing
    pyuipc.pyd file when linking."""
    def __init__(self, verbose = 0, dry_run = 0, force = 0):
        """Construct the compiler."""
        pass

    def compile(self, sources, output_dir=None, macros=None,
                include_dirs=None, debug=0, extra_preargs=None,
                extra_postargs=None, depends=None):
        """Just return a list of .o files from the .cc files."""
        return [source.replace(".cc", ".o") for source in sources]

    def link(self, target_desc, objects, output_filename, output_dir=None,
             libraries=None, library_dirs=None, runtime_library_dirs=None,
             export_symbols=None, debug=0, extra_preargs=None,
             extra_postargs=None, build_temp=None, target_lang=None):
        """Copy the pyuipc.pyd file to its final location."""
        if output_dir is None:
            output_dir = os.path.dirname(output_filename)
        if not os.path.isdir(output_dir):
            os.makedirs(output_dir)
        shutil.copyfile(pyuipc_pyd, output_filename)

long_description="""A wrapper for FSUIPC for Python

FSUIPC is an interface to Microsoft Flight Simulator
written by Peter Dowson
(see http://www.schiratti.com/dowson.html).

This module is a wrapper for this interface that can be
used from Python programs."""

if sys.argv[1]=='-v':
    pyuipc_pyd=os.path.join(scriptdir, "pyuipc.pyd." + sys.argv[2])
    sys.argv = sys.argv[0:1] + sys.argv[3:]

# If we have the module, override the MSVC compiler with out dummy one.
if os.path.exists(pyuipc_pyd):
    msvccompiler.MSVCCompiler = DummyCompiler

setup(name = "pyuipc",
      version = "0.6",
      description = "FSUIPC wrapper for Python",
      long_description = long_description,
      platforms = ["Win32"],
      license = "Public Domain",
      author = "István Váradi",
      author_email = "ivaradi@gmail.com",
      url = "http://mlx.varadiistvan.hu",
      ext_modules = [Extension("pyuipc", ["pyuipc.cc"])],
      )
