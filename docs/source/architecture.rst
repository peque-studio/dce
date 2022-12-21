Architecture
============

The engine consists of multiple "modules" (they aren't unloadable/dynamic or anything,
it's just a way we separate related code into chunks)

Graphics
--------

Currently that largest module of all, it contains everything related to graphics,
the Vulkan API handling, multiple (in future; one currently) renderer implementations.
The graphics module types and function live under the ``DCg`` namespace.

Renderers
~~~~~~~~~

A renderer is a bunch of utility types and functions that set up and interact with
the graphics functions and provide an abstraction layer over the raw graphics api.

Currently there is just one renderer: ``DCgBasicRenderer``, but in the future there
will be a separate user interface renderer (``DCgUIRenderer``).

Mathematics
-----------

This is a utility module providing types and inline functions for mathematical constructs
such as vectors and matricies. Mathematics is under the ``DCm`` namespace.

Vectors and Matricies are provided with different underlying types (``float``, ``int``, etc.)
A 4D vector of unsigned 16-bit ingegers is called :code:`DCmVector4u16`. A 3 by 3 matrix of
floats is :code:`DCmMatrix3x3f`. There are also some useful typedefs, for example a ``DCmVector2`` is
a ``DCmVector2f``.

Memory
------

This module provides functions for handling memory allocation. It also includes arenas,
a simple but useful memory management technique. It lives under the ``DCmem`` namespace.

Debug
-----

This module contains functions and macros for logging. As the name suggests, it isn't used
in release builds (it's used, but a stripped down version only). The namespace is ``DCd``.

To be continued...
------------------

Currently, these are all the modules that exist, as the engine is very early in development.
