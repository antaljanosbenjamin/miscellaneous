#include "Advice.h"
#include <istream>
#include <iostream>

#undef __STRICT_ANSI__

#include <cmath>
#include <algorithm>
#include <sstream>

using namespace std;

map<string, function<shared_ptr<Advice>( const std::string & )>>  Advice::ms_mapCreateFunctions;

bool Advice::registerCreateFunction( string strAdviceName, function<shared_ptr<Advice>( const std::string & )> fCreateFunction )
{
    transform( strAdviceName.begin(), strAdviceName.end(), strAdviceName.begin(), ::toupper );

    if ( ms_mapCreateFunctions.find( strAdviceName ) != ms_mapCreateFunctions.end())
        return false;

    ms_mapCreateFunctions.insert( pair<string, function<shared_ptr<Advice>( const std::string & )>>( strAdviceName, fCreateFunction ));

    return true;
}

bool Advice::hasAdviceWithName( const string &strAdviceName )
{
    string strUpperName = strAdviceName;
    transform( strUpperName.begin(), strUpperName.end(), strUpperName.begin(), ::toupper );
    return ms_mapCreateFunctions.find( strUpperName ) != ms_mapCreateFunctions.end();
}

shared_ptr<Advice> Advice::createAdvice( const string &strAdviceName, const string &strParam )
{
    string strUpperName = strAdviceName;
    transform( strUpperName.begin(), strUpperName.end(), strUpperName.begin(), ::toupper );
    function<shared_ptr<Advice>( const std::string & )> &fCreateFunction = ms_mapCreateFunctions[strUpperName];
    return fCreateFunction( strParam );
}

Advice::Advice( AdviceType adviceTypeInitial )
        : m_eAdviceType( adviceTypeInitial )
{}

StartAdvice::StartAdvice( Degree dStartDirection )
        : Advice( START ), m_dStartDirection( dStartDirection )
{}

TurnAdvice::TurnAdvice( Degree dTurnAngle )
        : Advice( TURN ), m_dTurnAngle( dTurnAngle )
{}

WalkAdvice::WalkAdvice( double dWalkAdvice )
        : Advice( WALK ), m_dWalkDistance( dWalkAdvice )
{}

void StartAdvice::apply( LocationInfo &locationInfo )
{
    locationInfo.dDirection = m_dStartDirection;
}

shared_ptr<Advice> StartAdvice::createStartAdvice( const string &strParam )
{
    double dStartDirection = stod( strParam );
    return shared_ptr<Advice>( new StartAdvice{ dStartDirection } );
}

void TurnAdvice::apply( LocationInfo &locationInfo )
{
    locationInfo.dDirection += m_dTurnAngle;
}

shared_ptr<Advice> TurnAdvice::createTurnAdvice( const string &strParam )
{
    double dTurnAngle = stod( strParam );
    return shared_ptr<Advice>( new TurnAdvice{ dTurnAngle } );
}

void WalkAdvice::apply( LocationInfo &locationInfo )
{
    Point pointWalkVector{ m_dWalkDistance, 0.0 };
    pointWalkVector.rotateByDegree( locationInfo.dDirection );
    locationInfo.pointCurrentLocation += pointWalkVector;
}

shared_ptr<Advice> WalkAdvice::createWalkAdvice( const string &strParam )
{
    double dWalkDistance = stod( strParam );
    return shared_ptr<Advice>( new WalkAdvice{ dWalkDistance } );
}

AdviceCalculator::AdviceCalculator( size_t sPathCount )
        : m_bCalculated( false ), m_sPathCount( sPathCount )
{
    function<shared_ptr<Advice>( const std::string & )> fStartAdviceCreator = &StartAdvice::createStartAdvice;
    Advice::registerCreateFunction( "start", function<shared_ptr<Advice>( const std::string & )>( StartAdvice::createStartAdvice ));
    Advice::registerCreateFunction( "turn", function<shared_ptr<Advice>( const std::string & )>( TurnAdvice::createTurnAdvice ));
    Advice::registerCreateFunction( "walk", function<shared_ptr<Advice>( const std::string & )>( WalkAdvice::createWalkAdvice ));
}

void AdviceCalculator::readDatas( istream &istreamReadFrom )
{
    if ( m_vectorPaths.size() != 0 )
        return;

    for ( size_t sPathPos = 0; sPathPos < m_sPathCount; ++sPathPos )
    {
        string strPathLine;
        getline( istreamReadFrom, strPathLine );
        stringstream stringStream( strPathLine );

        string strNextWord;
        stringStream >> strNextWord;

        shared_ptr<PathInfo> spNewPathInfo = shared_ptr<PathInfo>( new PathInfo());
        spNewPathInfo->pointSource.dX = stod( strNextWord );
        stringStream >> strNextWord;
        spNewPathInfo->pointSource.dY = stod( strNextWord );
        m_vectorPaths.push_back( spNewPathInfo );


        while ( stringStream >> strNextWord )
        {
            string strAdviceName = strNextWord;
            stringStream >> strNextWord;
            shared_ptr<Advice> spAdvice = Advice::createAdvice( strAdviceName, strNextWord );
            m_vectorPaths.back()->vectorAdvices.push_back( spAdvice );
        }
    }
}

void AdviceCalculator::calculate()
{
    Point pointSum{ 0, 0 };

    for ( shared_ptr<PathInfo> spPathInfo : m_vectorPaths )
    {
        LocationInfo locationInfo{ spPathInfo->pointSource, 0 };
        for ( shared_ptr<Advice> spAdvice : spPathInfo->vectorAdvices )
        {
            spAdvice->apply( locationInfo );
        }
        spPathInfo->pointDestination = locationInfo.pointCurrentLocation;
        pointSum += spPathInfo->pointDestination;
    }

    m_pointAverage = Point{ pointSum.dX / m_vectorPaths.size(), pointSum.dY / m_vectorPaths.size() };
    m_bCalculated = true;
}

Point AdviceCalculator::getAverageDestination()
{
    if ( m_bCalculated == false )
        calculate();

    return m_pointAverage;
}

double AdviceCalculator::getLongestDistance()
{
    if ( m_bCalculated == false )
        calculate();

    double dWorstDirectionDistance = ( m_pointAverage - m_vectorPaths[0]->pointDestination ).getLength();

    for ( shared_ptr<PathInfo> spPathInfo : m_vectorPaths )
    {
        double dActualDirectionDistance = ( m_pointAverage - spPathInfo->pointDestination ).getLength();

        if ( dActualDirectionDistance > dWorstDirectionDistance )
            dWorstDirectionDistance = dActualDirectionDistance;
    }

    return dWorstDirectionDistance;
}
