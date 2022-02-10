#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <chrono>

#include "sim_helper.h"
#include "skylinesketch.h"
#include "strawman.h"
#include "utils.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cout << "usage: ./performance memory attribute_num dataset" << endl;
        return 0;
    }

    // parameter
    int bucket_slotNum = 8;
    int global_bucket_slotNum = 16;

    int memory = atoi(argv[1]);
    int attribute_num = atoi(argv[2]);

    // calculate other parameter based on memory cost
    int slot_size = sizeof(uint64_t) + attribute_num * sizeof(int);
    int bucket_num = (memory * 1024 - global_bucket_slotNum * slot_size - attribute_num * sizeof(long long)) / (slot_size * bucket_slotNum);
    int strawman_num = memory * 1024 / (attribute_num * bucket_slotNum * (sizeof(long) + sizeof(uint64_t)));
    
    vector<Item> datasets;
    read_sim_data(datasets, argv[3], attribute_num);

    SkylineSketch sketch(bucket_slotNum, bucket_num, attribute_num, global_bucket_slotNum);
    Strawman strawman(attribute_num, strawman_num, bucket_slotNum);

    set<uint64_t> ground_truth;
    unordered_map<uint64_t, int> records[attribute_num];
    get_ground_truth(datasets, ground_truth, attribute_num, records);

    // --- skyline sketch ---
    cout << "Skyline Sketch: " << endl;

    auto start1 = chrono::steady_clock::now();
    for (auto &item : datasets) {
        sketch.insert(item);
    }
    auto end1 = chrono::steady_clock::now();
    chrono::duration<double> span1 = chrono::duration_cast<chrono::duration<double>>(end1 - start1);
    cout << "  Throughput: " << datasets.size() / (1000 * 1000 * span1.count()) << "Mips" << endl;

    auto query_start1 = chrono::steady_clock::now();
    set<uint64_t> sketch_result;
    sketch.report(sketch_result);
    auto query_end1 = chrono::steady_clock::now();
    chrono::duration<double> query_span1 =
      chrono::duration_cast<chrono::duration<double>>(query_end1 - query_start1);
    cout << "  Query Latency: " << query_span1.count() << endl;

    metrics(ground_truth, sketch_result);
    ARE(sketch_result, sketch, records, attribute_num);
    
    cout << endl;

    // --- Strawman solution ---
    cout << "Strawman Solution: " << endl;
    
    auto start2 = chrono::steady_clock::now();
    for (auto &item : datasets) {
        strawman.insert(item);
    }
    auto end2 = chrono::steady_clock::now();
    chrono::duration<double> span2 = chrono::duration_cast<chrono::duration<double>>(end2 - start2);
    cout << "  Throughput: " << datasets.size() / (1000 * 1000 * span2.count()) << "Mips" << endl;

    auto query_start2 = chrono::steady_clock::now();
    set<uint64_t> strawman_result;
    strawman.report(strawman_result);
    auto query_end2 = chrono::steady_clock::now();
    chrono::duration<double> query_span2 =
      chrono::duration_cast<chrono::duration<double>>(query_end2 - query_start2);
    cout << "  Query Latency: " << query_span2.count() << endl;


    metrics(ground_truth, strawman_result);
    ARE(strawman_result, strawman, records, attribute_num);

    return 0;
}
