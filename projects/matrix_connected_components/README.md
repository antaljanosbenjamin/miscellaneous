# Matrix connected components

I would like to share a few thoughts about my solution and how I made the decisions.

First I wanted to do something graph specific connected components algorithm as I have some experience with graphs and graph algorithms. However I realized that in this case we have a very special case. The possible connections to one node (or field in the matrix) are well specified, only the four neighbor can be connected to a node. I did some research on the internet and found a very interesting article that gives a multi-threaded algorithm for the problem. However their conflicts handling seemed a bit too complicated. I remembered the disjoint-set data structure from [Kruskal's algorithm](https://en.wikipedia.org/wiki/Kruskal%27s_algorithm), which I think can be easier to implement and useful in more general situations. From the performance point of view I wouldn't like to say anything concrete without proper measurements, but I can image the performance might not degrade too much. Merging and moving big hash sets might be expensive.

I know you mentioned that it is of no use to anyone if the algorithm is taken from the Internet, but I think properly utilizing the knowledge available on the Internet is a plus regardless of the work. Therefore I took an inspiration from a well described algorithm and implemented it in C++ based on the paper, without looking on the Java implementation. I hope it is not a problem.

And of course, to solve a similar, very common problem in the best possible way, I think the best way is to either copy one of the most recent research paper (absolute cutting edge) or use a well built, already existing solution. Usually the people who wrote those libraries are much more suited for solving such a pure mathematical solution then I or most of software engineers. But that would take away the fun part of my task and also wouldn't provide that much value for you to evaluate.

## Build requirements

The Github workflow provides a good overview about the required packages. To build the executables you will need:
1. A C++20 compatible compiler, I checked (GCC 10.3.0, Clang 11, Clang 12, Microsoft Visual Studio 2019 and 2022)
2. CMake 3.11+
3. [Conan](https://conan.io/)

I assume you know how to acquire the first two. For Conan, you only need to install Conan with `pip`: `pip install conan`. After that some configuration is recommended:
```
conan profile new default --detect
conan config set general.revisions_enabled=1
```

The [revisions are not necessary](https://blog.conan.io/2019/04/01/New-conan-release-1-14.html), but in general it is a good practice to enable them to make `conan` more flexible.

After that the project can be build with CMake. From the root folder execute the following commands:
```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake -build . --parallel
ctest
```

Clang-Tidy can be enabled by passing `-DENABLE_CLANG_TIDY=ON` to the CMake generator command. I have tested and eliminated all of the warning for Clang-Tidy 11.

## Data structure to store the matrix

The first question was in which data structure should I store the matrix. The two big contester is a vector of integers which is mapped to be a 2D matrix, or an `<integer, integer> -> integer` map of some sort. To decide on this, I created a small benchmark (`epxeriments/unordered_maps`) to compare hash maps from [`robin-hood-hashing`](https://github.com/martinus/robin-hood-hashing) library, `std::unordered_map`, `std::map` and of course `std::vector`. On the performance side `std::vector` was the absolute fastest unsurprisingly. The memory consumption for `std::vector` is constant (`n * sizeof(integer)` ignoring the very small amount of "metadata"), but for the maps it heavily depends on the sparsity of the matrix. Each map has to store at least the coordinates and the value for a marked field (unless some advanced compression or special sparse matrix data structure), therefore the memory consumption is at least `3 * sizeof(integer)` for any map implementation. That means if more than the third of the matrix is marked, then `std::vector` has less memory requirement then maps. As the example matrix has 5/2 of its fields marked, I opted for using `std::vector`. Fortunately the solution is flexible enough to support map-based implementations also if the properties of the inputs would make that desirable.

## C++ standard to use

I know you are using C++14, but as the task didn't specified I opted for C++20. On my hobby projects I am using C++20 and concepts provides a very big improvement over SFINAE that I opted for use it. In my opinion for this solution C++20 means a real value. I hope it is not an issue.

## Mutli-threading

My initial plan was to create a multi-threaded solution, but I realized that I don't have time to properly test a thread pool implementation (and I didn't want to introduce another dependency just because of this). Thus I haven't implemented it, but the current implementation can be extended easily to support multi-threaded processing:
1. Extend `DisjointSet` to be able to merge multiple multiple of them into one. This is important to merge the results of separate initial labelling done on multiple matrix slices.
2. `MatrixSlice` can be used to run the initial a labelling phase easily in a multi-threaded manner by slicing up a matrix into lines or columns (or rectangles if that is better).
3. After the initial labelling, marching over the border of matrix slices and registering cross-border conflicts.
4. As a last step, the relabelling can be done in multiple threads, because only the matrix should be changed, but with slicing properly the data races can be avoided. The final `DisjointSet` data structure has constant member function to look-up the final label, so calling that from multiple threads wouldn't introduce any data races.

## Interface

There are two main things about the interface I provided:
1. signed size types: I agree with [Bjarne Stroustrup](https://www.open-std.org/JTC1/SC22/WG21/docs/papers/2019/p1428r0.pdf) in using signed number as size types. I think the strongest reason for this is most of the arithmetic operations on integer types result in a signed integer. Therefore it can protect use from a lot of casting. And yes, 2^63 bytes are more than I think a single computer can utilize as RAM nowadays.
2. I used template functions for the algorithms to provide flexibility to the users of the library. My assumption here was people might want to use it with signed/unsigned integers and they might not necessary want to use my `Matrix` class. In this case they can provide their own class that fulfills the `IsNumericalMatrixLike` concept and the algorithm would work with their class beautifully. This is similar (of course, on a much simpler level) to how algorithms are implemented in the standard library.
