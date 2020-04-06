#pragma once

#include <cmath>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "Point.hpp"

typedef double Degree;

enum class AdviceType { Start, Walk, Turn };

struct LocationInfo {
  Point currentLocation;
  Degree direction;
};

class Advice;

using AdviceCreatorFunc = std::function<std::shared_ptr<Advice>(const std::string &)>;

class Advice {
public:
  AdviceType getType() const {
    return adviceType;
  }

  static bool registerCreateFunction(std::string adviceName, const AdviceCreatorFunc &creatorFunction);

  static std::shared_ptr<Advice> createAdvice(const std::string &adviceName, const std::string &param);

  virtual void apply(LocationInfo &locationInfo) = 0;

  virtual ~Advice() = default;

protected:
  explicit Advice(AdviceType adviceTypeInitial);

private:
  AdviceType adviceType;
};

class StartAdvice : public Advice {
public:
  explicit StartAdvice(Degree startDirection);

  static std::shared_ptr<Advice> createStartAdvice(const std::string &param);

  void apply(LocationInfo &locationInfo) override;

private:
  Degree startDirection;
};

class TurnAdvice : public Advice {
public:
  explicit TurnAdvice(Degree turnAngle);

  static std::shared_ptr<Advice> createTurnAdvice(const std::string &param);

  void apply(LocationInfo &locationInfo) override;

private:
  Degree turnAngle;
};

struct WalkAdvice : public Advice {
public:
  explicit WalkAdvice(double walkDistance);

  static std::shared_ptr<Advice> createWalkAdvice(const std::string &param);

  void apply(LocationInfo &locationInfo) override;

private:
  double walkDistance;
};

struct PathInfo {
  Point source{0, 0};
  std::vector<std::shared_ptr<Advice>> advices{};
  Point destination{0, 0};
};

class AdviceCalculator {
public:
  explicit AdviceCalculator(size_t pathCount);

  void readDatas(std::istream &is);
  void calculate();
  Point getAverageDestination();
  double getLongestDistance();

private:
  bool calculated;
  std::vector<PathInfo> paths;
  Point averagePoint;
  size_t pathCount;
};
