#ifndef CONVLAYER_H
#define CONVLAYER_H
#include "BaseLayer.h"
class ConvLayer: public Layer{
	public:
	ConvLayer();
	ConvLayer(const JsonValue& j);
	virtual ~ConvLayer();
	virtual void show();
	virtual void setup_outputs();
	virtual void forward();
	virtual void backward();
	void backward_params();
	bool bias;
	int group, 
	    num,
	    kernel_size_h, kernel_size_w, 
	    pad_h, pad_w, 
	    stride_h, stride_w;

	 private:
	 float * col = 0;
	 int * table = 0;
};
#endif
