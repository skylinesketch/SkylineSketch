#ifndef _BUCKET_H_
#define _BUCKET_H_

#include "item.h"
#include "slot.h"
#include "globalbucket.h"
#include <memory>
#include <vector>
#include <random>

// Vote, Prob, Sub
#define Sub 

static double _rand_double() {
    static std::default_random_engine e;
    std::uniform_real_distribution<double> unif(0, 1);
    return unif(e);
}

struct Vote_Anti { // if discarded, increase in the vote_anti
    int attribute_num;
    int *attributes;

    Vote_Anti(int _num) : attribute_num(_num) {
        attributes = new int [attribute_num];
        memset(attributes, 0, sizeof(int) * attribute_num);
    }

    ~Vote_Anti() {
        delete [] attributes;
    }

    void accumulate(Item &item) {
        for (int i = 0; i < attribute_num; ++i) 
            attributes[i] += item.attributes[i];
    }

    void clear() {
        memset(attributes, 0, sizeof(int) * attribute_num);
    }

    bool compare_value(Slot *slot, long long *attr_sum) { 
        // return true if substitute
        bool cmp = 0.0;
        for (int i = 0; i < attribute_num; ++i) {
            cmp += ((double)(attributes[i] - slot->attributes[i]) / attr_sum[i]);
        }
        return (cmp >= 0);
    }
};

class Bucket {
    bool *_is_skyline;
    GlobalBucket *global_bucket;
    Vote_Anti *vote_anti;
public:
    int size;
    Slot** slots;

    Bucket(int _size, int a_num, GlobalBucket *g_bucket) : 
      size(_size), global_bucket(g_bucket) {
        _is_skyline = new bool[size];
        slots = new Slot*[size];
        vote_anti = new Vote_Anti(a_num);
        for (int i = 0; i < size; ++i) {
            slots[i] = new Slot(a_num);
        }
    }
    ~Bucket() {
        for (int i = 0; i < size; ++i)
            delete slots[i];
        delete [] slots;
        delete [] _is_skyline;
        delete vote_anti;
    }
private:
    void insert_to_slot(Item &item, int index) {
        slots[index]->accumulate(item);
        for (int i = 0; i < size; ++i) {
            if (i == index) continue;
            if (!_is_skyline[i]) continue;
            if (slots[i]->dominateBy(slots[index]))
                _is_skyline[i] = false;
        }
        for (int i = 0; i < size; ++i) {
            if (i == index) continue;
            if (slots[index]->dominateBy(slots[i])) {
                _is_skyline[index] = false;
                return;
            }
        }
        _is_skyline[index] = true;
    }

    int get_substitute_pos() {
        // first, find empty slot
        for (int i = 0; i < size; ++i) {
            if (slots[i]->isEmpty())
                return i;
        }

        // next, find the farest slot
        double max_dist = 0.0;
        int pos = -1;
        for (int i = 0; i < size; ++i) {
            if (_is_skyline[i]) continue;
            double t = global_bucket->get_distance(slots[i]);
            if (max_dist < t) {
                max_dist = t;
                pos = i;
            }
        }
        // how to do if pos == -1?
        // assert(pos != -1);
        return pos;
    }

    double get_substitute_prob(Item &item, Slot *slot, long long *attr_sum) {
        // new / (new + old)
        double a = 0.0, b = 0.0;
        for (int i = 0; i < item.attribute_num; ++i) {
            if (attr_sum[i] == 0) continue;
            a += ((double)item.attributes[i] / attr_sum[i]);
            b += ((double)(item.attributes[i] + slot->attributes[i]) / attr_sum[i]);
        }
        // if b == 0?
        return (a / b);
    }
public:
    Slot* insert(Item &item) {
        int match_index = -1;
        for (int i = 0; i < size; ++i) {
            if (slots[i]->id == item.id) {
                match_index = i;
                break;
            }
        }

        if (match_index == -1) {
            // no match index
            int pos = get_substitute_pos();
            if (pos == -1) 
                return nullptr;
            if (slots[pos]->isEmpty()) {
                slots[pos]->id = item.id;
                memcpy(slots[pos]->attributes, item.attributes, item.attribute_num * sizeof(int));
            }

            #if defined(Vote)
            // substitute if the value of slot <= vote_anti's value
            if (vote_anti->compare_value(slots[pos], global_bucket->attr_sum)) {
                slots[pos]->id = item.id;
                memcpy(slots[pos]->attributes, item.attributes, item.attribute_num * sizeof(int));
                vote_anti->clear();
            }
            else {
                vote_anti->accumulate(item);
            }
            #elif defined(Prob)
            // substitute with probability
            if (_rand_double() < get_substitute_prob(item, slots[pos], global_bucket->attr_sum)) {
                slots[pos]->id = item.id;
                memcpy(slots[pos]->attributes, item.attributes, item.attribute_num * sizeof(int));
            }
            #elif defined(Sub)
            // substitute when reduced to 0
            if (slots[pos]->sub(item)) {
                slots[pos]->id = item.id;
                memcpy(slots[pos]->attributes, item.attributes, item.attribute_num * sizeof(int));
            }
            #else 
            // substitute like spacesaving
            slots[pos]->id = item.id;
            slots[pos]->accumulate(item);
            #endif

            return slots[pos];
        }
        else { // find match index
            insert_to_slot(item, match_index);
            return slots[match_index];
        }
        return nullptr;
    }

    void report(Slot* result[], int &result_size) {
        for (int i = 0; i < size; ++i) {
            if (_is_skyline[i]) {
                // result.push_back(slots[i]);
                result[result_size++] = slots[i];
            }
        }
    }

    int query(uint64_t id, int attr_index) {
        for (int i = 0; i < size; ++i) {
            if (slots[i]->id == id)
                return slots[i]->attributes[attr_index];
        }
        return 0;
    }
};

#endif	
