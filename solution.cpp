#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "math.h"
#include <memory>

/**
 * This code automatically collects game data in an infinite loop.
 * It uses the standard input to place data into the game variables such as x and y.
 * YOU DO NOT NEED TO MODIFY THE INITIALIZATION OF THE GAME VARIABLES.
 **/
 
class Vector;
class Pod;

std::vector<std::pair<int, int> > checkpoints;
std::vector<Vector> nextPoint;
std::unique_ptr<std::pair<Pod, Pod> > pods{};
std::unique_ptr<std::pair<Pod, Pod> > enemyPods{};
int laps;

static double deg2rad(double degrees){
    return degrees * M_PI / 180.0;
}

static double clamp(double value, double minValue, double maxValue){
    return std::min(std::max(value, minValue), maxValue);
}

static int clamp(int value, int minValue, int maxValue){
    return std::min(std::max(value, minValue), maxValue);
}

static double pythDistance(int x1, int y1, int x2, int y2){
    return pow(pow(x1 - x2, 2) + pow(y1 - y2, 2), 0.5);
}

class Vector{
    public:
    Vector(){
        Vector(0, 0);
    }
    
    Vector(int x, int y): _x(x), _y(y){
    }
    
    Vector(std::pair<int, int> vec): _x(vec.first), _y(vec.second){
    }
    
    Vector(double x, double y): _x(x), _y(y){
    }
    
    Vector(int x1, int y1, int x2, int y2){
        this->_x = x2 - x1;
        this->_y = y2 - y1;
    }
    
    std::string toString(){
        return "{" + std::to_string(_x) + "," + std::to_string(_y) + "}";
    }
    
    void normalize(){
        //double length = this->length();
        Vector newValue = this->normalizedValue();
        this->_x = newValue._x;
        this->_y = newValue._y;
    }
    
    Vector normalizedValue(){
        double length = pow(pow(_x, 2.0) + pow(_y, 2.0), 0.5);
        return length == 0.0 ? Vector(0, 0) : Vector(this->_x / length, this->_y / length);
    }
    
    double length(){
        return pow(pow(_x, 2.0) + pow(_y, 2.0), 0.5);
    }
    
    double heading(){
        return std::fmod(abs(atan2(_y, _x)) + _y < 0 ? M_PI : 0, M_2_PI);
    }
    
    double angleTo(Vector end){
        double angle = end.heading() - heading();
        while(angle < -M_PI) angle += M_2_PI;
        return angle;
    }
    
    double angleFrom(double start){
        double angle = heading() - start;
        //std::cerr << "angle: " << angle << endl;
        while(angle < -M_PI_2) angle += M_2_PI;
        return angle;
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
    
    bool operator == (Vector const &obj){
        return abs(obj._x - this->_x) < 0.0001 && abs(obj._y - this->_y) < 0.0001;
    }
    
    private:
    double _x, _y;
};

class Pod{
    public:
    Vector location;
    Vector speed;
    double angle;
    int nextCheckPointId;
    
    Pod(): nextCheckPointId(1){
        static int podCount = 0;
        //std::cerr << "Creating pod nr: " << podCount << endl;
        podId = podCount++;
    }
    
    std::string toString(){
        return "Pod" + std::to_string(podId)
                 + " to: " + std::to_string(nextCheckPointId)
                 + " at: " + location.toString() + " " + std::to_string(angle)
                 + " vel: " + speed.toString();
    }
    
    std::string getAction(){
        //std::cerr << "Getting action of " << toString() << endl;
        static bool boostUsed = false;
        
        Vector nextPointTarget = getNextPointCorrectedTarget(325);
        //std::cerr << "Done calculating nextPointTarget " << nextPointTarget.toString() << endl;
        Vector target = getSpeedCorrectedTarget(nextPointTarget);
        //std::cerr << "Done calculating target " << target.toString() << endl;
        
        double nextCheckpointDist = target.length();
        double nextCheckpointAngle = nextPointTarget.angleFrom(angle);
        std::cerr << "nextCheckpointAngle: " << nextCheckpointAngle << std::endl;
        
        double distanceMultiplier = clamp(pow(nextCheckpointDist, 2) / 7000000.0, 0.5, 1.0);
        double angleMultiplier = clamp(pow(clamp(cos(nextCheckpointAngle), 0.0, 1.0), 0.5), 0.25, 1.0);
        double enemyDistanceMultiplier = closestEnemyDistance() < 2000 ? 2.55 : 1;
        
        std::cerr << "Multipliers: dist:" << distanceMultiplier;
        std::cerr << " angle: " << angleMultiplier;
        std::cerr << " enemy: " << enemyDistanceMultiplier << std::endl;
        std::cerr << "Target index: " << nextCheckPointId << std::endl;
        
        
        std::pair<int, int> targetPoint = target.toPoint();
        if(aboutToCollide() && speed.length() > 350){
            return std::to_string(targetPoint.first) + " " + std::to_string(targetPoint.second) + " SHIELD";
        }else if(!boostUsed && (nextCheckpointDist >= 6500 && abs(nextCheckpointAngle) < 0.175)){
            return std::to_string(targetPoint.first) + " " + std::to_string(targetPoint.second) + " BOOST";
            boostUsed = true;
        }else{
            return std::to_string(targetPoint.first) + " " 
                   + std::to_string(targetPoint.second) + " "
                   + std::to_string(clamp((int) (100 
                                                 * distanceMultiplier 
                                                 * angleMultiplier 
                                                 * enemyDistanceMultiplier), 
                                          10, 
                                          100));
        }
    }
    
    private:
    int podId;
    
    Vector getNextPointCorrectedTarget(int range){
        return (nextPoint[nextCheckPointId] * range + checkpoints[nextCheckPointId]).toPoint();
    }
    
    Vector getSpeedCorrectedTarget(Vector target){
        return ((location + speed) 
                 + ((target - location).normalizedValue() * 2 
                    - speed.normalizedValue()).normalizedValue() 
                 * 7000);
    }
    
    double closestEnemyDistance(){
        return std::min((enemyPods->first.location - location).length(),
                        (enemyPods->second.location - location).length());
    }
    
    bool aboutToCollide(){
        Vector nextLocation = location + speed;
        return std::min((nextLocation - enemyPods->first.location + enemyPods->first.speed).length(),
                        (nextLocation - enemyPods->second.location + enemyPods->second.speed).length()) 
               < 800;
    }
};

void calculateNextPointVectors(){
    for(int i=0; i < checkpoints.size(); i++){
        int nextIndex = (i + 1) % checkpoints.size();
        Vector toNext(checkpoints[i].first, checkpoints[i].second,
                      checkpoints[nextIndex].first, checkpoints[nextIndex].second);
        toNext.normalize();
        nextPoint.push_back(toNext);
    }
}

void collectInitializationData(){
    int checkpointCount;
    std::cin >> laps >> checkpointCount;
    int x, y;
    for(int i=0; i < checkpointCount && std::cin >> x >> y; i++){
        checkpoints.push_back(std::make_pair(x, y));
    }
    calculateNextPointVectors();
}

void collectPodData(Pod* pod){
    int x, y, vx, vy;
    std::cin >> x >> y >> vx >> vy;
    pod->location = Vector(x, y);
    pod->speed = Vector(vx, vy);
    std::cin >> pod->angle >> pod-> nextCheckPointId;
    pod->angle = deg2rad(pod->angle);
    //std::cerr << "Done updating " << pod->toString() << endl; 
}

void updatePods(){
    collectPodData(&(pods->first));
    collectPodData(&(pods->second));
    collectPodData(&(enemyPods->first));
    collectPodData(&(enemyPods->second));
}

int main()
{
    std::cerr << "START" << std::endl;
    collectInitializationData();
    std::cerr << "INIT DATA COLLECT COMPLETE" << std::endl;
    pods = std::make_unique<std::pair<Pod, Pod> >(Pod(), Pod());
    enemyPods = std::make_unique<std::pair<Pod, Pod> >(Pod(), Pod());
    
    std::cerr << "INIT COMPLETE" << std::endl;
    // game loop
    while (1) {
        updatePods();
        //std::cerr << "POD UPDATE COMPLETE" << endl;
        std::cin.ignore();
        std::cout << pods->first.getAction() << std::endl;
        std::cout << pods->second.getAction() << std::endl;
        //std::cerr << "DATA OUTPUT COMPLETE" << endl;
    }
}