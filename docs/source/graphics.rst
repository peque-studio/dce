Graphics
========

This module is the biggest of them all! It contains functions that interact with the
Vulkan rendering API with very little abstraction.

State
-----

.. doxygentypedef:: DCgState

This structure contains the data the graphics module functions need to work.
Its members are an implementation detail and are defined in the
``dcore/graphics/internal.h`` header file.

.. doxygenfunction:: dcgNewState
.. doxygenfunction:: dcgInit
.. doxygenfunction:: dcgDeinit
.. doxygenfunction:: dcgFreeState

Windowing
---------

These functions allow interaction with the windowing system.

.. doxygenfunction:: dcgShouldClose
.. doxygenfunction:: dcgClose
.. doxygenfunction:: dcgGetMousePosition
.. doxygenfunction:: dcgUpdate

Commands
--------

.. doxygenfunction:: dcgCmdBegin
.. doxygenfunction:: dcgCmdBindVertexBuf
.. doxygenfunction:: dcgCmdBindIndexBuf
.. doxygenfunction:: dcgCmdBindMat
.. doxygenfunction:: dcgCmdDraw
.. doxygenfunction:: dcgSubmit

Materials
---------

TODO! Materials are vulkan pipelines and layouts together.
