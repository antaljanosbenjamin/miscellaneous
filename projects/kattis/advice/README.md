# Advice

If you walk through a big city and try to find your way around, you might try asking people for directions. However, asking n people for directions might result in n different sets of directions. But you believe in the law of averages: if you consider everyone’s advice, then you will have a good idea of where to go by computing the average destination that they all lead to. You would also like to know how far off were the worst directions. You compute this as the maximum straight-line distance between each direction’s destination and the averaged destination. 

## Input
Input consists of up to 100 test cases. Each test case starts with an integer 1≤n≤20, which is the number of people you ask for directions. The following nn lines each has two things: your location when you meet the person (since you are walking around meeting people) and that person’s directions from where you are standing. Each person’s directions are built from instructions of the following form:
 * `start α`, where α is the initial direction you are facing in degrees (east is 0 degrees, north is 90 degrees).
 * `turn α`, where α is an angle in degrees you should turn. A positive αα indicates to turn to the left.
 * `walk x`, where x is a number of units to walk.

The ‘start’ instruction is always the first instruction, and only occurs at the beginning. Each person’s directions contain at most 2525 instructions. All numeric inputs are real numbers in the range [−1000,1000] with at most four digits past the decimal. Input ends when n is zero. 

## Output:
For each test case, print a line with the x and y coordinates of the average destination, followed by the distance between the worst directions and the averaged destination Answers should be accurate within 0.01 units.

## Sample Input:
```
3
87.342 34.30 start 0 walk 10.0  
2.6762 75.2811 start -45.0 walk 40 turn 40.0 walk 60  
58.518 93.508 start 270 walk 50 turn 90 walk 40 turn 13 walk 5  
2  
30 40 start 90 walk 5  
40 50 start 180 walk 10 turn 90 walk 5  
0  
```

## Sample Output:
```
97.1547 40.2334 7.63097  
30 45 0  
```