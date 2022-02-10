#ifndef _UTILS_H_
#define _UTILS_H_

#include <set>
#include <vector>
#include <cstdint>
#include "item.h"
#include "strawman.h"
#include "skylinesketch.h"
#include <unordered_map>

void get_ground_truth(std::vector<Item> &dataset, std::set<uint64_t> &gt, int attribute_num,
       std::unordered_map<uint64_t, int> records[]);
void metrics(std::set<uint64_t> &gt, std::set<uint64_t> &result);
void ARE(std::set<uint64_t> &result, SkylineSketch &sketch, std::unordered_map<uint64_t, int> records[], int attribute_num);
void ARE(std::set<uint64_t> &result, Strawman &strawman, std::unordered_map<uint64_t, int> records[], int attribute_num);

#endif
