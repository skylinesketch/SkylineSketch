#ifndef _STRAWMAN_H_
#define _STRAWMAN_H_

#include <assert.h>
#include <climits>
#include <cstring>
#include <set>
#include <map>
#include <cstdlib>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <chrono>
#include "BOBHash32.h"
#include "item.h"

#define Sub

struct StrawmanSlot {
    uint64_t id;
    long attribute_sum; // sum of one attribute

    StrawmanSlot() : id(0), attribute_sum(0) {}
    StrawmanSlot(uint64_t _id) : id(_id), attribute_sum(0) {}
};

struct StrawmanBucket {
    int size;
    StrawmanSlot *slots;

    StrawmanBucket() : size(0), slots(NULL) {}

    ~StrawmanBucket() {
        delete slots;
    }

    void initialize(int _size) {
        size = _size;
        slots = new StrawmanSlot [size];
    }

    void insert(Item &item, int attr_index) {
        for (int i = 0; i < size; ++i) {
            if (slots[i].id == item.id) {
                slots[i].attribute_sum += (item.attributes[attr_index]);
                return;
            }
        }
        
        long min_sum = LONG_MAX;
        int min_index = -1;
        for (int i = 0; i < size; ++i) {
            if (min_sum > slots[i].attribute_sum) {
                min_sum = slots[i].attribute_sum;
                min_index = i;
            }
        }
        // assert(min_index != -1)
        if (slots[min_index].attribute_sum == 0) {
            slots[min_index].id = item.id;
            slots[min_index].attribute_sum = item.attributes[attr_index];
            return;
        }

        #if defined(Sub)
        slots[min_index].attribute_sum -= item.attributes[attr_index];
        if (slots[min_index].attribute_sum <= 0) {
            slots[min_index].id = item.id;
            slots[min_index].attribute_sum = item.attributes[attr_index];
        }
        #elif defined(SpaceSaving)
        slots[min_index].id = item.id;
        slots[min_index].attribute_sum += item.attributes[attr_index];
        #else
        abort();
        #endif
    }

    int query(uint64_t id) {
        for (int i = 0; i < size; ++i) {
            if (slots[i].id == id)
                return slots[i].attribute_sum;
        }
        return 0;
    }
};

class Strawman {
public:
    Strawman(int _attribute_num, int _bucket_num, int _bucket_size)
     : attribute_num(_attribute_num), bucket_num(_bucket_num), bucket_size(_bucket_size) {
        buckets = new StrawmanBucket* [attribute_num];
        hash = new BOBHash32(122);
        for (int i = 0; i < attribute_num; ++i) {
            buckets[i] = new StrawmanBucket [bucket_num];
            for (int j = 0; j < bucket_num; ++j) 
                buckets[i][j].initialize(bucket_size);
        }
    }

    ~Strawman() {
        for (int i = 0; i < attribute_num; ++i)
            delete [] buckets[i];
        delete [] buckets;
    }

    void insert(Item &item) {
        int hash_v = hash->run((char*)&(item.id),8) % bucket_num;
        for (int i = 0; i < attribute_num; ++i) {
            buckets[i][hash_v].insert(item, i);
        }
    }

    void report (std::set<uint64_t> &result) {
		struct candidate {
			std::vector<int> attributes;
			uint64_t id;
			int attribute_n;

			candidate(uint64_t _id, int attr_n) : id(_id), attribute_n(attr_n) {
				attributes.resize(attribute_n, 0);
			}

            candidate() : id (0) {}

			bool dominatedBy(const candidate &can) {
				for (int i = 0; i < attribute_n; ++i)
					if (attributes[i] >= can.attributes[i])
						return false;
				return true;
			}
		};

        //std::vector<candidate> candidates;
        candidate candidates[1000];
        int can_num = 0;
        std::map<uint64_t, int> id_idx;
        for (int i = 0; i < attribute_num; ++i) {
            for (int j = 0; j < bucket_num; ++j) {
                for (int k = 0; k < buckets[i][j].size; ++k) {
                    if (id_idx.count(buckets[i][j].slots[k].id) > 0) {
                        int idx = id_idx[buckets[i][j].slots[k].id];
                        candidates[idx].attributes[i] = buckets[i][j].slots[k].attribute_sum;
                    } else {
                        id_idx[buckets[i][j].slots[k].id] = can_num; // candidates.size();
				    	// candidates.emplace_back(candidate(buckets[i][j].slots[k].id, attribute_num));
				    	// candidates.back().attributes[i] = buckets[i][j].slots[k].attribute_sum;
                        candidates[can_num] = candidate(buckets[i][j].slots[k].id, attribute_num);
                        candidates[can_num++].attributes[i] = buckets[i][j].slots[k].attribute_sum;
				    }
                }
            }
        }
		
		// std::vector<bool> is_skyline;
		// is_skyline.resize(candidates.size(), true);
		// 
		// for (int i = 0; i < candidates.size(); ++i)
		// 	for (int j = 0; j < candidates.size(); ++j) {
		// 		if (candidates[i].dominatedBy(candidates[j])) {
		// 			is_skyline[i] = false;
		// 			break;
		// 		}
		// 	}
		// 
		// for (int i = 0; i < candidates.size(); ++i) {
		// 	if (is_skyline[i]) {
		// 		result.insert(candidates[i].id);
		// 	}
		// }

        // BNL
        std::set<int> skyline_index;
        for (int i = 0; i < can_num; ++i) {
            bool flag = true;
            for (auto iter = skyline_index.begin(); iter != skyline_index.end(); ) {
                auto current = iter++;
                if (candidates[i].dominatedBy(candidates[*current])) {
                    flag = false;
                    break;
                }
                if (candidates[*current].dominatedBy(candidates[i])) {
                    skyline_index.erase(current);
                }
            }
            if (flag)
                skyline_index.insert(i);
        }
	
        for (auto i : skyline_index)
            result.insert(candidates[i].id);

    }

    int query(uint64_t id, int index) {
        int hash_v = hash->run((char*)&id, 8) % bucket_num;
        return buckets[index][hash_v].query(id);
    }
        
private:

    int attribute_num;
    int bucket_num;
    int bucket_size;
    BOBHash32 *hash;
    StrawmanBucket **buckets; // buckets[attribute_num][bucket_num]

};

#endif // _SPACE
