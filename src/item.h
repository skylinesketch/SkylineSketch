#ifndef _ITEM_H_
#define _ITEM_H_

#include <cstring>
#include <cstdint>

struct Item {
    uint64_t id;
    int attribute_num;
    int *attributes;

    Item(int _num) : attribute_num(_num) {
        attributes = new int [_num];
        memset(attributes, 0, sizeof(int) * _num);
    }

    ~Item() {
        delete [] attributes;
    }

    Item(const Item& item) {
        id = item.id;
        attribute_num = item.attribute_num;
        attributes = new int [attribute_num];
        memcpy(attributes, item.attributes, sizeof(int) * attribute_num);
    }
};

#endif
