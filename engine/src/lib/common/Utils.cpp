#include "common/Utils.h"
#include "thread/CurrentThread.h"

#include <stdlib.h>

using namespace bling;
using namespace bling::utils;

int utils::random(int min, int max)
{
    if (min == max)
    {
        return min;
    }

    if ( min > max)
    {
        int temp = min;
        min = max;
        max = temp;
    }

    return min + static_cast<int>((static_cast<double>(max) - static_cast<double>(min)) * rand_r(&CurrentThread::t_randseed) / (RAND_MAX + 1.0));
}

bool utils::checkOdds(const int up, const int down)
{
    if (down < 1 || up < 1) return false;
    if ( up >= down) return true;

    return utils::random(1, down) <= up;
}

