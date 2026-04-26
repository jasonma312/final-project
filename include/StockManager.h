#ifndef STOCKMANAGER_H
#define STOCKMANAGER_H

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include "FinancialAsset.h"

using std::string;
using std::vector;
using std::cout;

template <typename T>
class StockManager {
private:
    vector<T*> assets;

public:
    ~StockManager() {
        for (int i = 0; i < static_cast<int>(assets.size()); i++) {
            delete assets[i];
        }
        assets.clear();
    }

    void addAsset(T* asset) {
        if (asset != nullptr) {
            assets.push_back(asset);
        }
    }

    void removeAsset(const string& ticker) {
        for (int i = 0; i < static_cast<int>(assets.size()); i++) {
            if (assets[i]->getTicker() == ticker) {
                delete assets[i];
                assets.erase(assets.begin() + i);
                return;
            }
        }
        cout << "[StockManager] Ticker not found: " << ticker << "\n";
    }

    T* findByTicker(const string& ticker) const {
        for (int i = 0; i < static_cast<int>(assets.size()); i++) {
            if (assets[i]->getTicker() == ticker) {
                return assets[i];
            }
        }
        return nullptr;
    }

    void sortByAnnualReturn(int year) {
        std::sort(assets.begin(), assets.end(),
            [year](T* a, T* b) {
                // Cast to FinancialAsset* to invoke virtual dispatch correctly
                FinancialAsset* fa = static_cast<FinancialAsset*>(a);
                FinancialAsset* fb = static_cast<FinancialAsset*>(b);
                return fa->calculateAnnualReturn(year) > fb->calculateAnnualReturn(year);
            });
    }

    void sortByTicker() {
        std::sort(assets.begin(), assets.end(),
            [](T* a, T* b) {
                return a->getTicker() < b->getTicker();
            });
    }

    void printAll() const {
        if (assets.empty()) {
            cout << "[StockManager] No assets loaded.\n";
            return;
        }
        for (int i = 0; i < static_cast<int>(assets.size()); i++) {
            assets[i]->printSummary();
        }
    }


    int getCount() const {
        return static_cast<int>(assets.size());
    }

    T* getAsset(int index) const {
        return assets[index];
    }
};

#endif