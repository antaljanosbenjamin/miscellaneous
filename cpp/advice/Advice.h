#include "Point.h"
#include <vector>
#include <cstddef>
#include <memory>
#include <map>

#ifndef ADVICE_H
#define ADVICE_H

typedef double Degree;

enum AdviceType
{
    START,
    WALK,
    TURN
};

struct LocationInfo
{
    Point pointCurrentLocation;
    Degree dDirection;
};

class Advice
{
public:
    Advice( AdviceType adviceTypeInitial );

    AdviceType getType()
    {
        return m_eAdviceType;
    }

    static bool registerCreateFunction( std::string strAdviceName, std::function<std::shared_ptr<Advice>( const std::string & )> fCreateFunction );
    static bool hasAdviceWithName( const std::string &strAdviceName );
    static std::shared_ptr<Advice> createAdvice( const std::string &strAdviceName, const std::string &strParam );

    virtual void apply( LocationInfo &locationInfo ) = 0;

private:
    static std::map<std::string, std::function<std::shared_ptr<Advice>( const std::string & )>> ms_mapCreateFunctions;

    AdviceType m_eAdviceType;
};

class StartAdvice : public Advice
{
public:
    StartAdvice( Degree dInitialDirection );

    static std::shared_ptr<Advice> createStartAdvice( const std::string &strParam );

    void apply( LocationInfo &locationInfo ) override;

private:
    Degree m_dStartDirection;

};

class TurnAdvice : public Advice
{
public:
    TurnAdvice( Degree dTurnAngle );

    static std::shared_ptr<Advice> createTurnAdvice( const std::string &strParam );

    void apply( LocationInfo &locationInfo ) override;

private:
    Degree m_dTurnAngle;
};

struct WalkAdvice : public Advice
{
public:
    WalkAdvice( double dWalkDistance );

    static std::shared_ptr<Advice> createWalkAdvice( const std::string &strParam );

    void apply( LocationInfo &locationInfo ) override;

private:
    double m_dWalkDistance;
};

struct PathInfo
{
    Point pointSource;
    std::vector<std::shared_ptr<Advice>> vectorAdvices;
    Point pointDestination;
};

class AdviceCalculator
{
public:
    AdviceCalculator( size_t sPathCount );

    void readDatas( std::istream &istreamReadFrom );
    void calculate();
    Point getAverageDestination();
    double getLongestDistance();

private:
    bool m_bCalculated;
    std::vector<std::shared_ptr<PathInfo>> m_vectorPaths;
    Point m_pointAverage;
    size_t m_sPathCount;
};


#endif // ADVICE_H
