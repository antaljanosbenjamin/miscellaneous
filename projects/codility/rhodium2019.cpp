
#include <algorithm>
#include <list>
#include <unordered_set>
#include <vector>

struct Node {
  int id{-1};
  std::unordered_set<int> connections{};
};

using NodesVector = std::vector<Node>;

std::vector<std::vector<std::unordered_set<int>>> getRoutes(std::vector<int> roads) {
  const auto nCities = static_cast<int>(roads.size());

  std::vector<std::vector<std::unordered_set<int>>> routes(
      nCities, std::vector<std::unordered_set<int>>(nCities, std::unordered_set<int>{}));
  NodesVector nodes(nCities, Node{});

  const auto addBidirectionalConnection = [&nodes](int from, int to) {
    nodes[from].connections.insert(to);
    nodes[to].connections.insert(from);
  };

  for (auto index = 0; index < nCities; ++index) {
    nodes[index].id = index;
    if (index != roads[index]) {
      addBidirectionalConnection(index, roads[index]);
    }
  }

  const auto fillRoutesForNode = [&routes, &nodes](const int startId) {
    std::list<int> nodeIdsToProcess;
    nodeIdsToProcess.push_back(startId);

    const auto isRouteUnknown = [&routes](const int from, const int to) {
      return from != to && routes[from][to].empty();
    };

    while (!nodeIdsToProcess.empty()) {
      const auto nodeIdToProcess = *nodeIdsToProcess.begin();
      nodeIdsToProcess.pop_front();
      for (auto childId: nodes[nodeIdToProcess].connections) {
        const auto &routeToParent = routes[startId][nodeIdToProcess];
        if (isRouteUnknown(startId, childId)) {
          auto &routeToChild = routes[startId][childId];
          routeToChild.insert(routeToParent.begin(), routeToParent.end());
          routeToChild.insert(childId);
          if (routeToParent.empty()) {
            routeToChild.insert(startId);
          }
          nodeIdsToProcess.push_back(childId);
        }
      }
    }
  };

  for (auto id = 0; id < nCities; ++id) {
    fillRoutesForNode(id);
  }

  return routes;
}

int solution(const std::vector<int> &T) {
  const auto routes = getRoutes(T);

  const auto getTransitCities = [&routes](const int startDate, const int endDate) {
    std::unordered_set<int> transitCities;
    for (auto date = startDate; date < endDate; ++date) {
      const auto &route = routes[date][date + 1];
      transitCities.insert(route.begin(), route.end());
    }
    return transitCities;
  };

  auto nPossibleVacations{0};
  const auto nCities = static_cast<int>(T.size());
  for (auto startId = 0; startId < nCities; ++startId) {
    for (auto endId = startId; endId < nCities; ++endId) {
      const auto transitCities = getTransitCities(startId, endId);
      const auto isVisited = [&startId, &endId](const int cityId) { return startId <= cityId && cityId <= endId; };
      if (std::all_of(transitCities.begin(), transitCities.end(), isVisited)) {
        nPossibleVacations++;
      }
    }
  }
  return nPossibleVacations;
}

int main() {
  solution(std::vector{2, 0, 2, 2, 1, 0});
}