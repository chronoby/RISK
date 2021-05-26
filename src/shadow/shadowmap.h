#ifndef SHADOWMAP_H_
#define SHADOWMAP_H_

class ShadowMap
{
public:
	ShadowMap();

	unsigned int depthMapFBO;
	unsigned int depthMap;
    unsigned int SHADOW_WIDTH;
    unsigned int SHADOW_HEIGHT;
};

#endif // SHADOWMAP_H_