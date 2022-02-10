#ifndef _SLOT_H_
#define _SLOT_H_

#include "item.h"
#include <assert.h>
#include <cstdint>

struct Slot {
    uint64_t id;
    int attribute_num;
    int *attributes;

    Slot(int _num) : attribute_num(_num) {
        id = -1;
        attributes = new int[_num];
        memset(attributes, 0, sizeof(int) * _num);
    }
    ~Slot() {
        delete [] attributes;
    }
    void accumulate(Item &item) {
        assert(attribute_num == item.attribute_num);
        // assert(id == item.id);
        for (int i = 0; i < attribute_num; ++i) {
            attributes[i] += item.attributes[i];
	        assert(attributes[i] >= 0);
	    }
    }
    bool dominateBy(Slot *s) {
        for (int i = 0; i < attribute_num; ++i) {
            if (attributes[i] >= s->attributes[i])
                return false;
        }
        return true;
    }
    void Copy(Slot *s) {
        assert(attribute_num == s->attribute_num);
        id = s->id;
        for (int i = 0; i < attribute_num; ++i)
            attributes[i] = s->attributes[i];
    }

    bool isEmpty() {
        for (int i = 0; i < attribute_num; ++i) {
            if (attributes[i])
                return false;
        }
        return true;
    }

    bool sub(Item &item) {
        // if reduced to 0 or neg, return true
        for (int i = 0; i < attribute_num; ++i) {
	    assert(attributes[i] >= 0);
            attributes[i] -= item.attributes[i];
            if (attributes[i] < 0)
                return true;
        }
        return false;
    }
};

#endif