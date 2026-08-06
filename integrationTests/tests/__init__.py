"""Loads every test module in this directory.

Importing this package populates the registry in lib via decorators.

Submitty's equivalent loader uses loader.find_module().load_module(), which
was removed in Python 3.12, importlib.util is the supported way to do the
same thing.
"""

import importlib.util
import os
import pkgutil
import sys

__all__ = []

for _loader, module_name, _is_pkg in pkgutil.iter_modules(__path__):
    module_path = os.path.join(__path__[0], module_name)
    spec = importlib.util.spec_from_file_location(
        module_name,
        os.path.join(module_path, "__init__.py"),
        submodule_search_locations=[module_path],
    )
    module = importlib.util.module_from_spec(spec)
    sys.modules[module_name] = module
    spec.loader.exec_module(module)

    __all__.append(module_name)
    globals()[module_name] = module