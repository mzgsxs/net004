#ifndef CONCATLAYER_H
#define CONCATLAYER_H
#include "BaseLayer.h"
class ConcatLayer: public Layer{
	public:
	ConcatLayer(const std::string&name);
	virtual ~ConcatLayer();
	virtual void forward();
	virtual void backward();
	virtual void setup_shape();
	virtual void setup_data();
	virtual void show()const;

	Blob output, output_dif;
};

#endif
