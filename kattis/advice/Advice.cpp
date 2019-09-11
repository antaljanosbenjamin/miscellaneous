#include "Advice.hpp"

#include <algorithm>
#include <iostream>
#include <istream>
#include <sstream>
#include <unordered_map>

std::unordered_map<std::string, std::function<std::shared_ptr<Advice>(const std::string &)>> createFunctions;

void MakeUpper(std::string &str) {
  transform(str.begin(), str.end(), str.begin(), [](char c) { return static_cast<char>(toupper(c)); });
}

bool Advice::registerCreateFunction(std::string adviceName, AdviceCreatorFunc fCreateFunction) {
  MakeUpper(adviceName);
  if (createFunctions.find(adviceName) != createFunctions.end())
    return false;

  createFunctions.emplace(adviceName, fCreateFunction);

  return true;
}

bool Advice::hasAdviceWithName(const std::string &adviceName) {
  auto upperName = adviceName;
  MakeUpper(upperName);
  return createFunctions.find(upperName) != createFunctions.end();
}

std::shared_ptr<Advice> Advice::createAdvice(const std::string &adviceName, const std::string &param) {
  auto upperName = adviceName;
  MakeUpper(upperName);
  auto &createFunction = createFunctions[upperName];
  return createFunction(param);
}

Advice::~Advice(){}

Advice::Advice(AdviceType adviceTypeInitial)
  : adviceType(adviceTypeInitial) {
}

StartAdvice::StartAdvice(Degree startDirection)
  : Advice(START)
  , startDirection(startDirection) {
}

TurnAdvice::TurnAdvice(Degree dTurnAngle)
  : Advice(TURN)
  , turnAngle(dTurnAngle) {
}

WalkAdvice::WalkAdvice(double dWalkAdvice)
  : Advice(WALK)
  , walkDistance(dWalkAdvice) {
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
  if (paths.size() != 0)
    return;

  std::string pathLine;
  std::string strNextWord;

  for (auto sPathPos = 0u; sPathPos < pathCount; ++sPathPos) {
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
  if (calculated == false)
    calculate();

  return averagePoint;
}

double AdviceCalculator::getLongestDistance() {
  if (calculated == false)
    calculate();

  double dWorstDirectionDistance = GetLength((averagePoint - paths[0].destination));

  for (const auto &pathInfo: paths) {
    double dActualDirectionDistance = GetLength(averagePoint - pathInfo.destination);

    if (dActualDirectionDistance > dWorstDirectionDistance)
      dWorstDirectionDistance = dActualDirectionDistance;
  }

  return dWorstDirectionDistance;
}
