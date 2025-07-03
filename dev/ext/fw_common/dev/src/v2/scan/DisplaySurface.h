#pragma once

#include <v2/scan/Surface1d.h>

class Location1d;
class AScan;

class DisplaySurface : public Surface1d
{
    Q_OBJECT

public:

    DisplaySurface(Location1d* p_pLocation1d);

    qint32 pushAScan(AScan* p_pAScan); //call this function to redraw with more resolution the bscan

};

