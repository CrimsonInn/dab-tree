#!/usr/bin/env python
# -*- coding: utf-8 -*-
from ctypes import cdll

dllPath='src/cluster/test.so'
mydll = cdll.LoadLibrary(dllPath)
print(mydll)
print(mydll.run())
def start():
    pass

