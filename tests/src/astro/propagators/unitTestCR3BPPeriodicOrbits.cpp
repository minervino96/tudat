/*    Copyright (c) 2010-2019, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>
#include <boost/bind/bind.hpp>
using namespace std::placeholders;

#include <boost/make_shared.hpp>
#include <memory>

#include "tudat/io/basicInputOutput.h"
#include "tudat/simulation/propagation_setup/createCR3BPPeriodicOrbits.h"
#include "tudat/math/integrators/createNumericalIntegrator.h"
#include "tudat/astro/gravitation/librationPoint.h"

namespace tudat
{

namespace unit_tests
{

using namespace tudat;
using namespace propagators;
using namespace numerical_integrators;
using namespace circular_restricted_three_body_problem;

BOOST_AUTO_TEST_SUITE( test_cr3bp_periodic_orbits )

BOOST_AUTO_TEST_CASE( testCr3bpPeriodicOrbits )
{
    double minimumStepSize   = std::numeric_limits<double>::epsilon( ); // 2.22044604925031e-16
    double maximumStepSize   = 100.0;//std::numeric_limits<double>::infinity( ); // 2.22044604925031e-16

    const double relativeErrorTolerance = 1.0E-10;
    const double absoluteErrorTolerance = 1.0E-14;

    std::shared_ptr< IntegratorSettings< double > > integratorSettings =
            std::make_shared< RungeKuttaVariableStepSizeSettings< > >
            ( 0.0, 1.0E-5,
              rungeKutta87DormandPrince, minimumStepSize, maximumStepSize,
              relativeErrorTolerance, absoluteErrorTolerance );


    double massParameter = computeMassParameter( 3.986004418E14, 1.32712440018e20 / ( 328900.56 * ( 1.0 + 81.30059 ) ) );

    int maximumDifferentialCorrections = 25;
    double maximumPositionDeviation = 1.0E-12;
    double maximumVelocityDeviation = 1.0E-12;

    int maximumNumberOfOrbits = 10000;
    double maximumEigenvalueDeviation = 1.0E-3;

    std::string baseOutputFolder = "/home/dominic/Software/periodicOrbitResults/";

    input_output::writeMatrixToFile( ( Eigen::MatrixXd( 1, 1 ) << massParameter ).finished( ), "massParameter.dat", 16, baseOutputFolder );

    for( int j = 0; j < 2; j++ )
    {
        for( int k = 0; k < 3; k++ )
        {
            int librationPointNumber = j + 1;
            CR3BPPeriodicOrbitTypes orbitType = static_cast< CR3BPPeriodicOrbitTypes >( k );
            CR3BPPeriodicOrbitGenerationSettings orbitSettings(
                        massParameter, orbitType, librationPointNumber,maximumDifferentialCorrections,
                        maximumPositionDeviation, maximumVelocityDeviation, maximumNumberOfOrbits, maximumEigenvalueDeviation );

            double amplitudeFirstGuess;
            std::pair< Eigen::Vector6d, double > periodicOrbitInitialGuess;
            std::vector< CR3BPPeriodicOrbitConditions > periodicOrbits;

            for( int i = 0; i < 2; i ++ )
            {
                amplitudeFirstGuess = initializeEarthMoonPeriodicOrbitAmplitude(
                            librationPointNumber, orbitType, i );
                periodicOrbitInitialGuess = richardsonApproximationLibrationPointPeriodicOrbit(
                            massParameter, orbitType, librationPointNumber, amplitudeFirstGuess );
                periodicOrbits.push_back( createCR3BPPeriodicOrbit(
                                              periodicOrbitInitialGuess.first, periodicOrbitInitialGuess.second,
                                              orbitSettings, integratorSettings ) );
            }

            createCR3BPPeriodicOrbitsThroughNumericalContinuation(
                        periodicOrbits, integratorSettings, orbitSettings );

            //            for( unsigned int i = 0; i < periodicOrbits.size( ); i++ )
            //            {
            //                Eigen::Vector6d testInitialState = periodicOrbits.at( i ).initialState_;
            //                double propagationTime = periodicOrbits.at( i ).orbitPeriod_;

            //                std::map< double, Eigen::Vector6d > testStateHistory =
            //                        performCR3BPIntegration(
            //                        integratorSettings, massParameter, testInitialState, propagationTime, true, false );
            //                BOOST_CHECK_CLOSE_FRACTION( testStateHistory.begin( )->first, propagationTime, 1.0E-14 );

            //                Eigen::Vector6d stateDifference = testInitialState - testStateHistory.begin( )->second;
            //                for( unsigned int l = 0; l < 6; l++ )
            //                {
            //                    BOOST_CHECK_SMALL( std::fabs( stateDifference( l ) ), 1.0E-10 );
            //                }

            std::map< double, Eigen::VectorXd > initialStates;
            for( unsigned int i = 0; i < periodicOrbits.size( ); i++ )
            {
                Eigen::Vector6d testInitialState = periodicOrbits.at( i ).initialState_;
                double propagationTime = periodicOrbits.at( i ).orbitPeriod_;
                initialStates[ propagationTime ] = testInitialState;
            }

            std::string outputFolder = baseOutputFolder + "/L" + std::to_string( j + 1 ) + "/" + getPeriodicOrbitName( orbitType ) + "/";
            input_output::writeDataMapToTextFile(
                        initialStates, "initial_states.dat", outputFolder );
//            for( unsigned int i = 0; i < periodicOrbits.size( ); i++ )
//            {

//                std::map< double, Eigen::Vector6d > currentOrbit = propagatePeriodicOrbit(
//                            periodicOrbits.at( i ), integratorSettings );
//                input_output::writeDataMapToTextFile(
//                            currentOrbit, "periodic_orbit_" +
//                            std::to_string( i ) + ".dat", outputFolder );

//            }
        }
    }
}



BOOST_AUTO_TEST_SUITE_END( )

}

}