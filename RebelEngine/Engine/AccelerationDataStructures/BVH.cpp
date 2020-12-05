#include "BVH.h"

#include "Utils/Timer.h"
#include "Utils/Globals.h"

#include "Components/ComponentMeshRenderer.h"

#include <string>
#include <omp.h>

struct int2 {

    int2(int _x, int _y) : x(_x), y(_y) {};

    int x, y;
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

int LongestCommonPrefix(int i, int j, int size, std::vector<std::unique_ptr<GameObject>>& orderedGameObjects) {

    if (i < 0 || i > size - 1 || j < 0 || j > size - 1) return -1;

    ComponentMeshRenderer* compF = static_cast<ComponentMeshRenderer*>(orderedGameObjects[i].get()->GetComponent(type_component::MESHRENDERER));
    ComponentMeshRenderer* compL = static_cast<ComponentMeshRenderer*>(orderedGameObjects[j].get()->GetComponent(type_component::MESHRENDERER));

    uint32_t codeI = compF->GetMorton();
    uint32_t codeJ = compL->GetMorton();

    if (i == j) {
        return clz32d(codeI ^ codeJ);
    }
    else return clz32d(codeI ^ codeJ);

}

unsigned int findSplit(std::vector<std::unique_ptr<GameObject>>& orderedGameObjects, int first, int last) {


    if (first == last) {
        return -1;
    }

    ComponentMeshRenderer* compF = static_cast<ComponentMeshRenderer*>(orderedGameObjects[first].get()->GetComponent(type_component::MESHRENDERER));
    ComponentMeshRenderer* compL = static_cast<ComponentMeshRenderer*>(orderedGameObjects[last].get()->GetComponent(type_component::MESHRENDERER));

    uint32_t firstCode = compF->GetMorton();
    uint32_t lastCode = compL->GetMorton();

    unsigned int commonPrefix = clz32d(firstCode ^ lastCode);

    int split = first;
    int step = last - first;

    do {
        step = (step + 1) >> 1;
        int newSplit = split + step;

        if (newSplit < last) {

            ComponentMeshRenderer* aux = static_cast<ComponentMeshRenderer*>(orderedGameObjects[newSplit].get()->GetComponent(type_component::MESHRENDERER));
            uint32_t splitCode = aux->GetMorton();

            int splitPrefix = clz32d(firstCode ^ splitCode);

            if (splitPrefix > commonPrefix) {

                split = newSplit;
            }
        }

    } while (step > 1);

    return split;

}

int2 determineRange(std::vector<std::unique_ptr<GameObject>>& orderedGameObjects, int idx, unsigned int size) {

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

BVHNode* BVH::GenerateBVH(std::vector<std::unique_ptr<GameObject>>& orderedGameObjects, unsigned int size) {

    BVHNode* leaf_nodes = new BVHNode[size];
    BVHNode* internal_nodes = new BVHNode[size-1];

    MSTimer constructionTime; constructionTime.start();

    for (unsigned int i = 0; i < size; i++) {
        leaf_nodes[i].go = orderedGameObjects[i].get();
    }

    int maxthreads = omp_get_max_threads();
    omp_set_num_threads(maxthreads);
    #pragma omp parallel for schedule(static, maxthreads)
    for (int idx = 0; idx < size - 1; idx++) {

        //determine range
        int2 range = determineRange(orderedGameObjects, idx, size - 1);

        int first = range.x;
        int last = range.y;

        //find partition point
        int split = findSplit(orderedGameObjects, first, last);

        BVHNode* childA;
        BVHNode* childB;

        if (split == -1) {
            split = (first + last) >> 1;
            ++last;
        }

        if (split == first) {
            childA = &leaf_nodes[split];
            childA->name = leaf_nodes[split].name;
        }
        else {
            childA = &internal_nodes[split];
            std::string aux("internal+" + std::to_string(split));
            childA->name = aux.c_str();
        }

        if (split + 1 == last) {
            childB = &leaf_nodes[split + 1];
            childB->name = leaf_nodes[split + 1].name;
        }
        else {
            childB = &internal_nodes[split + 1];
            std::string aux("internal+" + std::to_string(split+1));
            childB->name = aux.c_str();
        }

        internal_nodes[idx].left = childA;
        internal_nodes[idx].right = childB;
        childA->parent = &internal_nodes[idx];
        childB->parent = &internal_nodes[idx];

    }

    Uint32 time = constructionTime.read(); LOG(_INFO, "Parallel BVH Construction time: %d", time);

    return &internal_nodes[0];
}
