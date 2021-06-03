// demo.grog implements a simple loop with a counter; ends after 10 iterations.
LOAD x0 00
LOAD x1 01
LOAD x2 0A
:loop
ADD x0 x1
BNEQ x0 x2 :loop
XCF