#include "shipcontrol.hpp"

int main(int argc, char **argv)
{
    shipcontrol::ShipControl control;
    int ret = control.run();
    return ret;
}
