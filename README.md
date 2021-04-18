# fybrrLink-Vault
the underground storage cellar for fybrrLink research project. feel free to contribute.

# Performance evaluation
We have provided the modified files in the folder **NS3_Simulator**

## Steps to run the simulation
  1. Clone the repository. Link : https://github.com/anysam/starlink_ns3/
  2. Replace the starlink_ns3/ns-allinone-3.30.1/ns-3.30.1/src/leo-satellite folder with NS3_Simulator/leo-satellite folder
  3. Open terminal in starlink_ns3/ns-allinone-3.30.1/ns-3.30.1 directory
  4. Follow NS3 configuration setup steps. Link : https://www.nsnam.org/docs/tutorial/html/getting-started.html
  5. Run the QoS aware routing simulation with the following command
  ```console
  foo@bar:~$ sudo ./waf --run qos-aware-routing
  ```
  
## Plotting data
Data collected (After the simulation) for the plotting is present in the python-scripts/analysis.csv. A python script with name plotAnalysis.py (present in the same folder only) can be used to get the plots.   

## Complexity Analysis Script
A python script for complexity analysis (file - bresenham.py) is present in the python-scripts folder.

## fybrrLink-VizTool
The Modified Bresenham's Routing Algorithm Visualization Tool is hosted at - https://fybrrlink-viztool.herokuapp.com. The code can be found at - https://github.com/RotonEvan/fybrrLink-VizTool
