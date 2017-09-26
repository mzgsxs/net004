#include <iostream>
#include <fstream>
#include "stdlib.h"
#include "ForwardBackwardRun.h"
using namespace std;
Cifar10Data::Cifar10Data(){
	type = "cifar10";
}
Cifar10Data::~Cifar10Data(){
	if(labels) delete [] labels;
	if(data) delete [] data;
}
void Cifar10Data::init(){
	// load data
	(this->*f)();

	// load label map
	string label_map_path = path + "/batches.meta.txt";
	ifstream ifile(label_map_path);
	if(!ifile.is_open()){
		printf("cannot locate file %s\n",label_map_path.c_str());
		exit(0);
	}
	string line;
	for(int i = 0; getline(ifile,line ); ++i)
		label_map[i] = line;
	//for (auto i:label_map)
	//	printf("%d %s\n",i.first,i.second.c_str());

	ifile.close();
}
std::string& Cifar10Data::label_name(int label){
	return label_map[label];
}
void Cifar10Data::fill_batch(float*& layer_data, int n, int c, int h ,int w, int index){
	index %= count;
	int left = n;
	while(left>0){
		int to_be_filled = min(count-index,left);
		memcpy(layer_data,data+index*(c*h*w),(c*h*w)*to_be_filled*sizeof(float));
		left -= to_be_filled;
		index += to_be_filled;;
	}
}
void Cifar10Data::get_labels(int*& label_data, int n, int index){
	index %= count;
	int left = n;
	while(left>0){
		int to_be_filled = min(count-index,left);
		memcpy(label_data,labels+index,to_be_filled*sizeof(int));
		left -= to_be_filled;
		index += to_be_filled;;
	}
}
void Cifar10Data::load(const std::vector<std::string>& list){
	int count = 10000, hw = 32*32, step = hw*3+1, len = count*step;
	labels = new int[count * list.size()];
	data = new float[count * list.size() * (32*32*3)];
	unsigned char * buffer = new unsigned char[len];
	int *pl = labels;
	float *pd = data;
	unsigned char *pb = buffer;
	for(const auto& l : list){
		string filepath = path + "/" + l;
		ifstream file(filepath,ios::binary);
		if(!file.is_open()){
			printf("cannot locate file %s\n",filepath.c_str());
			exit(0);
		}
		size_t extracted = file.read((char*)buffer, len).gcount();
		if(extracted != len){
			printf("cifar10 file %s incompleted\n",l.c_str());
			exit(0);
		}
		file.close();
		for(int i=0;i<count;++i){
			*pl++ = *pb++;
			for(int c=0;c<3;++c){
				float m = mean[c], s = std[c];
				float* channel_data = pd + c*hw;
				for(int j=0;j<hw;++j)
					channel_data[j] = (float(*pb++)-m)/s;
			}
			pd += 3*hw;
		}
	}
	delete []buffer;
	this->count = list.size()*count;
}
void Cifar10Data::load_train(){
	printf("init_train\n");
	load({"data_batch_1.bin", "data_batch_2.bin", "data_batch_3.bin", "data_batch_4.bin", "data_batch_5.bin"});
}
void Cifar10Data::load_test(){
	//printf("init_test\n");
	load({"test_batch.bin"});
}
ForwardBackwardRun::ForwardBackwardRun(){ }
ForwardBackwardRun::ForwardBackwardRun(const JsonValue& j):Run(j){
	if(omit) return;
	const auto& omap = j.jobj.at("attrs").jobj.at("layer_map").jobj;
	for(const auto&i: omap) layer_map[i.first] = i.second.jv.s;

	const auto& odata = j.jobj.at("attrs").jobj.at("data").jobj;
	const string& data_type = odata.at("type").jv.s;
	if(data_type == "cifar10"){
		input_data = new Cifar10Data();
		input_data->name = odata.at("name").jv.s;
		input_data->path = odata.at("path").jv.s;
		input_data->method = odata.at("method").jv.s;
		const auto& omean = odata.at("mean").jarray;
		for(const auto& i:omean) input_data->mean.push_back(i.jv.d);
		const auto& ostd = odata.at("std").jarray;
		for(const auto& i:ostd) input_data->std.push_back(i.jv.d);

		if(input_data->method == "train")
			((Cifar10Data*)input_data)->f = &Cifar10Data::load_train;
		else if(input_data->method == "test")
			((Cifar10Data*)input_data)->f = &Cifar10Data::load_test;
		else{
			printf("unknown data method %s\n",input_data->method.c_str());
			exit(0);
		}
	}
	else{
		printf("unknown data type %s\n",data_type.c_str());
		exit(0);
	}
}
void ForwardBackwardRun::show()const{
	if(omit) return;
	Run::show();
	printf("  (data)\n");
	printf("    (name) %s\n",input_data->name.c_str());
	printf("    (type) %s\n",input_data->type.c_str());
	printf("    (path) %s\n",input_data->path.c_str());
	printf("    (method) %s\n",input_data->method.c_str());
	printf("    (mean) ");
	for(int i=0;i<input_data->mean.size();++i){
		if(i!=0)printf(", ");
		printf("%lf",input_data->mean[i]);
	}
	printf("\n");
	printf("    (std) ");
	for(int i=0;i<input_data->std.size();++i){
		if(i!=0)printf(", ");
		printf("%lf",input_data->std[i]);
	}
	printf("\n");
}
void ForwardBackwardRun::check(const Net004& net)const{
	if(omit) return;
	Run::check(net);
	for(const auto& l: layer_map){
		const string& lname = l.second;
		if(net.ls.n2i.find(lname)==net.ls.n2i.end()){
			printf("cannot find layer: %s\n",lname.c_str());
			exit(0);
		}
	}
}
void ForwardBackwardRun::operator()(Net004& net, int cur) {
	if(omit) return;
	if(cur%iter_interval != 0) return;
	printf("[%d]run: train step\n",cur);
	
}
void ForwardBackwardRun::init(const Net004& net){
	if(omit) return;
	printf("init: ForwardBackwardRun\n");
	input_data->init();
}
