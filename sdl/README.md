# Software Diagnostic Library
SDL is a compilation of Safety Software for different Texas Instruments SoCs that implements diagnostic software. It also includes reference examples for helping others implement the Safety Software in their application.

## Usage
Use the following make command for instructions on building a module, example or test in this project:
> make help

## Dependencies
The following projects are required to build and use the SDL Project:

### Code Composer Studio (CCS)
CCS is required to load the application and test executables onto the target SoC. Find the latest download here: https://software-dl.ti.com/ccs/esd/documents/ccs\_downloads.html

### Code Generation Tools (CGT)
CGT is included with the installation of CCS. Note the directory for editing the Rules.make.

### SDK Dependencies
The tests and examples depend on some modules from the SDK. For Jacinto SDK, the tests/examples depend on modules from the PDK.
The PDK, CSL and rm_pm_hal must be cloned to build tests for j721e. Alternatively, the SDK can be downloaded and used. The path for the PDK must be updated in the Rules.make file.

## Environment Setup
The project assumes a version and location of the Code Generation Tools (CGT) in the Rules.make file. Edit to match your environment.
The project assumes a locatiaon for the PDK in the Rules.make file. Edit to match your environment.




