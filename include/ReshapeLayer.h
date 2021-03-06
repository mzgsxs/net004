#ifndef RESHAPELAYER_H
#define RESHAPELAYER_H
#include "BaseLayer.h"
class ReshapeLayer: public Layer{
	public:
	ReshapeLayer();
	ReshapeLayer(const JsonValue& j);
	virtual void show();
	virtual void setup_outputs();
	virtual void forward();
	int shape[4];
};
#endif
