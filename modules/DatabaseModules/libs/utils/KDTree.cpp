#include "KDTree.h"

KDTree::KDTree(int numDimensions)
{
    this->numDimensions = numDimensions;
}

void KDTree::clear()
{
    root.reset();
}

std::unique_ptr<KDNode> KDTree::buildTree(std::vector<Point2D*>& dataVector, int depth)
{
    if(dataVector.empty()) return nullptr;
    
    int axis = depth % numDimensions;
    
    std::sort(dataVector.begin(), dataVector.end(), [axis](const Point2D* a, const Point2D* b){
        return axis == 0 ? a->dim1 < b->dim1 : a->dim2 < b->dim2;
    });
    
    size_t median = dataVector.size() / 2;
    
    auto node = std::make_unique<KDNode>();
    node->point = dataVector[median];
    
    std::vector<Point2D*> left(dataVector.begin(), dataVector.begin() + median);
    std::vector<Point2D*> right(dataVector.begin() + median + 1 , dataVector.end());
    
    node->left = buildTree(left,depth + 1);
    node->right = buildTree(right, depth + 1);
    
    return node;
    
}

void KDTree::searchInRange(float cx, float cy, float radius, std::vector<PointDistance2D>& resultQueue) const
{
    float r2 = radius*radius;
    search(root, cx, cy, r2, 0, resultQueue);
}

void KDTree::search(const std::unique_ptr<KDNode>& node, float cx, float cy, float r2, int depth,
                    std::vector<PointDistance2D>& resultQueue) const
{
    if (!node) return;
    
    float dx = node->point->dim1 - cx;
    float dy = node->point->dim2 - cy;
    float dist2 = dx * dx + dy * dy;
    
    if (dist2 <= r2)
    {
        resultQueue.emplace_back((r2 - dist2) / r2, node->point);
    }
        
    int axis = depth % numDimensions;
    
    float split = axis == 0 ? node->point->dim1 : node->point->dim2;
    float center = axis == 0 ? cx : cy;
    
    const std::unique_ptr<KDNode>& nextBranch = (center < split) ? node->left : node->right;
    const std::unique_ptr<KDNode>& otherBranch = (center < split) ? node->right : node->left;
    
    search(nextBranch, cx, cy, r2, depth + 1, resultQueue);
    
    float diff = center - split;
    if (diff * diff <= r2)
    {
        search(otherBranch, cx, cy, r2, depth + 1, resultQueue);
    }
        
}

/*
void KDTree::searchNInRange(float cx, float cy, float radius, int N, std::priority_queue<PointDistance2D>& closestPoints) const
{
    float r2 = radius*radius;
    searchNClosest(root, cx, cy, r2, 0, N, closestPoints);
}

void KDTree::searchNClosest(const std::unique_ptr<KDNode> &node, float cx, float cy, float r2, int depth, int N,
                            std::priority_queue<PointDistance2D>& closestPoints) const
{
    if (!node) return;
    
    float dx = node->point->dim1 - cx;
    float dy = node->point->dim2 - cy;
    float dist2 = dx * dx + dy * dy;
    
    if (dist2 <= r2)
    {
        if((int)closestPoints.size() < N)
        {
            closestPoints.emplace(dist2, node->point);
        } else if (dist2 < closestPoints.top().distance)
        {
            closestPoints.pop();
            closestPoints.emplace(dist2, node->point);
        }
    }
    
    int axis = depth % numDimensions;
    
    float split = axis == 0 ? node->point->dim1 : node->point->dim2;
    float center = axis == 0 ? cx : cy;
    
    std::unique_ptr<KDNode>* nextBranch = nullptr;
    std::unique_ptr<KDNode>* otherBranch = nullptr;
    
    if (center < split) {
        nextBranch = &node->left;
        otherBranch = &node->right;
    } else {
        nextBranch = &node->right;
        otherBranch = &node->left;
    }
    
    searchNClosest(*nextBranch, cx, cy, r2, depth + 1, N, closestPoints);
    
    float diff = center - split;
    if ((int)closestPoints.size() < N || diff * diff <= r2)
    {
        searchNClosest(*otherBranch, cx, cy, r2, depth + 1, N, closestPoints);
    }
}
*/
