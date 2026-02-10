
#ifndef DUMMY_H
#define DUMMY_H

class Dummy {
    int i;
public:
    Dummy(int i) : i(i){};
    ~Dummy(){};
    int get_int() {
        return i;
    };
};

#endif
