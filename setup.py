from distutils.core import setup, Extension

setup(
    name='dublin',
    version='1.0',
    ext_modules=[
        Extension('dublin',
        sources=['dublin.c'])
    ]   
)