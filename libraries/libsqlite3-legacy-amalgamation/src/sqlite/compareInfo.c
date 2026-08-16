#include "sqlite/_All.h"

const struct compareInfo globInfo = {'*', '?', '[', 0};

const struct compareInfo likeInfoNorm = {'%', '_', 0, 1};

const struct compareInfo likeInfoAlt = {'%', '_', 0, 0};
