**Carpooling Management System**

A small C-based carpool management project that provides core matching logic and a simple user interface. The repository contains source code in `code/` and sample/testing data in `testing/`.

**Features**
- **Core matching logic:** implemented in `corelogic.c`.
- **User interface/driver for operations:** implemented in `carpool_gui.c`.
- **Sample data files:** located in the `testing/` folder to exercise the program.

**Repository layout**
- `code/` : source files (`carpool_gui.c`, `corelogic.c`).
- `testing/` : example data files used by the program:
  - `admin.txt` — admin configuration or credentials (project-specific).
  - `drivers.txt` — driver records.
  - `passengers.txt` — passenger records.
  - `flex_rides.txt` — flexible ride entries or requests.
  - `matches.txt` — output or sample matches produced by the program.

**Requirements**
- A C compiler (GCC/MinGW on Windows, or any standard C toolchain).

**Build (Windows, cmd.exe)**
1. Open `cmd.exe` in the repository root (where this `README.md` is located).
2. Compile the sources with GCC (adjust if you have a different compiler):

```
gcc code\*.c -o carpool
```

This creates an executable named `carpool` in the current directory.

**Run**
- Run the program from the repository root. If the program reads files from `testing/`, run it without arguments or consult the source for required arguments:

```
carpool
```

- If the program expects file paths or command-line options, provide them accordingly. If you want, I can open `carpool_gui.c` and `corelogic.c` to extract exact command-line usage and update this README with precise run examples.

**Testing / Sample Data**
- The `testing/` directory contains sample files you can use to exercise the program. To reproduce a run, place the files next to the executable or pass their paths if the program requires it.

**Notes & Next Steps**
- This README contains general build/run guidance. I can:
  - Inspect `carpool_gui.c` and `corelogic.c` and add exact compile flags and command-line usage examples.
  - Add sample expected input formats for each file in `testing/`.
  - Add a small script or `Makefile` (or `build.bat`) to simplify building on Windows.

If you'd like those improvements, tell me whether you want me to (1) auto-detect run arguments from the sources and update README, (2) create a `build.bat` for Windows, or (3) do both.

**License & Contact**
-Use n Enjoy :)
