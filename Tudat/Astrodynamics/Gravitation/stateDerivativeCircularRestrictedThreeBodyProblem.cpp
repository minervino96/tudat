/*    Copyright (c) 2010-2017, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 *
 *    References
 *        Wakker, K.F., "Astrodynamics I, AE4-874", Delft University of Technology, 2007.
 *
 */

#include <cmath>

#include "Tudat/Basics/utilityMacros.h"

#include "Tudat/Astrodynamics/Gravitation/stateDerivativeCircularRestrictedThreeBodyProblem.h"

namespace tudat
{
namespace gravitation
{
namespace circular_restricted_three_body_problem
{

//! Compute state derivative.
basic_mathematics::Vector6d StateDerivativeCircularRestrictedThreeBodyProblem::computeStateDerivative(
        const double time, const basic_mathematics::Vector6d& cartesianState )
{
    TUDAT_UNUSED_PARAMETER( time );

    // Compute distance to primary body.
    const double xCoordinateToPrimaryBodySquared =
            ( cartesianState( xCartesianPositionIndex ) + massParameter )
            * ( cartesianState( xCartesianPositionIndex ) + massParameter );

    const double yCoordinateSquared = cartesianState( yCartesianPositionIndex )
            * cartesianState( yCartesianPositionIndex );

    const double zCoordinateSquared = cartesianState( zCartesianPositionIndex )
            * cartesianState( zCartesianPositionIndex );

    const double normDistanceToPrimaryBodyCubed = pow(
                xCoordinateToPrimaryBodySquared + yCoordinateSquared + zCoordinateSquared, 1.5 );

    // Compute distance to secondary body.
    const double xCoordinateSecondaryBodySquared =
            ( cartesianState( xCartesianPositionIndex ) - ( 1.0 - massParameter ) )
            * ( cartesianState( xCartesianPositionIndex ) - ( 1.0 - massParameter ) );

    double normDistanceToSecondaryBodyCubed = pow(
                xCoordinateSecondaryBodySquared + yCoordinateSquared + zCoordinateSquared, 1.5 );

    // Compute derivative of state.
    Eigen::VectorXd stateDerivative( 6 );

    stateDerivative.segment( xCartesianPositionIndex, 3 ) = cartesianState.segment( xCartesianVelocityIndex, 3 );

    stateDerivative( xAccelerationIndex ) = cartesianState( xCartesianPositionIndex )
            - ( ( 1.0 - massParameter ) / normDistanceToPrimaryBodyCubed )
            * ( cartesianState( xCartesianPositionIndex ) + massParameter )
            - ( massParameter / normDistanceToSecondaryBodyCubed )
            * ( cartesianState( xCartesianPositionIndex ) - ( 1.0 - massParameter ) )
            + 2.0 * cartesianState( yCartesianVelocityIndex );
    stateDerivative( yAccelerationIndex ) = cartesianState( yCartesianPositionIndex )
            * ( 1.0 - ( ( 1.0 - massParameter ) / normDistanceToPrimaryBodyCubed )
                - ( massParameter / normDistanceToSecondaryBodyCubed ) )
            - 2.0 * cartesianState( xCartesianVelocityIndex );
    stateDerivative( zAccelerationIndex ) = -cartesianState( zCartesianPositionIndex )
            * ( ( ( 1.0 - massParameter ) / normDistanceToPrimaryBodyCubed )
                + ( massParameter / normDistanceToSecondaryBodyCubed ) );

    // Return computed state derivative.
    return stateDerivative;
}

} // namespace circular_restricted_three_body_problem
} // namespace gravitation
} // namespace tudat
