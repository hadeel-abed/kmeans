from setuptools import setup, Extension
import sys
import os

# Ensure we use the correct MS Visual C++ version
os.environ['DISTUTILS_USE_SDK'] = '1'
os.environ['MSSdk'] = '1'

# Determine the Python architecture
is_64bits = sys.maxsize > 2**32

# Set the appropriate library dirs and libraries
if is_64bits:
    library_dirs = ['C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\um\\x64',
                    'C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\ucrt\\x64']
    libraries = ['ucrt', 'vcruntime']
else:
    library_dirs = ['C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\um\\x86',
                    'C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\ucrt\\x86']
    libraries = ['ucrt', 'vcruntime']

# Define the extension module
symnmf_module = Extension('symnmfmodule',
                          sources=['symnmf.c', 'symnmfmodule.c'],
                          include_dirs=['.'],
                          library_dirs=library_dirs,
                          libraries=libraries,
                          extra_compile_args=['/DNDEBUG', '/Ox', '/MD', '/W3', '/GS-', '/DWIN32', '/D_WINDOWS'],
                          extra_link_args=['/MANIFEST'])

setup(name='symnmfmodule',
      version='1.0',
      description='SymNMF module for Python',
      ext_modules=[symnmf_module])
