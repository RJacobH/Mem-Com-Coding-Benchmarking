# Mem-Com-Coding-Benchmarking
Solutions for question 1 of the Mem-Com-Coding assignment

Below is an explanation of the goals of my code as well as answers to the written part of this assignment.


(a) Benchmarking
  I did not manage to complete this code as well as I would have liked to. However, I did attempt it, and I have some explanation for my attempt. 
  
First, I attempted to accurately get the mean time to read a single byte by using the clock. In the middle of everything else, the central piece of my code is three lines - a line to record the clock, a line to read a byte from an array, and a line to record the clock again. By finding the difference between the first and second clock reads, I tried to record how long that middle step took. I suspect that the read time is much smaller than my results, as when I removed the middle line from between the clock reads, the time did not change significantly. 

To overcome the miniscule size of the read, I made a number of reads, then averaged their time. I see now that it might have produced better results and actually addressed the issue if I had done multiple reads BETWEEN THE CLOCK READS. In order to get something accurate for this, I worry how timing the for-loop and clock overhead could have been dealt with effectively. 

Limiting what happens between the two clock reads was my main way to try to isolate hardware reads. For some reason that I was unable to identify, every million or so reads produced a 16-digit read time. 


(b) Graphing
  As you see if you run the code, I make a primitive graph in the terminal as opposed to using gnuplot or exporting my data to python, then plotting from there. My reason for this was that it felt sufficient to show the relationship between the latency and buffer size, and that my code did not produce a clear relationship between the two. I argue that the data is readable and useful, despite not having numbers on the axies. It shows that I coded an exponential relationship between the axies (points get more spread out as the buffer size increases) and I also think it looks pretty cool and feels pretty minimistically elegant.
  
  
(c) Analysis
  As I didn't get good data, I'm unable to identify solely from that the sizes of the caches on my computer. There is a way to find the cache sizes on Mac, and I'm figuring that out now. If I did have good data, I suspect I would see the data points form a specific shape - near the x-axis until some buffer size was reached, then quickly jump up to some larger latency-per-read value and make a line parallel to the x-axis there. It would jump up and go flat one or two more times, eventually just continuing flat. The values on the x-axis where the data jumped up would be the cache sizes of my computer - As the amount of data that needs to be stored increases, the data is stored deeper in the memory/cache system. Any deviations to this pattern may have been caused by things other than memory reads that have been counted in the timing as well, they would have to be common enough to affect the data.
