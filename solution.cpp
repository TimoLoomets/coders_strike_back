#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "math.h"

using namespace std;

/**
 * This code automatically collects game data in an infinite loop.
 * It uses the standard input to place data into the game variables such as x and y.
 * YOU DO NOT NEED TO MODIFY THE INITIALIZATION OF THE GAME VARIABLES.
 **/

int main()
{
    bool boost_used = false;

    // game loop
    while (1) {
        int x; // x position of your pod
        int y; // y position of your pod
        int nextCheckpointX; // x position of the next check point
        int nextCheckpointY; // y position of the next check point
        int nextCheckpointDist;
        int nextCheckpointAngle;
        int _;
        cin >> x >> y >> nextCheckpointX >> nextCheckpointY >> nextCheckpointDist >> nextCheckpointAngle; 
        cin >> _ >> _;
        cin.ignore();

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;


        // Edit this line to output the target position
        // and thrust (0 <= thrust <= 100)
        // i.e.: "x y thrust"
        if(!boost_used && (nextCheckpointDist >= 8500 && nextCheckpointAngle < 20 && nextCheckpointAngle > -20)){
            cout << nextCheckpointX << " " << nextCheckpointY << " BOOST" << endl;
        }else{
            cout << nextCheckpointX << " " << nextCheckpointY << " " 
             << (int) min(max(nextCheckpointDist / 25.0, 50.0), 100.0) << endl;
        }
    }
}
