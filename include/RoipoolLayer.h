#ifndef ROIPOOLLAYER_H
#define ROIPOOLLAYER_H
#include "BaseLayer.h"
class RoipoolLayer: public Layer{
	public:
	RoipoolLayer();
	RoipoolLayer(const JsonValue& j);
	virtual void show();
	virtual void setup_outputs();
	virtual void forward();
	int h,w;
	float s;
};
#endif
