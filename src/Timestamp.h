#pragma once

#include <iostream>
#include <string>
#include "comm_def.h"


namespace dwt {


class Timestamp {

public:
    Timestamp();

    explicit Timestamp(s64 microSec);

    static Timestamp now();

    std::string toString() const;

private:
    s64 time_s_; // ms

};

}