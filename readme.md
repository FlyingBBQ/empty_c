I would like to take some time to explain how I approached this assignment, because I think the process is at least as important as the resulting code.
Preferably I would have done this in a setting where I can ask questions.

## Design
After reading the assignment, I took some pen and paper to make a little design.
The first thing that comes to mind is to simply hardcode the thresholds in some `if` `else` statements.
It would do the job, but does not score great on maintainability.
An improvement would be to place these thresholds in a lookup table.
However, since the thresholds are equal in size and have the same amount of noise, this lookup table can also be generated.
Looking at the function prototype it uses `unsigned int` instead of fixed width integers, so I assume this code should be portable for different platforms.
Since the result is based on the previous input percentage I should use a `static` variable like `prev_input` to store the value between calls.

## Searching about hysteresis
At this moment I was about 10min into the assigment and I was ready to start coding.
But, before I started, I took a few minutes to search about hysteresis.
As this is a common problem in engineering, there are probably people - much smarter than me - who have solved this before.

Two searches in I came across this: https://github.com/lille-boy/hysteresis, the exact same assignment including tests.
It is always nice to find exactly the thing you need (don't forget to check the license if you can use the code!).
And I think being able to quickly find the right information is an important skill for an engineer.

I could just hand this in and be done with the assigment in 15mins.. but I also saw room for improvement.
Therefore I decided to do a review of this code and still create my version based on the initial design.

## Review
The code uses an array of structs for the threshold:

```C
const struct threshold thresholds[NB_THRESHOLDS]
```
I think this lookup table is a fine solution. 
It works great when you only have a few thresholds and is even better if you would like to individually tweak the thresholds.
For example, if you were reading out the voltage of a battery, which is not linear, you could adjust some thresholds to better fit the curve.
I would have left out the `level` value of the struct, as you could easily derive this from the index of the array.

The comparison with the previous input is smart:

```C
    if (input_percent >= prev_input_precent) {
```
It helps to detect which bound to check for a threshold, and could save some cycles when searching for the threshold.
However, the latter was not the case:

```C
    for(int i = 0; i < NB_THRESHOLDS; i++) {
```
Both loops start from 0 and go up to `NB_THRESHOLDS`, which makes this a simple linear search with O(n).
Since the `discrete_level` is also `static`, you could use this as starting point to search.
In addition, you can break out of the loop when the threshold is found, resulting in the following loop:

```C
        for (int i = discrete_level - 1; i >= 0; i--) {
            if (input_percent <= thresholds[i].low) {
                discrete_level = thresholds[i].level - 1;
            } else {
                break;
            }
        }
```
In the best case you find the threshold directly O(1), and on average this would be roughly O(n/2).

## My implementation
For my own implementation I wanted to make the code more reusable.
If you look at this ADC, it would have a resolution of 2 bits over a range of 100.
I would expect this range to be voltage, e.g. 5V or 3.3V.
The noise is in this assignment 2.5 in both directions.
In percentages this would be 10% of the threshold.

The implementation allows you to define the ADC bits, range, and noise in percentage; and generates a lookup table based on these values.
The code in `main.c` is now set to an ADC of 4 bits (16 thresholds), over a range of 1000mV, with 10% noise on each threshold.
Note that because of the use of integers, there can be slight rounding errors.
You could use floating point to calculate the thresholds more accurately, but in general it is preferred to use integers, as some microcontrollers might not have an FPU.

## Other optimizations
One further optimization that could be made is the search algorithm, especially if the ADC has a higher resolution like 16 bits.
You could implement binary search which checks for the threshold range, which would make it O(log n).
This would add some complexity to the code, and seems a bit overkill for this assignment.
