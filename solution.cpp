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

static double deg2rad(double degrees){
    return degrees * M_PI / 180.0;
}

static double clamp(double value, double minValue, double maxValue){
    return min(max(value, minValue), maxValue);
}

static int clamp(int value, int minValue, int maxValue){
    return min(max(value, minValue), maxValue);
}

static double pythDistance(int x1, int y1, int x2, int y2){
    return pow(pow(x1 - x2, 2) + pow(y1 - y2, 2), 0.5);
}

class Vector{
    public:
    Vector(double x, double y): _x(x), _y(y){
    }
    
    Vector(int x1, int y1, int x2, int y2){
        this->_x = x2 - x1;
        this->_y = y2 - y1;
    }
    
    std::string toString(){
        char buff[100];
        snprintf(buff, sizeof(buff), "{%f,%f}", _x, _y);
        std::string output = buff;
        return output;
    }
    
    void normalize(){
        double length = pow(pow(_x, 2.0) + pow(_y, 2.0), 0.5);
        this->_x /= length;
        this->_y /= length;
    }
    
    std::pair<int, int> toPoint(){
        return std::make_pair((int) this->_x, (int) this->_y);
    }
    
    Vector operator + (Vector const &obj) { 
         Vector res(this->_x + obj._x, this->_y + obj._y); 
         return res; 
    }
    
    Vector operator + (std::pair<int, int> const &obj) { 
         Vector res(this->_x + obj.first, this->_y + obj.second); 
         return res; 
    }
    
    Vector operator - (Vector const &obj) { 
         Vector res(this->_x - obj._x, this->_y - obj._y); 
         return res; 
    }
    
    Vector operator * (double const &obj) { 
         Vector res(this->_x * obj, this->_y * obj); 
         return res; 
    }
    
    Vector operator / (double const &obj) { 
         Vector res(this->_x / obj, this->_y / obj); 
         return res; 
    }
    
    private:
    double _x, _y;
};

std::vector<std::pair<int, int> > checkpoints;
bool doneMapping = false;
std::pair<int, int> nextCheckpoint;
std::pair<int, int> prevCheckpoint;
int targetCounter = 0;

std::vector<Vector> nextPoint;

void calculateNextPointVectors(){
    for(int i=0; i < checkpoints.size(); i++){
        int nextIndex = (i + 1) % checkpoints.size();
        std::cerr << "Indexes " << i << " " << nextIndex << endl;
        Vector toNext(checkpoints[i].first, checkpoints[i].second,
                      checkpoints[nextIndex].first, checkpoints[nextIndex].second);
        toNext.normalize();
        nextPoint.push_back(toNext);
    }
}

void updateCheckpoints(std::pair<int, int> checkpoint){    
    if(!doneMapping){
        if(checkpoints.size() > 2 && checkpoints[0] == checkpoint){
            doneMapping = true;
            std::rotate(checkpoints.rbegin(), checkpoints.rbegin() + 1, checkpoints.rend());
            calculateNextPointVectors();
        } else if(checkpoints.size() == 0 || checkpoints[checkpoints.size() - 1] != checkpoint){
            checkpoints.push_back(checkpoint);
        }
    }
    
    
    std::cerr << "Checkpoints: ";
    for(auto point: checkpoints){
        std::cerr << point.first << ", " << point.second << " ; ";
    }
    std::cerr << endl;
}

void updateTargetCounter(std::pair<int, int> checkpoint){
    if(nextCheckpoint == std::make_pair(0, 0) || checkpoint != nextCheckpoint){
        prevCheckpoint = nextCheckpoint;
        nextCheckpoint = checkpoint;
        ++targetCounter;
        if(doneMapping){
            targetCounter %= checkpoints.size();
        }
    }
}

std::pair<int, int> getCorrectedTarget(int range){
    std::cerr << "nextPoints: ";
    for(auto point: nextPoint){
        std::cerr << point.toString() << " ; ";
    }
    std::cerr << endl;
    return (nextPoint[targetCounter] * range + checkpoints[targetCounter]).toPoint();
}

int main()
{
    bool boostUsed = false;
    
    // game loop
    while (1) {
        int x, y; // position of your pod
        int nextCheckpointX, nextCheckpointY; // position of the next check point
        int nextCheckpointDist, nextCheckpointAngle;
        int opponentX, opponentY;
        cin >> x >> y >> nextCheckpointX >> nextCheckpointY >> nextCheckpointDist >> nextCheckpointAngle; 
        cin >> opponentX >> opponentY;
        cin.ignore();
        
        std::pair<int, int> checkpoint = std::make_pair(nextCheckpointX, nextCheckpointY);
        updateCheckpoints(checkpoint);
        updateTargetCounter(checkpoint);
        
        if(doneMapping){
            std::pair<int, int> target = getCorrectedTarget(300);
            nextCheckpointX = target.first;
            nextCheckpointY = target.second;
            
        }
        
        double distanceMultiplier = clamp(pow(nextCheckpointDist, 2) / 5000000.0, 0.5, 1.0);
        double angleMultiplier = clamp(pow(clamp(cos(deg2rad(nextCheckpointAngle)), 0.0, 1.0), 0.5), 0.25, 1.0);
        double enemyDistanceMultiplier = pythDistance(x, y, opponentX, opponentY) < 2000 ? 2 : 1;
        
        std::cerr << "Multipliers: dist:" << distanceMultiplier;
        std::cerr << " angle: " << angleMultiplier;
        std::cerr << " enemy: " << enemyDistanceMultiplier << endl;
        std::cerr << "Target index: " << targetCounter << endl;

        if(!boostUsed && (nextCheckpointDist >= 6000 && nextCheckpointAngle < 10 && nextCheckpointAngle > -10)){
            cout << nextCheckpointX << " " << nextCheckpointY << " BOOST" << endl;
            boostUsed = true;
        }else{
            cout << nextCheckpointX << " " << nextCheckpointY << " " 
             << clamp((int) (100 * distanceMultiplier * angleMultiplier * enemyDistanceMultiplier), 15, 100) << endl;
        }
    }
}