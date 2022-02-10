#include "utils.h"
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

void get_ground_truth(vector<Item> &dataset, set<uint64_t> &gt, int attribute_num,
      unordered_map<uint64_t, int> records[]) {
    std::set<uint64_t> players_set;
    std::vector<uint64_t> players;

    for (auto &data : dataset) {
        for (int i = 0; i < attribute_num; ++i) {
            if (records[i].count(data.id) == 0) {
                records[i][data.id] = data.attributes[i];
            }
            else {
                records[i][data.id] += data.attributes[i];
                assert(records[i][data.id] >= 0);
            }
        }
        players_set.insert(data.id);
    }

    for (auto p : players_set)
        players.push_back(p);

    int player_num = players.size();
    bool *filter = new bool [player_num];
    memset(filter, 0, sizeof(bool) * player_num);

    for (int i = 0; i < player_num; ++i) {
        for (int j = 0; j < player_num; ++j) {
            if (j == i) continue;
            if (filter[j]) continue;
            bool j_dom_i = true;
            for (int k = 0; k < attribute_num; ++k) {
                if (records[k][players[i]] >= records[k][players[j]]) {
                    j_dom_i = false;
                    break;
                }
            }
            if (j_dom_i) {
                filter[i] = true;
                break;
            }
        }
    }

    // cout << "skyline: " << endl;
    for (int i = 0; i < player_num; ++i)
        if (!filter[i]) {
            gt.insert(players[i]);
        }

    delete [] filter;

    std::cout << "ground truth: " << gt.size() << " skylines" << std::endl;
    std::cout << "data point number: " << player_num << std::endl;
}

void metrics(set<uint64_t> &gt, set<uint64_t> &result) {
    double precision, recall, f1;
    int match = 0;
    for (auto i : result) {
        if (gt.count(i) != 0)
            ++match;
    }

    precision = (double) match / result.size();
    recall = (double) match / gt.size();
    f1 = 2 * precision * recall / (precision + recall);

    cout << "  Precision: " << precision << endl;
    cout << "  Recall: " << recall << endl;
    cout << "  F1: " << f1 << endl;
}

void ARE(set<uint64_t> &result, SkylineSketch &sketch, unordered_map<uint64_t, int> records[], int attribute_num) {
    double are = 0;
    for (auto id : result) {
        double diff = 0;
        double sum = 0;
        for (int i = 0; i < attribute_num; ++i) {
            int sketch_result = sketch.query(id, i);
            diff += fabs(sketch_result - records[i][id]);
            sum += records[i][id];
        }
        are += (diff / sum);
    }
    are /= result.size();

    cout << "  ARE: " << are << endl;
}

void ARE(set<uint64_t> &result, Strawman &strawman, unordered_map<uint64_t, int> records[], int attribute_num) {
    double are = 0;
    for (auto id : result) {
        double diff = 0;
        double sum = 0;
        for (int i = 0; i < attribute_num; ++i) {
            int strawman_result = strawman.query(id, i);
            diff += fabs(strawman_result - records[i][id]);
            sum += records[i][id];
        }
        are += (diff / sum);
    }
    are /= result.size();

    cout << "  ARE: " << are << endl;
}
