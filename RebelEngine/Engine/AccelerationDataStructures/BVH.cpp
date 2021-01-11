#include "BVH.h"

#include "Utils/Timer.h"
#include "Utils/Globals.h"
#include "Utils/debugdraw.h"

#include "Components/ComponentMeshRenderer.h"


#include <string>
#include <omp.h>

#include <stack>
#include <queue>

#include <random>

#include <iomanip>

struct int2 {

    int2(unsigned int _x, unsigned int _y) : x(_x), y(_y) {};

    unsigned int x, y;
};

static constexpr uint8_t clz_table_4bit[16] = { 4, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };

static unsigned int clz32d(uint32_t x) /* 32-bit clz */{
    unsigned int n = 0;
    if ((x & 0xFFFF0000) == 0) { n = 16; x <<= 16; }
    if ((x & 0xFF000000) == 0) { n += 8; x <<= 8; }
    if ((x & 0xF0000000) == 0) { n += 4; x <<= 4; }
    n += (unsigned int)clz_table_4bit[x >> (32 - 4)];
    return n;
}

int LongestCommonPrefix(int i, int j, int size, std::vector<GameObject *>& orderedGameObjects) {

    if (i < 0 || i > size - 1 || j < 0 || j > size - 1) return -1;

    uint32_t codeI = orderedGameObjects[i]->GetMorton();
    uint32_t codeJ = orderedGameObjects[j]->GetMorton();

    if (i == j) {
        return clz32d(codeI ^ codeJ);
    }
    else return clz32d(codeI ^ codeJ);

}

unsigned int findSplit(std::vector<GameObject *>& orderedGameObjects, int first, int last) {

    if (first == last) {
        return -1;
    }

    uint32_t firstCode = orderedGameObjects[first]->GetMorton();
    uint32_t lastCode = orderedGameObjects[last]->GetMorton();

    unsigned int commonPrefix = clz32d(firstCode ^ lastCode);

    int split = first;
    int step = last - first;

    do {
        step = (step + 1) >> 1;
        int newSplit = split + step;

        if (newSplit < last) {

            uint32_t splitCode = orderedGameObjects[newSplit]->GetMorton();

            unsigned int splitPrefix = clz32d(firstCode ^ splitCode);

            if (splitPrefix > commonPrefix) {

                split = newSplit;
            }
        }

    } while (step > 1);

    return split;

}

int2 determineRange(std::vector<GameObject *>& orderedGameObjects, int idx, unsigned int size) {

    int d = LongestCommonPrefix(idx, idx + 1, size, orderedGameObjects) -
        LongestCommonPrefix(idx, idx - 1, size, orderedGameObjects) >= 0 ? 1 : -1;

    int dmin = LongestCommonPrefix(idx, idx - d, size, orderedGameObjects);

    int lmax = 2;

    while (LongestCommonPrefix(idx, idx + lmax * d, size, orderedGameObjects) > dmin) {
        lmax <<= 1;
    }

    int l = 0;
    int div = 2;

    for (int t = lmax / div; t >= 1; t >>= 1) {

        if (LongestCommonPrefix(idx, idx + (l + t) * d, size, orderedGameObjects) > dmin) l += t;

    }

    int jdx = idx + l * d;

    if (jdx < idx) return int2(jdx, idx);
    else return int2(idx, jdx);

}

void computeBoundingBox(BVHNode* root) {

    std::stack <BVHNode*> stack;
    std::queue <BVHNode*> queue;

    queue.push(root);

    while (!queue.empty()) {

        root = queue.front();

        queue.pop();
        stack.push(root);

        if (root->left) 
            queue.push(root->left);
        if (root->right) 
            queue.push(root->right);
    }

    while (!stack.empty()) {

        root = stack.top();

        if(!root->is_leaf) { //Internal node

            root->aab_box = root->left->aab_box;
            AABB right_aabb = root->right->aab_box;
            root->aab_box.Enclose(right_aabb);
        }
        stack.pop();
    }
}

BVHNode* BVH::GenerateBVH(std::vector<GameObject*>& orderedGameObjects, unsigned int size) {

    BVHNode* leaf_nodes = new BVHNode[size];
    BVHNode* internal_nodes = new BVHNode[size - 1];

    MSTimer constructionTime; constructionTime.start();

    for (unsigned int i = 0; i < size; i++) {
        leaf_nodes[i].is_leaf = true;
        AABB box; orderedGameObjects[i]->GetAABB(box);
        //OBB obb = box.Transform(orderedGameObjects[i]->GetGlobalMatrix());
        leaf_nodes[i].aab_box = box;
        leaf_nodes[i].name = orderedGameObjects[i]->GetName();
        leaf_nodes[i].go = new GameObject(*orderedGameObjects[i]);
    }

    //int maxthreads = omp_get_max_threads();
    //omp_set_num_threads(maxthreads);
    //#pragma omp parallel for schedule(static, maxthreads)
    for (unsigned int idx = 0; idx < size - 1; idx++) {

        //determine range
        int2 range = determineRange(orderedGameObjects, idx, size - 1);

        unsigned int first = range.x;
        unsigned int last = range.y;

        //find partition point
        unsigned int split = findSplit(orderedGameObjects, first, last);

        BVHNode* childA;
        BVHNode* childB;

        if (split == -1) {
            split = (first + last) >> 1;
            ++last;
        }

        if (split == first) {
            childA = &leaf_nodes[split];
        }
        else {
            childA = &internal_nodes[split];
            std::string aux("internal+" + std::to_string(split));
            childA->name = _strdup(aux.c_str());
        }

        if (split + 1 == last) {
            childB = &leaf_nodes[split + 1];
        }
        else {
            childB = &internal_nodes[split + 1];
            std::string aux("internal+" + std::to_string(split + 1));
            childB->name = _strdup(aux.c_str());
        }

        //childA->parent = &internal_nodes[idx];
        //childB->parent = &internal_nodes[idx];

        internal_nodes[idx].left = childA;
        internal_nodes[idx].right = childB;


    }

    Uint32 time = constructionTime.read(); LOG(_INFO, "Parallel BVH Construction time: %d", time);

    internal_nodes[0].name = "root";
    computeBoundingBox(&internal_nodes[0]);

    return &internal_nodes[0];
}

void BVH::DeleteRec(BVHNode* node) {
    if (node->left) DeleteRec(node->left);
    if (node->right) DeleteRec(node->right);
    if (node->is_leaf) {
        delete node->go;
        node->go = nullptr;
    }
    delete[] node;
}

BVH::~BVH() {
    /*if (_root) {
        DeleteRec(_root);
        _root = nullptr;
    }*/
}

#ifdef _DEBUG
// Helper function to print branches of the binary tree
void showTrunks(Trunk* p, std::ostringstream& oss)
{
    if (p == nullptr)
        return;

    showTrunks(p->prev, oss);

    oss << p->str;
}

// Recursive function to print binary tree
// It uses inorder traversal
void BVH::Inorder(BVHNode* root, Trunk* prev, bool isLeft)
{
    if (root == nullptr)
        return;

    std::string prev_str = "    ";
    Trunk* trunk = new Trunk(prev, prev_str);

    Inorder(root->left, trunk, true);

    if (!prev)
        trunk->str = "---";
    else if (isLeft)
    {
        trunk->str = ".---";
        prev_str = "   |";
    }
    else
    {
        trunk->str = "`---";
        prev->str = prev_str;
    }

    std::ostringstream oss;
    showTrunks(trunk, oss);
    oss << root->name;
    LOG(_ERROR, "%s", std::string(oss.str()).c_str());

    if (prev)
        prev->str = prev_str;
    trunk->str = "   |";

    Inorder(root->right, trunk, false);
    delete trunk;
    trunk = nullptr;
}
#endif

BVH::BVH(){}

void BVH::DebugBVH(BVHNode *node) {

    dd::aabb(node->aab_box.minPoint, node->aab_box.maxPoint, dd::colors::White);

    if (node->left) DebugBVH(node->left);
    if (node->right) DebugBVH(node->right);

}
