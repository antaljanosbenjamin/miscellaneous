#include "Advice.hpp"

#include <algorithm>
#include <iostream>
#include <istream>
#include <sstream>
#include <unordered_map>

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::unordered_map<std::string, std::function<std::shared_ptr<Advice>(const std::string &)>> createFunctions;

void MakeUpper(std::string &str) {
  transform(str.begin(), str.end(), str.begin(), [](char c) { return static_cast<char>(toupper(c)); });
}

bool Advice::registerCreateFunction(std::string adviceName, const AdviceCreatorFunc &creatorFunction) {
  MakeUpper(adviceName);
  if (createFunctions.find(adviceName) != createFunctions.end()) {
    return false;
  }

  createFunctions.emplace(adviceName, creatorFunction);

  return true;
}

std::shared_ptr<Advice> Advice::createAdvice(const std::string &adviceName, const std::string &param) {
  auto upperName = adviceName;
  MakeUpper(upperName);
  const auto &createFunction = createFunctions[upperName];
  return createFunction(param);
}

Advice::Advice(AdviceType adviceTypeInitial)
  : adviceType(adviceTypeInitial) {
}

StartAdvice::StartAdvice(Degree startDirection)
  : Advice(AdviceType::Start)
  , startDirection(startDirection) {
}

TurnAdvice::TurnAdvice(Degree turnAngle)
  : Advice(AdviceType::Turn)
  , turnAngle(turnAngle) {
}

WalkAdvice::WalkAdvice(double walkDistance)
  : Advice(AdviceType::Walk)
  , walkDistance(walkDistance) {
}

void StartAdvice::apply(LocationInfo &locationInfo) {
  locationInfo.direction = startDirection;
}

std::shared_ptr<Advice> StartAdvice::createStartAdvice(const std::string &param) {
  auto startDirection = stod(param);
  return std::shared_ptr<Advice>(new StartAdvice{startDirection});
}

void TurnAdvice::apply(LocationInfo &locationInfo) {
  locationInfo.direction += turnAngle;
}

std::shared_ptr<Advice> TurnAdvice::createTurnAdvice(const std::string &param) {
  auto turnAngle = stod(param);
  return std::shared_ptr<Advice>(new TurnAdvice{turnAngle});
}

void WalkAdvice::apply(LocationInfo &locationInfo) {
  Point walkVector{walkDistance, 0.0};
  RotateByDegree(walkVector, locationInfo.direction);
  locationInfo.currentLocation += walkVector;
}

std::shared_ptr<Advice> WalkAdvice::createWalkAdvice(const std::string &param) {
  double dWalkDistance = stod(param);
  return std::shared_ptr<Advice>(new WalkAdvice{dWalkDistance});
}

AdviceCalculator::AdviceCalculator(size_t pathCount)
  : calculated(false)
  , paths()
  , averagePoint()
  , pathCount(pathCount) {

  Advice::registerCreateFunction("start", AdviceCreatorFunc(StartAdvice::createStartAdvice));
  Advice::registerCreateFunction("turn", AdviceCreatorFunc(TurnAdvice::createTurnAdvice));
  Advice::registerCreateFunction("walk", AdviceCreatorFunc(WalkAdvice::createWalkAdvice));
}

void AdviceCalculator::readDatas(std::istream &is) {
  if (!paths.empty()) {
    return;
  }

  std::string pathLine;
  std::string strNextWord;

  for (auto sPathPos = 0U; sPathPos < pathCount; ++sPathPos) {
    std::getline(is, pathLine);
    std::stringstream ss(pathLine);

    ss >> strNextWord;

    PathInfo newPathInfo;
    newPathInfo.source.x = stod(strNextWord);
    ss >> strNextWord;
    newPathInfo.source.y = stod(strNextWord);
    paths.push_back(std::move(newPathInfo));

    while (ss >> strNextWord) {
      auto adviceName = strNextWord;
      ss >> strNextWord;
      auto advice = Advice::createAdvice(adviceName, strNextWord);
      paths.back().advices.push_back(Advice::createAdvice(adviceName, strNextWord));
    }
  }
}

void AdviceCalculator::calculate() {
  Point pointSum{0, 0};

  for (auto &pathInfo: paths) {
    LocationInfo locationInfo{pathInfo.source, 0};
    for (auto &spAdvice: pathInfo.advices) {
      spAdvice->apply(locationInfo);
    }
    pathInfo.destination = locationInfo.currentLocation;
    pointSum += pathInfo.destination;
  }

  averagePoint = Point{pointSum.x / paths.size(), pointSum.y / paths.size()};
  calculated = true;
}

Point AdviceCalculator::getAverageDestination() {
  if (calculated == false) {
    calculate();
  }

  return averagePoint;
}

double AdviceCalculator::getLongestDistance() {
  if (calculated == false) {
    calculate();
  }

  double dWorstDirectionDistance = GetLength((averagePoint - paths[0].destination));

  for (const auto &pathInfo: paths) {
    double dActualDirectionDistance = GetLength(averagePoint - pathInfo.destination);

    if (dActualDirectionDistance > dWorstDirectionDistance) {
      dWorstDirectionDistance = dActualDirectionDistance;
    }
  }

  return dWorstDirectionDistance;
}
