#include <iostream>
#include "Point.h"
#include "Advice.h"

#undef __STRICT_ANSI__

#include <cmath>
#include <vector>
#include <fstream>
#include <limits>

using namespace std;

int main()
{
    size_t sPathCount;
    cin >> sPathCount;
    while ( sPathCount > 0 )
    {
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        AdviceCalculator adviceCalculator( sPathCount );
        adviceCalculator.readDatas( cin );
        adviceCalculator.calculate();
        Point pointAverage = adviceCalculator.getAverageDestination();
        std::cout << pointAverage.dX << " " << pointAverage.dY << " " << adviceCalculator.getLongestDistance() << endl;

        cin >> sPathCount;
    }
}