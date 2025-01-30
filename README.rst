TI Processor SDK RTOS J721E EVM
===============================

This repository is a stripped version of the `TI Processor SDK
<https://dr-download.ti.com/software-development/software-development-kit-sdk/MD-50weZVBfzl/09.02.00.05/ti-processor-sdk-rtos-j721s2-evm-09_02_00_05.tar.gz>`_.
It uses SDK Version 09_02_00_05 (J721S2).

Stripping
---------

The following files and directories have been removed before the first commit
to make the SDK Repository smaller:

- ``tisdk-adas-image-j721s2-evm.tar.xz``
- ``boot-adas-j721s2-evm.tar.gz``
- ``pdk_j721s2_09_02_00_30/docs/``
- ``pdk_j721s2_09_02_00_30/packages/ti/boot/sbl/``
- ``ti-cgt-armllvm_3.2.1.LTS`` (toolchain which is part of Container setup)
- ``protobuf-3.20.2`` (part of Container setup)
- all ``.gitignore`` files in the SDK

The files have been stripped by initializing a repository at the location where
the TI SDK has been unpacked. After that, all locations listed in the
``.gitignore`` file have been removed with the following command::

    git clean -fdX

Followed by ``git add .`` and ``git commit -m "ti-rtos-sdk: initial stripped
version"``.
