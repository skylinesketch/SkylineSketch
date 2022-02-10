#ifndef _GLOBALBUCKET_H_
#define _GLOBALBUCKET_H_

#include <random>
#include <ctime>
#include <iostream>
#include <assert.h>
#include "item.h"

static inline int _rand_int() {
    srand(time(NULL));
    return rand();
}

class GlobalBucket {
    bool optimized;
    int attribute_num;
    int size;
    bool *_is_skyline;
public:
	long long *attr_sum;
    Slot **slots;

    GlobalBucket(int _size, int a_num, bool opt) : size(_size), attribute_num(a_num), optimized(opt) {
        attr_sum = new long long [a_num];
        slots = new Slot*[size];
        _is_skyline = new bool[size];
        memset(_is_skyline, 0, sizeof(bool)*size);
        for (int i = 0; i < size; ++i)
            slots[i] = new Slot(a_num);
    }

    ~GlobalBucket() {
        for (int i = 0; i < size; ++i)
            delete slots[i];
        delete [] slots;
        delete [] _is_skyline;
        delete [] attr_sum;
    }
private:
    inline int get_mul(int pos) {
        int result = 1;
        for (int i = 0; i < size; ++i)
            result *= slots[pos]->attributes[i];
        return result;
    }

    inline double get_manhattan_dis(int g_index, Slot *slot) {
        double dis = 0.0;
        for (int i = 0; i < attribute_num; ++i) {
            assert(attr_sum[i] > 0);
            if (optimized) {
                dis += ((double) fabs(slots[g_index]->attributes[i] - slot->attributes[i]) / attr_sum[i]);
            } else {
                dis += ((double) fabs(slots[g_index]->attributes[i] - slot->attributes[i]));
            }
        }
        return dis;
    }

    // return -1 if is dominated, may be adjusted?
    int get_substitute_pos(Slot *slot) {
        for (int i = 0; i < size; ++i) {
            if (slot->dominateBy(slots[i])) 
                return -1;
        }
        // for (int i = 0; i < size; ++i) {
        //     if (slots[i]->id == slot->id)
        //         return i;
        // }
        for (int i = 0; i < size; ++i) {
            if (!_is_skyline[i])
                return i;
        }
        for (int i = 0; i < size; ++i) {
            if (slots[i]->dominateBy(slot))
                return i;
        }
        
        // randomly select one bucket
        int pos;
        pos = _rand_int() % size;

        return pos;
    }
public:
    double get_distance(Slot *slot) {
        // if one in global bucket is dominated, return 0
        double min_dist = std::numeric_limits<double>::max();
        for (int i = 0; i < size; ++i) {
            if (!_is_skyline[i]) continue;
            if (slots[i]->dominateBy(slot))
                return 0;
            double t = get_manhattan_dis(i, slot);
            min_dist = t < min_dist ? t : min_dist;
        }
        return min_dist;
    }

    void insert(Slot *slot) {
        if (slot == nullptr)
            return;
        int pos = get_substitute_pos(slot);
        if (pos == -1)
            return;
        slots[pos]->Copy(slot);
        _is_skyline[pos] = true;
        for (int i = 0; i < size; ++i) {
            if (i == pos) continue;
            if (slots[i]->dominateBy(slots[pos]))
                _is_skyline[pos] = false;
        }
    }

    void accumulate_sum(Item &item) {
        for (int i = 0; i < attribute_num; ++i) {
            attr_sum[i] += item.attributes[i];
        }
    }

    void print() {
        std::cout << "global bucket: " << std::endl;
        for (int i = 0; i < size; ++i) {
            std::cout << slots[i]->id << " ";
            for (int j = 0; j < attribute_num; ++j)
                std::cout << slots[i]->attributes[j] << " ";
            std::cout << std::endl;
        }
    }
};

#endif
