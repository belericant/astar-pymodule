from setuptools import setup, Extension

setup(name="astar",
      version="0.1",
      ext_modules=[Extension("astar", ["wrapper.c", "astar.c", "graph.c", "heap.c", "hashmap.c"])])
