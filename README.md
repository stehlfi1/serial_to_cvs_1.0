# serial_to_cvs_1.0

This is simple script to get ekg and emg data from arduino to csv.
Also includes realtime graphs.

## Inside this repo

- EKG and EMG arduino files.
- standalone backend script without graphs
- simple gui seving as main for now
- already compiled into working exe, can be found in dist

## How to Run
- clone/download repo
- already compiled exe in dist (py installer used)
- running ekg_n_emg_gui.py 
- running ekg_n_emg_be for only be

## How to use

- run script
- select mode
- select serial coms ( use this if you cant figure them out: python -m serial.tools.list_ports)
- set params confirms aformentioned settings
- start starts arduino and communication also starts up rt graphs
- start and end recording selfexplanatory
- exit selfexplanatory

## Notes

Earlier version had cpu heavy plotting, for now still in BE. Current version less heavy, 20fps, however quite ugly.

## TO DO
- improve this read me
- polish code
- bug testing




