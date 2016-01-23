# NetBenchMark

<h2>About</h2>
This program tests the speed at which two computers can send and received 'blocks'. Each block has a configurable size of 1000, 2000, 5000, and 10,000 bytes. The code is meant to compile on both Linux and Windows. This has been typdef'd in.

The test is meant to measure the overall speed of the network. Other considerations had to be factored in for this test. Such as that we are using standard ethernet to transmit these blocks and that the computers were linked by a simple switch. More of this information can be found in the PDF report that is in the repository as well.

This program has been written in C. Why C? It's pretty close to the metal and allows us minimal application overhead when running this test.
