# PCB2Gcode - A Flying Probe Tester Application

A Qt-based application designed for a Flying Probe Testing (FPT) machine for PCB testing. This application provides G-code generation for precise probe movements and comprehensive PCB testing capabilities.

## Key Features

- **Automated G-code Generation**
  - Dual probe movement coordination
  - Configurable testing sequences
  - Net-based test point grouping

- **File Support**
  - Gerber file processing
  - CSV test point import for custom test points.
  - Automatic pad detection

- **Visualization**
  - Real-time PCB preview
  - Test point overlay

- **Hardware Control**
  - UART communication interface (Under Construction)

## Installation & Setup

### Requirements
- **Qt Framework** (Qt 5 or newer)
- **Python 3.12**
- **Pygerber library installed**
- **Anaconda (Optional)** for managing the Python environment

### Configuration
1. **Edit the `.pro` file**:
   - Set the correct Python paths based on your installed version.
   - Modify the `PYTHON_VERSION` and `PYTHON_VERSIONLIB` variables accordingly.
   - Adjust `CONDA_ENV_PATH` to point to your python environment.

2. **Modify `gerbermanager.cpp`**:
   - Update the file path used for the python file for parsing Gerber files.

### Building the Application

#### Using Qt Creator
1. Open `pcb2gcode.pro` in Qt Creator.
2. Configure the Qt Kit and Compiler.
3. **Build & Run**.


## License

This project is licensed under the MIT License. See the LICENSE file for details.

