#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <chrono>
#include <cmath>

#include "sim_helper.h"
#include "skylinesketch.h"
#include "strawman.h"
#include "utils.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cout << "usage: ./global_size_expr global_bucket_slotNum attribute_num dataset" << endl;
        return 0;
    }

    // parameter
    int bucket_slotNum = 8;
    char *uniform = argv[3];
    int attribute_num = atoi(argv[2]);
    int global_bucket_slotNum = atoi(argv[1]);

    // calculate other parameter based on memory cost
    int slot_size = sizeof(uint64_t) + attribute_num * sizeof(int);

    vector<Item> datasets;
    read_sim_data(datasets, uniform, attribute_num);

    set<uint64_t> ground_truth;
    unordered_map<uint64_t, int> records[attribute_num];
    get_ground_truth(datasets, ground_truth, attribute_num, records);

    int memory = 10;

    int bucket_num = (memory * 1024 - global_bucket_slotNum * slot_size -
    attribute_num * sizeof(long long)) / (slot_size * bucket_slotNum);

    SkylineSketch sketch(bucket_slotNum, bucket_num, attribute_num, global_bucket_slotNum);
        
    cout << "Global Bucket Size: " << global_bucket_slotNum << endl;

    auto start = chrono::steady_clock::now();
    for (auto &item : datasets) {
        sketch.insert(item);
    }
    auto end = chrono::steady_clock::now();
    chrono::duration<double> span = chrono::duration_cast<chrono::duration<double>>(end - start);
    cout << "  Throughput: " << datasets.size() / (1000 * 1000 * span.count()) << "Mips" << endl;
    
    auto query_start = chrono::steady_clock::now();
    set<uint64_t> sketch_result;
    sketch.report(sketch_result);
    auto query_end = chrono::steady_clock::now();
    chrono::duration<double> query_span = chrono::duration_cast<chrono::duration<double>>(query_end
    - query_start);
    cout << "   Latency(s): " << query_span.count() << endl;

    metrics(ground_truth, sketch_result);
    ARE(sketch_result, sketch, records, attribute_num);

    cout << endl;

    return 0;
}
