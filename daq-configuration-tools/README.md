# Instructions
This folder scripts to operate on DAQ configuration to extract, load or update threshold and baseline settings. There are three relevant set of parameters in the PMT `fcl` files:
- `triggerThresholdXX`: absolute threshold value for channel `XX` in the ADC scale (integer value between `0` and `16383`). These parameters are set inside the board registers to define the threshold levels.
- `channelPedestalXX`: pedestal or DC offset value for channel `XX` in the DAC scale (integer value between `0` and 65535`. These parameters are set inside the board registers to define the baseline levels.
- `BaselineChXX`: absolute baseline value for channel `XX` in the ADC scale (integer value between `0` and `16383`. These values are NOT written directly in the board registers, but they are used to compute the `triggerThresholdXX` values given the physical (relative) threshold we desire.

The relationship between `channelPedestalXX` and `BaselineChXX` is determined via a channel-by-channel calibration. Unfortunately, in our experience we have seen a lot of instability in this relation.
Despite keeping the DC offsets constant, baselines can spread out up to +/-100ADC counts from their nominal position. However, the majority of the channels live within +/- 15 ADC counts from the nominal set value.

## Setting up
The code requires some specific python libraries, so the advice is to set up a python environment in which to install everything. To do so:

1. First, get a recent version of `icaruscode`: `setup icaruscode v09_66_02 -q e20:prof`
2. Create the environment at a destination of your preference:  `python3 -m venv /path/to/env/`
3. Do `source path/to/env/bin/activate`
4. Finally, install all requirements:  `python -m pip install -r requirement.txt`

The required libraries (and much more) are in the file [requirement.txt](../requirement.txt). 
After the first time, do step 1 and 3 to activate the environment each time.

## Scripts

- `extractConfigPedestalThreshold.py`: This script is used to extract the pedestal, target baseline and theshold values from a DAQ configutation and store it in a `.csv` file. Usage:
```
python extractConfigPedestalThreshold.py <config-directory>
``` 
- `loadConfigPedestalThreshold.py`: This script loads the pedestal, target baseline and theshold values from a `.csv` file into a new DAQ configuration. It takes as input a configuration (tipically in /workdir) and creates another one in the same directory with the new settings. Usage:
```
python loadConfigPedestalThreshold.py <path-to-csv> <path-to-config> <new-name>
```
- `updateConfigThresholds.py`: This script updates the PMT relative threshold in a DAQ configuration. It assumes the pedestal and target baseline values are still valid. If that's not the case, update everything using `loadConfigPedestalThreshold.py`. Usage:
```
python updateConfigThresholds.py <path-to-config> <new-threshold>
```
