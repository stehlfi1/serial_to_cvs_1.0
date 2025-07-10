# serial_to_cvs_1.0

A simple script that streams EKG and EMG data from an Arduino to a CSV file, with optional real-time graphs.

## Repository Contents

- **Arduino sketches** for EKG and EMG (PlatformIO; plain C++ also works)
- **Standalone backend script** (no graphs)
- **Simple GUI** — currently the main entry point
- **Pre-built executable** in `dist/` (built with PyInstaller)

## How to Run

1. **Clone or download** this repository.
2. **Executable:** run the binary in `dist/` (no Python needed).
3. **GUI:** run `ekg_n_emg_gui.py` (Python + required libraries).
4. **Backend only:** run `ekg_n_emg_be.py` (Python + required libraries).

## How to Use

1. Launch the script.
2. Choose **mode**.
3. Select a **serial port** (use `python -m serial.tools.list_ports` if unsure).
4. Click **Set Params** to confirm the above settings.
5. **Start** begins Arduino communication and real-time graphs.
6. **Start Recording** / **End Recording** are self-explanatory.
7. **Exit** closes the application.

Recommendation: restart the application and arduinos after each session. 

## Notes

Two branches provide different internal designs:

- **Concurrent implementation** — recommended for reliability
- **Multiprocessing implementation** — faster graph rendering, may contain minor bugs

## TODO

- Fix remaining multiprocessing bugs
- Replace matplotlib in the real-time stream (current implementation is CPU-heavy)
