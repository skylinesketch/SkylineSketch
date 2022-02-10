#ifndef _SKYLINESKETCH_H_
#define _SKYLINESKETCH_H_

#include <assert.h>
#include <climits>
#include <cstring>
#include <set>
#include <chrono>
#include "BOBHash32.h"
#include "item.h"
#include "bucket.h"
#include "globalbucket.h"

class SkylineSketch {
public:
    SkylineSketch(int b_size, int b_num, int a_num, int g_size, bool _opt=true) : 
      bucket_num(b_num), attribute_num(a_num), optimized(_opt) {
        hash = new BOBHash32(222);
        bucket_size = b_size;
        buckets = new Bucket*[b_num];
        global_bucket = new GlobalBucket(g_size, attribute_num, optimized);
        for (int i = 0; i < b_num; ++i)
            buckets[i] = new Bucket(b_size, attribute_num, global_bucket);
    }
    ~SkylineSketch() {
        if (global_bucket)
            delete global_bucket;
        for (int i = 0; i < bucket_num; ++i)
            delete buckets[i];
        delete [] buckets;
        delete hash;
    }

    void insert(Item &item) {
        global_bucket->accumulate_sum(item);
        int hash_v = hash->run((char *)&(item.id), 8) % bucket_num;
        Slot *s = buckets[hash_v]->insert(item);
        global_bucket->insert(s);
    }

    void report(std::set<uint64_t> &result) {
        // std::vector<Slot*> extract;
        // extract.reserve(bucket_num * bucket_size);
        Slot* extract[1000];
        int extract_size = 0;
        for (int i = 0; i < bucket_num; ++i)
            // buckets[i]->report(extract);
            buckets[i]->report(extract, extract_size);
        // bool *not_skyline = new bool [extract.size()];
        // memset(not_skyline, 0, sizeof(bool) * extract.size());
        // for (int i = 0; i < extract.size(); ++i) {
        //     for (int j = 0; j < extract.size(); ++j) {
        //         if (j == i) continue;
        //         if (extract[i]->dominateBy(extract[j])) {
        //             not_skyline[i] = true;
        //             break;
        //         }
        //     }
        // }
        // for (int i = 0; i < extract.size(); ++i)
        //     if (!not_skyline[i])
        //         result.insert(extract[i]->id);
        // delete [] not_skyline;

        // BNL
        std::set<int> skyline_index;
        for (int i = 0; i < extract_size; ++i) {
            bool flag = true;
            for (auto iter = skyline_index.begin(); iter != skyline_index.end(); ) {
            auto current = iter++;
            if (extract[i]->dominateBy(extract[*current])) {
                flag = false;
                break;
            }
            if (extract[*current]->dominateBy(extract[i])) {
                skyline_index.erase(current);
            }
        }
        if (flag)
            skyline_index.insert(i);
    }
    
    for (auto i : skyline_index)
        result.insert(extract[i]->id);
    }

    void report_global_bucket() {
        global_bucket->print();
    }

    int query(uint64_t id, int attr_index) {
        int hash_v = hash->run((char*)&id, 8) % bucket_num;
        return buckets[hash_v]->query(id, attr_index);
    }


private:
    bool optimized;
    int attribute_num;
    int bucket_num;
    int bucket_size;
    Bucket **buckets;
    GlobalBucket *global_bucket;
    BOBHash32 *hash;
};

#endif
