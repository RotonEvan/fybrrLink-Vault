# fybrrLink-Vault
the underground storage cellar for fybrrLink research project. feel free to contribute.

# Performance evaluation
We have provided the modified files in the folder **NS3_Simulator**

## Steps to run the simulation
  1. Clone the repository from [this] (https://github.com/anysam/starlink_ns3/) link.
  2. Replace the starlink_ns3/ns-allinone-3.30.1/ns-3.30.1/src/leo-satellite with NS3_Simulator/leo-satellite
  3. Open terminal in starlink_ns3/ns-allinone-3.30.1/ns-3.30.1 directory
  4. Follow NS3 configuration setup steps from this [link] (https://www.nsnam.org/docs/tutorial/html/getting-started.html)
  5. Run the QoS aware routing simulation with the following command
  ```console
  foo@bar:~$ sudo ./waf --run qos-aware-routing
  foo
  ```


