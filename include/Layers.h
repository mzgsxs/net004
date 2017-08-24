#ifndef LAYERS_H
#define LAYERS_H
#include <map>
#include "BaseLayer.h"

class Layers{
	public:
	void add_data(const std::string& name, int n, int c, int h, int w, const std::string& method);
	void add_conv(const std::string& name, const std::vector<int>& p8, bool is_bias, const std::string& activity);
	void add_dconv(const std::string& name, const std::vector<int>& p8, bool is_bias, const std::string& activity);
	void add_pool(const std::string& name, const std::vector<int>& p3, const std::string& method);
	void add_lrn(const std::string& name, int n, float alpha, float beta);
	void add_fc(const std::string& name, int n, bool is_bias, const std::string& activity);
	void add_crop(const std::string& name, int axis, const std::vector<int>& offset, const std::vector<std::string>& names);
	void add_loss(const std::string& name, const std::string& method);
	void add_concat(const std::string& name, const std::vector<std::string>& names, const std::string& method = "channel");
	void add_activity(const std::string& name, const std::string& method, float negative_slope);
	void add_split(const std::string& name);
	void add_bn(const std::string& name,float eps);
	void add_scale(const std::string& name, bool is_bias);
	void add_softmax(const std::string& name);
	void add_proposal(const std::string& name, int feat_stride, const std::vector<std::string>& names, const std::string& method);
	void add_eltwise(const std::string& name, const std::string&l0, const std::string& l1, const std::string& method, float f0, float f1);
	void add_reshape(const std::string&name, const std::vector<int>& p4);
	void add_roipooling(const std::string&name, int h, int w, float scale, const std::vector<std::string>& names);
	void add(const std::string& name, Layer** p);

	void show();
	void show(const std::string& name);
	
	void clear();
	Layer* operator [] (const std::string& name);
	bool exist(const std::string& name);
	int count();
	int parameter_number(const std::string& name);
	int input_parameter_number(const std::string& name);
	int output_parameter_number(const std::string& name);

	private:
	std::map<std::string, Layer*> layers;
};
#endif
