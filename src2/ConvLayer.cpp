#include "Accelerate/Accelerate.h"
#include "stdlib.h"
#include "ConvLayer.h"
#include "im2col.h"
ConvLayer::ConvLayer(){}
ConvLayer::ConvLayer(const LayerUnit& u):Layer(u){
	float v;	
	u.geta("bias",v); bias = v;
	u.geta("group",v); group = v;
	u.geta("num",v); num = v;
	u.geta("kernel_size_h",v); kernel_size_h = v;
	u.geta("kernel_size_w",v); kernel_size_w = v;
	u.geta("pad_h",v); pad_h = v;
	u.geta("pad_w",v); pad_w = v;
	u.geta("stride_h",v); stride_h = v;
	u.geta("stride_w",v); stride_w = v;
	if(bias != (params.find("bias") != params.end())){
		printf("conv parameter wrong: bias\n");
		exit(0);
	}
}
ConvLayer::~ConvLayer(){
	if(col){
		delete []col;
		col = 0;
	}
	if(table){
		delete []table;
		table = 0;
	}
}
void ConvLayer::show(){
	Layer::show();
	printf("  (num) %d\n",num);
	printf("  (kernel) %d %d\n",kernel_size_h,kernel_size_w);
	printf("  (pad) %d %d\n",pad_h,pad_w);
	printf("  (stride) %d %d\n",stride_h,stride_w);
	printf("  (group) %d\n",group);
	printf("  (bias) %d\n",int(bias));
}
void ConvLayer::setup_outputs(){
	int ih = inputs[0].h, 
	    iw = inputs[0].w,
	    oh = i2o_floor(ih,kernel_size_h,stride_h,pad_h),
	    ow = i2o_floor(iw,kernel_size_w,stride_w,pad_w);
	outputs[0].set_shape(inputs[0].n, num, oh, ow);
	setup_outputs_data();

	int ncol = kernel_size_h * kernel_size_w * outputs[0].hw() * inputs[0].c * inputs[0].n;
	if(col) delete []col;
	col = new float[ncol];
	if(table) delete []table;
	table = new int[ncol/inputs[0].n/group];
	generate_table(inputs[0].c/group, inputs[0].h, inputs[0].w, table, kernel_size_h,kernel_size_w, stride_h,stride_w, pad_h,pad_w);
}
void ConvLayer::forward(){
	//show_inputs();
	Blob &input = inputs[0], &output = outputs[0];
	int istep = input.chw(), 
	    ncol = kernel_size_h * kernel_size_w * output.hw() * input.c,
	    w = kernel_size_h * kernel_size_w * input.c, 
	    nloc = output.hw();
	float * idata = input.data, 
	      * odata = output.data,
	      * col_data = col,
	      * weight_data = params["weight"].data;
	for(int b = 0;b < input.n; ++b){
		for(int g=0;g<group;++g){
			im2col2(idata + input.chw()/group*g, 
					table, 
					col_data + ncol/group * g, 
					kernel_size_h * kernel_size_w * outputs[0].hw() * inputs[0].c/group);
			cblas_sgemm(CblasRowMajor, 
					CblasNoTrans, CblasTrans, 
					num/group, nloc, w/group,
					1.0f,
					weight_data + g * params["weight"].nchw()/group, w/group,
					col_data + ncol/group * g, w/group,
					0.0,
					odata + output.chw()/group * g, nloc);
		}
		if(bias){
			float * bias_data = params["bias"].data;
			for(int i = 0; i < num; ++i)
			for(int j = 0; j < nloc; ++j)
				odata[i*nloc +j] += bias_data[i];
		}
		idata += istep;
		odata += output.chw();
		col_data += ncol;
	}
	//show_outputs();
}