// S. 135. There is a terrain map of an area to which we are thinking of spreading an N × N square grid. Each square of
// the grid is assigned an elevation value based on the map. We would like to traverse at least half of the squares (for
// odd N, the upper integer), we can pass from one square to an other adjacent square if the difference in the heights
// of the two squares is at most D. Find the smallest D value so that we can traverse at least half of the area if the
// crawl can start from any square.
//
// Input: The first line contains the number N; the following N lines will contain N numbers each: the j-th number in
// the i-th line indicates the height value of the square with (i,j) coordinates.
//
// Output: The smallest integer D that can be used to implement the traverse. Limits: 1≤N≤500, 0≤one square height≤106.
//
// Example input:
// 5
// 0 0 0 0 0
// 0 0 0 0 3
// 0 0 3 3 3
// 9 9 9 3 9
// 9 9 9 9 9
//
// Output for the example input:
// 3

#include <iostream>

#ifdef GENERATE_INPUT
#include <fstream>
#include <numeric>
#include <random>
#else
#include <algorithm>
#include <numeric>
#include <vector>
#endif

#ifndef GENERATE_INPUT

using Height = int32_t;

struct Edge {
  size_t from;
  Height difference;
  size_t to;
};

class Table {
public:
  explicit Table(std::istream &inputStream) {
    inputStream >> this->size;
    this->heights = std::vector<Height>(this->size * this->size, 0);
    this->components.reserve(this->size);
    for (auto i = 0u; i < this->size * this->size; ++i) {
      inputStream >> this->heights[i];
      this->components.push_back(std::vector<size_t>{i});
    }
    this->fieldToComponent.resize(this->size * this->size);
    std::iota(this->fieldToComponent.begin(), this->fieldToComponent.end(), 0);
  }

  size_t getD();

private:
  size_t getFieldId(size_t x, size_t y) const {
    return y * this->size + x;
  }

  void setDifference(size_t from, Height difference, size_t to) {
    edges.emplace_back(Edge{from, difference, to});
  }

  void calculateNorthSouthDifference() {
    for (auto x = 0u; x <= this->size - 1; ++x) {
      for (auto y = 1u; y <= this->size - 1; ++y) {
        auto fieldIdFrom = this->getFieldId(x, y);
        auto fieldIdTo = this->getFieldId(x, y - 1);
        this->setDifference(fieldIdFrom, abs(this->heights[fieldIdFrom] - this->heights[fieldIdTo]), fieldIdTo);
      }
    }
  }

  void calculateEastWestDifference() {
    for (auto x = 1u; x <= this->size - 1; ++x) {
      for (auto y = 0u; y <= this->size - 1; ++y) {
        auto fieldIdFrom = this->getFieldId(x, y);
        auto fieldIdTo = this->getFieldId(x - 1, y);
        this->setDifference(fieldIdFrom, abs(this->heights[fieldIdFrom] - this->heights[fieldIdTo]), fieldIdTo);
      }
    }
  }

  void calculateDifferences() {
    this->edges.reserve(this->size * 2 * (this->size - 1));
    this->calculateNorthSouthDifference();
    this->calculateEastWestDifference();
    std::sort(this->edges.begin(), this->edges.end(),
              [](const Edge &e1, const Edge &e2) { return e1.difference < e2.difference; });
  }

  void setNewestComponentSize(size_t newestComponentSize) {
    if (this->maxComponentSize < newestComponentSize) {
      this->maxComponentSize = newestComponentSize;
    }
  }

  bool hasBigEnoughComponent() const {
    static auto minSize = (this->size * this->size + 1) / 2;
    return this->maxComponentSize >= minSize;
  }

  bool isInSameComponent(size_t field1, size_t field2) const {
    return this->fieldToComponent[field1] == this->fieldToComponent[field2];
  }

  void mergeComponents(size_t componentIdToExtend, size_t componentIdToMerge) {
    auto &componentToExtend = this->components[componentIdToExtend];
    auto &componentToMerge = this->components[componentIdToMerge];
    for (auto &fieldID: componentToMerge) {
      this->fieldToComponent[fieldID] = componentIdToExtend;
    }

    componentToExtend.insert(componentToExtend.end(), componentToMerge.begin(), componentToMerge.end());
    componentToMerge.clear();
    this->setNewestComponentSize(componentToExtend.size());
  }

  std::vector<std::vector<size_t>> components;
  std::vector<size_t> fieldToComponent;
  std::vector<Height> heights;
  size_t size;
  size_t maxComponentSize = 0u;
  std::vector<Edge> edges;
};

size_t Table::getD() {
  this->calculateDifferences();

  auto i = 0u;
  while (!this->hasBigEnoughComponent()) {
    auto &e = this->edges[i++];
    if (!this->isInSameComponent(e.from, e.to)) {
      this->mergeComponents(this->fieldToComponent[e.from], this->fieldToComponent[e.to]);
    }
  }
  return this->edges[i - 1].difference;
}

int main() {
  Table t(std::cin);
  std::cout << t.getD() << '\n';
  return 0;
}
#else
int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Specify output file name as first command line argument!\n";
    return -1;
  }

  std::ofstream outputFile;
  outputFile.open(argv[1]);
  outputFile << "500\n";

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist6(1, 1000000);

  std::cout << dist6(rng) << std::endl;
  for (auto i = 0; i < 500; ++i) {
    for (auto j = 0; j < 500; ++j) {
      outputFile << dist6(rng) << ' ';
    }
    outputFile << '\n';
  }
  outputFile.close();

  return 0;
}

#endif