Python Coding Guidelines
------------------------

.. _s-python-boilerplate:

Python Boilerplate
##################

.. index::
   pair: Python; boilerplate
   pair: licensing; Python boilerplate

If a Python file is meant to be executed (as opposed to imported), it should
have a ``.in`` extension, and its first line should be:

.. code-block:: python

   #!@PYTHON@

which will be replaced with the appropriate python executable when Pacemaker is
built. To make that happen, add an ``AC_CONFIG_FILES()`` line to
``configure.ac``, and add the file name without ``.in`` to ``.gitignore`` (see
existing examples).

After the above line if any, every Python file should start like this:

.. code-block:: python

   """ <BRIEF-DESCRIPTION>
   """

   # Pacemaker targets compatibility with Python 2.7 and 3.2+
   from __future__ import print_function, unicode_literals, absolute_import, division

   __copyright__ = "Copyright <YYYY[-YYYY]> the Pacemaker project contributors"
   __license__ = "<LICENSE> WITHOUT ANY WARRANTY"

*<BRIEF-DESCRIPTION>* is obviously a brief description of the file's
purpose. The string may contain any other information typically used in
a Python file `docstring <https://www.python.org/dev/peps/pep-0257/>`_.

The ``import`` statement is discussed further in :ref:`s-python-future-imports`.

``<LICENSE>`` should follow the policy set forth in the
`COPYING <https://github.com/ClusterLabs/pacemaker/blob/master/COPYING>`_ file,
generally one of "GNU General Public License version 2 or later (GPLv2+)"
or "GNU Lesser General Public License version 2.1 or later (LGPLv2.1+)".


Python Compatibility
####################

.. index::
   single: Python; 2
   single: Python; 3
   single: Python; versions

Pacemaker targets compatibility with Python 2.7, and Python 3.2 and
later. These versions have added features to be more compatible with each
other, allowing us to support both the 2 and 3 series with the same code. It is
a good idea to test any changes with both Python 2 and 3.

.. _s-python-future-imports:

Python Future Imports
_____________________

The future imports used in :ref:`s-python-boilerplate` mean:

* All print statements must use parentheses, and printing without a newline
  is accomplished with the ``end=' '`` parameter rather than a trailing comma.
* All string literals will be treated as Unicode (the ``u`` prefix is
  unnecessary, and must not be used, because it is not available in Python 3.2).
* Local modules must be imported using ``from . import`` (rather than just
  ``import``). To import one item from a local module, use
  ``from .modulename import`` (rather than ``from modulename import``).
* Division using ``/`` will always return a floating-point result (use ``//``
  if you want the integer floor instead).

Other Python Compatibility Requirements
_______________________________________

* When specifying an exception variable, always use ``as`` instead of a comma
  (e.g. ``except Exception as e`` or ``except (TypeError, IOError) as e``).
  Use ``e.args`` to access the error arguments (instead of iterating over or
  subscripting ``e``).
* Use ``in`` (not ``has_key()``) to determine if a dictionary has a particular
  key.
* Always use the I/O functions from the ``io`` module rather than the native
  I/O functions (e.g. ``io.open()`` rather than ``open()``).
* When opening a file, always use the ``t`` (text) or ``b`` (binary) mode flag.
* When creating classes, always specify a parent class to ensure that it is a
  "new-style" class (e.g. ``class Foo(object):`` rather than ``class Foo:``).
* Be aware of the bytes type added in Python 3. Many places where strings are
  used in Python 2 use bytes or bytearrays in Python 3 (for example, the pipes
  used with ``subprocess.Popen()``). Code should handle both possibilities.
* Be aware that the ``items()``, ``keys()``, and ``values()`` methods of
  dictionaries return lists in Python 2 and views in Python 3. In many case, no
  special handling is required, but if the code needs to use list methods on
  the result, cast the result to list first.
* Do not raise or catch strings as exceptions (e.g. ``raise "Bad thing"``).
* Do not use the ``cmp`` parameter of sorting functions (use ``key`` instead,
  if needed) or the ``__cmp__()`` method of classes (implement rich comparison
  methods such as ``__lt__()`` instead, if needed).
* Do not use the ``buffer`` type.
* Do not use features not available in all targeted Python versions. Common
  examples include:

  * The ``html``, ``ipaddress``, and ``UserDict`` modules
  * The ``subprocess.run()`` function
  * The ``subprocess.DEVNULL`` constant
  * ``subprocess`` module-specific exceptions

Python Usages to Avoid
______________________

Avoid the following if possible, otherwise research the compatibility issues
involved (hacky workarounds are often available):

* long integers
* octal integer literals
* mixed binary and string data in one data file or variable
* metaclasses
* ``locale.strcoll`` and ``locale.strxfrm``
* the ``configparser`` and ``ConfigParser`` modules
* importing compatibility modules such as ``six`` (so we don't have
  to add them to Pacemaker's dependencies)


Formatting Python Code
######################

.. index:: Python; formatting

* Indentation must be 4 spaces, no tabs.
* Do not leave trailing whitespace.
* Lines should be no longer than 80 characters unless limiting line length
  significantly impacts readability. For Python, this limitation is
  flexible since breaking a line often impacts readability, but
  definitely keep it under 120 characters.
* Where not conflicting with this style guide, it is recommended (but not
  required) to follow `PEP 8 <https://www.python.org/dev/peps/pep-0008/>`_.
* It is recommended (but not required) to format Python code such that
  ``pylint
  --disable=line-too-long,too-many-lines,too-many-instance-attributes,too-many-arguments,too-many-statements``
  produces minimal complaints (even better if you don't need to disable all
  those checks).
