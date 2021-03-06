#include <cmath>
#include <cfloat>
#include "LossLayer.h"
LossLayer::LossLayer(){}
LossLayer::LossLayer(const JsonValue& j):Layer(j){
	const JsonValue& attrs = j.jobj.at("attrs");
	method = attrs.jobj.at("method").jv.s;
	if(method == "softmax"){
		forward_f = &LossLayer::forward_softmax;
		backward_f = &LossLayer::backward_softmax;
		init_f = &LossLayer::init_softmax;
	}
}
void LossLayer::show(){
	Layer::show();
	printf("  (method) %s\n",method.c_str());
}
void LossLayer::setup_outputs(){
	outputs[0].set_shape(1,1,1,1);
	inplace = false;
	setup_outputs_data();
	(this->*init_f)();
}
void LossLayer::forward(){
	//show_inputs();
	(this->*forward_f)();
	//show_outputs();
}
void LossLayer::backward(){
	//show_diff_outputs();
	(this->*backward_f)();
	//show_diff_inputs();
}
void LossLayer::init_softmax(){
	Blob &ib = inputs[0];
	maxs.set_shape(1,1,ib.h,ib.w);
	maxs.alloc();
	sums.set_shape(1,1,ib.h,ib.w);
	sums.alloc();
	softmaxblob.set_shape(ib.n,ib.c,ib.h,ib.w);
	softmaxblob.alloc();
}
void LossLayer::forward_softmax(){
	Blob& ib = inputs[0], &ib2 = inputs[1];
	int batch_size = ib.n, c = ib.c, hw = ib.hw();
	float *pdata = ib.data, *gdata = ib2.data, *odata = outputs[0].data, 
	      *maxdata = maxs.data, *sumdata = sums.data, *softmaxdata = softmaxblob.data,
	      loss = 0.0f;
	for(int i=0;i<batch_size;++i){
		for(int k=0;k<hw;++k){
			maxdata[k] = pdata[k];
			for(int j=1;j<c;++j)
				maxdata[k] = std::max(maxdata[k],pdata[j*hw+k]);
		}
		for(int j=0;j<c;++j)
			for(int k=0;k<hw;++k)
				softmaxdata[j*hw+k] = exp(pdata[j*hw+k] - maxdata[k]);
		for(int k=0;k<hw;++k){
			sumdata[k] = softmaxdata[k];
			for(int j=1;j<c;++j)
				sumdata[k] += softmaxdata[j*hw + k];
		}
		for(int j=0;j<c;++j)
			for(int k=0;k<hw;++k){
				softmaxdata[j*hw +k] /= sumdata[k];
				//printf("%f\n",softmaxdata[j*hw +k]);
			}
		for(int k=0;k<hw;++k)
			loss -= log(std::max(softmaxdata[int(gdata[k]) * hw + k], FLT_MIN));
		pdata += c*hw;
		gdata += inputs[1].chw();
		softmaxdata += softmaxblob.chw();
	}
	odata[0] = loss/inputs[0].n;
}
void LossLayer::backward_softmax(){
	Blob& diff_blob = diff_inputs[0], &label_blob = inputs[1];
	int batch_size = label_blob.n, hw = label_blob.hw(), nchw = softmaxblob.nchw();
	memcpy(diff_blob.data,softmaxblob.data,nchw * sizeof(float));
	float *diff_data = diff_blob.data, *label_data = label_blob.data;
	for(int i=0;i<batch_size;++i){
		for(int k=0;k<hw;++k)
			diff_data[int(label_data[k]) * hw + k] -= 1.0f;
		label_data += label_blob.chw();
		diff_data += diff_blob.chw();
	}
	float weight = diff_outputs[0].data[0]/batch_size;
	for(int i=0;i<nchw;++i) diff_blob.data[i] *= weight;
}
