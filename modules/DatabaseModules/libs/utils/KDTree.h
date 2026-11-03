#pragma once
#include <vector>
#include <string>
#ifdef  _WIN32
#include "UserDefinitions.h"
#endif // Windows

#ifdef __APPLE__
#include "UserDefinitions.h"
#endif // MacOs

struct PointMag
{
    std::vector<float> vec;
    int uID = -1;
};

struct Point2D
{
    float dim1 = 0.0f;
    float dim2 = 0.0f;
    int cluster = -1;
    std::string samplePath = "unknown";
    TUsineColor color = sdkAlphaColor(255, 255, 0);
    int uID = -1;
    std::shared_ptr<const PointMag> mag;
    
    Point2D(float firstValue, float secondValue, const std::string& samplePath)
    {
        this->dim1 = firstValue;
        this->dim2 = secondValue;
        this->samplePath = samplePath;
    }
    
    Point2D() = default;
    ~Point2D() = default;
};



struct KDNode
{
    Point2D* point;
    std::unique_ptr<KDNode> left = nullptr;
    std::unique_ptr<KDNode> right = nullptr;
};


struct PointDistance2D
{
    float distance = 0.0f;
    Point2D* point = nullptr;
    
    PointDistance2D(float dist, Point2D* point)
    {
        this->distance = dist;
        this->point = point;
    }
    
    PointDistance2D() = default;
    
    bool operator<(const PointDistance2D& other) const
    {
        return distance < other.distance;
    }
    
    bool operator>(const PointDistance2D& other) const
    {
        return distance > other.distance;
    }
    
};

class KDTree
{
public:
    KDTree(int numDimensions);
    ~KDTree() = default;
 
    std::unique_ptr<KDNode> buildTree(std::vector<Point2D*>& dataVector, int depth);
    std::unique_ptr<KDNode> root = nullptr;
    
    bool isCreated = false;
    
    void searchInRange(float cx, float cy, float radius, std::vector<PointDistance2D>& resultQueue) const;
    /*
    void searchNInRange(float cx, float cy, float radius, int N, std::priority_queue<PointDistance2D>& closestPoints) const;
    */
    void clear();
    
private:
    
    int numDimensions = 2;
    void search(const std::unique_ptr<KDNode>& node, float cx, float cy, float r2, int depth,
                std::vector<PointDistance2D>& resultQueue) const;
    /*
    void searchNClosest(const std::unique_ptr<KDNode>& node, float cx, float cy, float r2, int depth, int N,
                        std::priority_queue<PointDistance2D>& closestPoints) const;
    */
};

