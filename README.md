The IBMStreams/streamsx.ps GitHub repository is home to the Streams toolkit named PS (Process Store).
This Streams toolkit allows a simple way for the SPL and C++ operators that are fused inside a
single PE (Processing Element) to share the application specific state information. It does this via
a collection of APIs that can be called from any part of the SPL and C++ operator code. This toolkit
is very different from another similar sounding DPS toolkit which works entirely differently for
sharing state among the non-fused operators. Please be clear that PS and DPS have nothing in common.
The scope of the PS toolkit is only for those operators that are fused within a single PE.
                               
There are plenty of details available about the API description and the built-in example. Please refer to
the com.ibm.streamsx.ps/doc/ps-usage-tips.txt file for getting a good start in using this toolkit.
